#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/statvfs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <stdint.h>
#include <inttypes.h>

#define BUFFER_SIZE 4096

struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    void* next_operation;
};

void copy_file(const char* src_file, const char* dest_file, int n) {
    struct aio_operation operations[n];
    struct sigevent signal_event;
    struct stat src_stat;

    // Открываем исходный файл
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        return;
    }

    // Проверяем размер исходного файла
    if (fstat(src_fd, &src_stat) < 0) {
        perror("Failed to get source file size");
        close(src_fd);
        return;
    }

    // Создаем или открываем целевой файл для записи
    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd < 0) {
        perror("Failed to create destination file");
        close(src_fd);
        return;
    }

    struct statvfs dest_statvfs;

    // Устанавливаем размер буфера в соответствии с размером кластера
    int buffer_size = BUFFER_SIZE;
    if (fstatvfs(dest_fd, &dest_statvfs) < 0) {
        perror("Failed to get cluster size");
        close(src_fd);
        close(dest_fd);
        return;
    }
    buffer_size = buffer_size - buffer_size % dest_statvfs.f_bsize;

    // Инициализируем массив структур aio_operation
    for (int i = 0; i < n; i++) {
        operations[i].buffer = malloc(buffer_size);
        operations[i].aio.aio_buf = operations[i].buffer;
        operations[i].aio.aio_sigevent.sigev_notify = SIGEV_NONE;
        operations[i].next_operation = &operations[(i + 1) % n];
    }

    // Инициализируем последнюю структуру в цепочке
    operations[n - 1].next_operation = NULL;

    // Запускаем асинхронные операции чтения-записи
    struct aio_operation* current_operation = &operations[0];
    off_t offset = 0;
    int remaining = src_stat.st_size;

    while (remaining > 0) {
        // Устанавливаем размер операции чтения или записи
        int operation_size = buffer_size;
        if (remaining < buffer_size) {
            operation_size = remaining;
        }
        current_operation->aio.aio_nbytes = operation_size;

        // Устанавливаем смещение операции
        current_operation->aio.aio_offset = offset;

        // Читаем данные из исходного файла
        if (aio_read(&current_operation->aio) < 0) {
            perror("Failed to start read operation");
            break;
        }

        // Ожидаем завершения операции чтения
        if (aio_suspend(&current_operation->aio, 1, NULL) < 0) {
            perror("Failed to suspend read operation");
            break;
        }

        // Проверяем результат чтения
        int read_bytes = aio_return(&current_operation->aio);
        if (read_bytes <= 0) {
            perror("Failed to read from source file");
            break;
        }

        // Устанавливаем размер операции записи
        current_operation->aio.aio_nbytes = read_bytes;

        // Записываем данные в целевой файл
        if (aio_write(&current_operation->aio) < 0) {
            perror("Failed to start write operation");
            break;
        }

        // Ожидаем завершения операции записи
        if (aio_suspend(&current_operation->aio, 1, NULL) < 0) {
            perror("Failed to suspend write operation");
            break;
        }

        // Проверяем результат записи
        int written_bytes = aio_return(&current_operation->aio);
        if (written_bytes <= 0) {
            perror("Failed to write to destination file");
            break;
        }

        // Обновляем оставшееся количество байт и смещение
        remaining -= written_bytes;
        offset += written_bytes;

        // Переходим к следующей операции в цепочке
        current_operation = current_operation->next_operation;
    }

    // Освобождаем ресурсы
    for (int i = 0; i < n; i++) {
        free(operations[i].buffer);
    }

    close(src_fd);
    close(dest_fd);
}

int main() {
    copy_file("./source.txt", "destination.txt", 3);

    return 0;
}

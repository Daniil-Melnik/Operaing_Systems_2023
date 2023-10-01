#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#include <stdint.h>
#include <inttypes.h>

#define BLOCK_SIZE 256 // размер блока данных

void handler(int sig, siginfo_t *si, void *ucontext) {
    // обработчик сигнала завершения операции ввода-вывода
    // здесь можно выполнять дополнительные действия по окончании операции
}

void copy_file(const char *src, const char *dest, int n) {
    int src_fd, dest_fd;
    struct aiocb *aiocb_list[n];

    // Открытие исходного файла
    src_fd = open(src, O_RDONLY);
    if (src_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Получение информации о размере исходного файла
    struct stat st;
    if (fstat(src_fd, &st) == -1) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    off_t file_size = st.st_size;

    // Открытие или создание нового файла
    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Расчет количества блоков данных
    off_t num_blocks = file_size / BLOCK_SIZE;

    // Создание и заполнение структур aiocb
    for (int i = 0; i < n; i++) {
        aiocb_list[i] = calloc(1, sizeof(struct aiocb));
        aiocb_list[i]->aio_fildes = src_fd;
        aiocb_list[i]->aio_nbytes = BLOCK_SIZE;
        aiocb_list[i]->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        aiocb_list[i]->aio_sigevent.sigev_signo = SIGRTMIN;
        aiocb_list[i]->aio_sigevent.sigev_value.sival_ptr = aiocb_list[i];

        // Установка обработчика сигнала
        struct sigaction sa;
        memset(&sa, 0, sizeof(struct sigaction));
        sa.sa_sigaction = handler;
        sa.sa_flags = SA_SIGINFO;
        sigaction(SIGRTMIN, &sa, NULL);
    }

    // Выделение памяти для буфера данных
    char *buffer = malloc(BLOCK_SIZE * n);

    // Копирование блоков данных
    for (int i = 0; i < num_blocks; i += n) {
        // Чтение данных из исходного файла
        for (int j = 0; j < n; j++) {
            aiocb_list[j]->aio_offset = i * BLOCK_SIZE + j * BLOCK_SIZE;
            aiocb_list[j]->aio_buf = buffer + j * BLOCK_SIZE;
            aio_read(aiocb_list[j]);
            printf("offset = %ld\n", aiocb_list[j]->aio_offset);
        }

        // Ожидание завершения операций чтения
        for (int j = 0; j < n; j++) {
            while (aio_error(aiocb_list[j]) == EINPROGRESS) {
                // ожидание завершения операции чтения
            }
            ssize_t read_bytes = aio_return(aiocb_list[j]);
            if (read_bytes == -1) {
                perror("aio_return");
                exit(EXIT_FAILURE);
            }
            if (read_bytes == 0) {
                break;
            }
        }

        // Запись данных в новый файл
        ssize_t write_bytes = write(dest_fd, buffer, BLOCK_SIZE * n);
        if (write_bytes == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }

    // Освобождение ресурсов
    for (int i = 0; i < n; i++) {
        free(aiocb_list[i]);
    }
    free(buffer);
    close(src_fd);
    close(dest_fd);
}

int main() {
    const char *src_file = "./1.txt";
    const char *dest_file = "./destination_4.txt";
    int n = 4; // количество перекрывающихся операций ввода-вывода

    copy_file(src_file, dest_file, n);

    return 0;
}

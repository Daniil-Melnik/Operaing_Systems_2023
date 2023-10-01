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

// Структура для хранения информации об асинхронной операции
struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    void* next_operation;
};

// Количество асинхронных операций
int n = 10;

// Размер кластера
int cluster_size = 4096;

// Функция, которая выполняет нужную асинхронную операцию
void perform_aio_operation(struct aio_operation* operation) {
    if (operation->write_operation) {
        aio_write(&operation->aio);
        //printf("buffer = %s \n", &operation->buffer);
    } else {
        aio_read(&operation->aio);
        //printf("buffer = %s \n", &operation->buffer);
    }
}

// Обработчик сигнала SIGIO
void sigio_handler(int sig, siginfo_t *info, void *context) {
    struct aio_operation* operation = (struct aio_operation*)info->si_value.sival_ptr;
    int ret;

    // Проверяем, выполнена ли операция успешно
    ret = aio_error(&operation->aio);
    if (ret != 0) {
        printf("Error %d: %s\n", ret, strerror(ret));
        return;
    }

    // Получаем количество прочитанных/записанных байт
    ret = aio_return(&operation->aio);
    if (ret == -1) {
        perror("aio_return");
        return;
    }

    // Переходим к следующей операции
    if (operation->next_operation != NULL) {
        printf("Next operation\n");
        perform_aio_operation((struct aio_operation*)operation->next_operation);
    }
}

int main(int argc, char *argv[]) {
    int input_fd, output_fd;
    struct aio_operation* first_operation = NULL;
    struct aio_operation* last_operation = NULL;

    // Проверяем правильность использования программы
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Открываем входной файл
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Success with open input\n");
    }

    // Открываем выходной файл
    output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (output_fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    else {
        printf("Success with open output\n");
    }

    // Заполняем буферы операций данными
    off_t offset = 0;
    char* buffer = malloc(cluster_size);
    int i;
    for (i = 0; i < n; i++) {
        struct aio_operation* operation = malloc(sizeof(struct aio_operation));
        memset(operation, 0, sizeof(struct aio_operation));
        operation->buffer = buffer;
        printf("buffer = %s \n", operation->buffer);
        operation->aio.aio_buf = buffer;
        operation->aio.aio_fildes = input_fd;
        operation->aio.aio_offset = offset;
        operation->aio.aio_nbytes = cluster_size;
        operation->write_operation = 0;
        operation->next_operation = NULL;

        // Меняем указатель следующей операции у предыдущей операции
        if (last_operation != NULL) {
            last_operation->next_operation = operation;
        }

        // Добавляем операцию в список операций
        if (first_operation == NULL) {
            first_operation = operation;
        }
        last_operation = operation;

        offset += cluster_size;
    }

    // Устанавливаем обработчик сигнала SIGIO
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_sigaction = sigio_handler;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    // Переключаем файловый дескриптор в неблокирующий режим
    int flags = fcntl(input_fd, F_GETFL);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    flags |= O_NONBLOCK;
    if (fcntl(input_fd, F_SETFL, flags) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
        }

    // Начинаем выполнять асинхронные операции
    perform_aio_operation(first_operation);

    // Ожидаем завершения всех операций
    while (aio_error(&last_operation->aio) == EINPROGRESS);

    // Закрываем файлы и освобождаем память
    close(input_fd);
    close(output_fd);
    free(buffer);
    free(first_operation);

    return 0;
}

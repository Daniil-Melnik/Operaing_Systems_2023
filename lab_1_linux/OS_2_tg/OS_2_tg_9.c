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

void handler(int sig, siginfo_t *si, void *ucontext) {
    // обработчик сигнала завершения операции ввода-вывода
    // здесь можно выполнять дополнительные действия по окончании операции
}

int n_menu();

int buf_menu();

void copy_file(const char *src, const char *dest, int n, int block_size, int offset_cond) {
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
    off_t num_blocks = file_size / block_size;

    // Создание и заполнение структур aiocb
    for (int i = 0; i < n; i++) {
        aiocb_list[i] = calloc(1, sizeof(struct aiocb));
        aiocb_list[i]->aio_fildes = src_fd;
        aiocb_list[i]->aio_nbytes = block_size;
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
    char *buffer = malloc(block_size * n);

    // Копирование блоков данных
    for (int i = 0; i < num_blocks; i += n) {
        // Чтение данных из исходного файла
        for (int j = 0; j < n; j++) {
            aiocb_list[j]->aio_offset = i * block_size + j * block_size;
            aiocb_list[j]->aio_buf = buffer + j * block_size;
            aio_read(aiocb_list[j]);
            if (offset_cond == 1)
                printf("offset[%d][%d] = %ld\n", i, j, aiocb_list[j]->aio_offset);
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
        ssize_t write_bytes = write(dest_fd, buffer, block_size * n);
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
    char src_file[50] = "./1.txt";
    char dest_file[50] = "./2.txt";

    printf("Введите исходный файл              : ");
    fgets(src_file, 50, stdin);

    printf("Введите целевой файл               : ");
    fgets(dest_file, 50, stdin);

    src_file[str_change(src_file)] = '\0';
    dest_file[str_change(dest_file)] = '\0';

    int offset_cond = cond_menu("Показывать сдвиг по потокам  ");

    int block_size = buf_menu(); // размер буфера в байтах
    int n = n_menu(); // количество перекрывающихся операций ввода-вывода

    /*int block_size = 512;
    int n = 4;
    int offset_cond = 1;*/

    printf ("Исходный файл      :%s\nЦелевой файл       :%s\nРазмер блока       : %d\nКоличество потоков : %d\nПоказывать сдвиг   : %d\n", src_file, dest_file, block_size, n, offset_cond);

    copy_file(src_file, dest_file, n, block_size, offset_cond);

    return 0;
}


int buf_menu()
{
    int size = 128;

    int k = 1, size_k;
    printf ("Введите размер блока: \n");
    for (int i = 128; i < 4097; i *= 2)
    {
        printf("%d) => %d MiB\n", k, i);
        k++;
    }
    printf("=>");
    scanf ("%d", &size_k);
    while ((size_k < 1) || (size_k > 6))
    {
        printf("=>");
        scanf ("%d", &size_k);
    }

    for (int i = 0; i < size_k-1; i++)
    {
        size *= 2;
    }
    return size;
}


int cond_menu(char *str)
{
    int cond = 0;
    printf("%s (0/1): ", str);
    scanf("%d", &cond);
    while ((cond != 0) && (cond != 1))
    {
        printf("Попробуйте ещё раз (0/1): ");
        scanf("%d", &cond);
    }
    return cond;
}

int n_menu()
{
    int n;

    printf("Введите количество потоков (положитльное, целое, кратное 2) : ");
    scanf ("%d", &n);
    while ((n % 2 != 0) || (n == 0))
    {
        printf("Введите количество потоков (положитльное, целое, кратное 2):");
        scanf ("%d", &n);
    }
    printf("\n");
    return n;
}

int str_change(char str[]) {
    // initializing count variable (stores the length of the string)
    int count = 0;

    // incrementing the count till the end of the string
    while (str[count] != '\n')
    {
        count++;
    }
    // returning the character count of the string
    return count;
}

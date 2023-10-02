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

#define BUFFER_SIZE 4096

/*
$ gcc -o aio_copy aio_copy.c
$ ./aio_copy input.txt output.txt
*/

struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    void* next_operation;
};

void aio_completion_handler(int signo, siginfo_t *info, void *context) {
    struct aio_operation* operation = (struct aio_operation*)info->si_value.sival_ptr;
    off_t offset = aio_offset(&operation->aio);
    ssize_t ret = aio_return(&operation->aio);

    if (ret == -1) {
        printf("AIO error: %s\n", strerror(errno));
        exit(1);
    }

    if (operation->write_operation) {
        printf("Write operation completed at offset %lld\n", (long long)offset);
    } else {
        printf("Read operation completed at offset %lld\n", (long long)offset);
    }

    free(operation->buffer);
    free(operation);
}

void create_aio_operation(struct aio_operation* operation, int fd_in, int fd_out, off_t offset, int size) {
    memset(&operation->aio, 0, sizeof(struct aiocb));
    operation->aio.aio_fildes = fd_in;
    operation->aio.aio_offset = offset;
    operation->aio.aio_buf = operation->buffer;
    operation->aio.aio_nbytes = size;
    operation->aio.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    operation->aio.aio_sigevent.sigev_signo = SIGRTMIN;
    operation->aio.aio_sigevent.sigev_value.sival_ptr = operation;

    operation->write_operation = 1;
    if (aio_read(&operation->aio) == -1) {
        printf("AIO read error: %s\n", strerror(errno));
        exit(1);
    }

    operation->aio.aio_fildes = fd_out;
    operation->write_operation = 0;
    if (aio_write(&operation->aio) == -1) {
        printf("AIO write error: %s\n", strerror(errno));
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    int fd_in = open(argv[1], O_RDONLY);
    if (fd_in == -1) {
        printf("Failed to open input file: %s\n", strerror(errno));
        return 1;
    }

    struct stat statbuf;
    if (fstat(fd_in, &statbuf) == -1) {
        printf("Failed to get input file info: %s\n", strerror(errno));
        return 1;
    }

    int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, statbuf.st_mode);
    if (fd_out == -1) {
        printf("Failed to open output file: %s\n", strerror(errno));
        return 1;
    }

    int cluster_size = statbuf.st_blksize;

    struct aio_operation* operations = NULL;

    off_t offset = 0;
    while (offset < statbuf.st_size) {
        struct aio_operation* operation = malloc(sizeof(struct aio_operation));

        operation->buffer = malloc(cluster_size);
        create_aio_operation(operation, fd_in, fd_out, offset, cluster_size);

        operation->next_operation = operations;
        operations = operation;

        offset += cluster_size;
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = aio_completion_handler;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
        printf("Failed to set signal handler: %s\n", strerror(errno));
        return 1;
    }

    while (operations != NULL) {
        struct aio_operation* operation = operations;
        operations = operation->next_operation;

        while (aio_error(&operation->aio) == EINPROGRESS) {}
    }

    close(fd_in);
    close(fd_out);

    return 0;
}

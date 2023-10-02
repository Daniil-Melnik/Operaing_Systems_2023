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

struct aio_operation {
    struct aiocb aio;
    char *buffer;
    int write_operation;
    void* next_operation;
};

void aio_completion_handler(sigval_t sigval) {
    struct aio_operation *aio_op = (struct aio_operation *) sigval.sival_ptr;

    ssize_t ret = aio_return(&aio_op->aio);
    if (ret == -1) {
        printf("Error in aio_return(): %s\n", strerror(errno));
        free(aio_op->buffer);
        free(aio_op);
        return;
    }

    if (aio_op->write_operation) {
        printf("Successfully copied %zu bytes\n", ret);
    } else {
        printf("Successfully read %zu bytes\n", ret);
    }

    if (aio_op->next_operation != NULL) {
        struct aio_operation *next_op = (struct aio_operation *) aio_op->next_operation;
        aio_op->next_operation = NULL;
        aio_read(&next_op->aio);
    } else {
        free(aio_op->buffer);
        free(aio_op);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./program input_file output_file\n");
        return 1;
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        printf("Error opening input file: %s\n", strerror(errno));
        return 1;
    }

    struct stat input_file_info;
    if (fstat(input_fd, &input_file_info) == -1) {
        printf("Error getting input file information: %s\n", strerror(errno));
        close(input_fd);
        return 1;
    }

    int output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, input_file_info.st_mode);
    if (output_fd == -1) {
        printf("Error opening output file: %s\n", strerror(errno));
        close(input_fd);
        return 1;
    }

    size_t block_size = input_file_info.st_blksize;
    char *buffer = malloc(block_size);
    if (buffer == NULL) {
        printf("Error allocating memory for buffer\n");
        close(input_fd);
        close(output_fd);
        return 1;
    }

    struct aio_operation *read_op = malloc(sizeof(struct aio_operation));
    if (read_op == NULL) {
        printf("Error allocating memory for aio_operation\n");
        free(buffer);
        close(input_fd);
        close(output_fd);
        return 1;
    }

    read_op->buffer = buffer;
    read_op->write_operation = 0;
    read_op->next_operation = NULL;

    struct aiocb *read_aio = &read_op->aio;
    memset(read_aio, 0, sizeof(struct aiocb));
    read_aio->aio_fildes = input_fd;
    read_aio->aio_buf = buffer;
    read_aio->aio_nbytes = block_size;
    read_aio->aio_offset = 0;
    read_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
    read_aio->aio_sigevent.sigev_notify_function = aio_completion_handler;
    read_aio->aio_sigevent.sigev_value.sival_ptr = read_op;

    struct aio_operation *write_op = malloc(sizeof(struct aio_operation));
    if (write_op == NULL) {
        printf("Error allocating memory for aio_operation\n");
        free(buffer);
        free(read_op);
        close(input_fd);
        close(output_fd);
        return 1;
    }

    write_op->buffer = buffer;
    write_op->write_operation = 1;
    write_op->next_operation = NULL;

    struct aiocb *write_aio = &write_op->aio;
    memset(write_aio, 0, sizeof(struct aiocb));
    write_aio->aio_fildes = output_fd;
    write_aio->aio_buf = buffer;
    write_aio->aio_offset = 0;
    write_aio->aio_sigevent.sigev_notify = SIGEV_THREAD;
    write_aio->aio_sigevent.sigev_notify_function = aio_completion_handler;
    write_aio->aio_sigevent.sigev_value.sival_ptr = write_op;

    aio_read(read_aio);
    aio_write(write_aio);

    while (aio_error(read_aio) == EINPROGRESS || aio_error(write_aio) == EINPROGRESS)
    {
        usleep(10000);
    }

    close(input_fd);
    close(output_fd);

    return 0;
}

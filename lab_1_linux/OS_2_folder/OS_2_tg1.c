#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define BUFFER_SIZE 4096

void copy_file_async(const char* src_file, const char* dest_file) {
    int src_fd = open(src_file, O_RDONLY);
    if (src_fd == -1) {
        perror("Error opening source file");
        return;
    }

    int dest_fd = open(dest_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("Error opening destination file");
        close(src_fd);
        return;
    }

    struct aiocb read_aiocb, write_aiocb;
    char buffer[BUFFER_SIZE];

    memset(&read_aiocb, 0, sizeof(struct aiocb));
    memset(&write_aiocb, 0, sizeof(struct aiocb));

    read_aiocb.aio_fildes = src_fd;
    read_aiocb.aio_buf = buffer;
    read_aiocb.aio_nbytes = BUFFER_SIZE;

    write_aiocb.aio_fildes = dest_fd;
    write_aiocb.aio_nbytes = BUFFER_SIZE;

    while (aio_read(&read_aiocb) == -1) {
        if (errno != EAGAIN && errno != EINTR) {
            perror("Error initiating async read");
            close(src_fd);
            close(dest_fd);
            return;
        }
    }

    while (1) {
        int completed_ops = 0;

        while (aio_error(&read_aiocb) == EINPROGRESS);

        ssize_t bytes_read = aio_return(&read_aiocb);
        if (bytes_read < 0) {
            perror("Error reading from source file");
            close(src_fd);
            close(dest_fd);
            return;
        }
        else if (bytes_read == 0) {
            break;  // Reached end of the file
        }

        write_aiocb.aio_buf = buffer;
        write_aiocb.aio_nbytes = bytes_read;
        while (aio_write(&write_aiocb) == -1) {
            if (errno != EAGAIN && errno != EINTR) {
                perror("Error initiating async write");
                close(src_fd);
                close(dest_fd);
                return;
            }
        }

        while (aio_error(&write_aiocb) == EINPROGRESS);

        ssize_t bytes_written = aio_return(&write_aiocb);
        if (bytes_written < 0) {
            perror("Error writing to destination file");
            close(src_fd);
            close(dest_fd);
            return;
        }

        if (bytes_written != bytes_read) {
            printf("Error: Bytes written (%zd) doesn't match bytes read (%zd)\n", bytes_written, bytes_read);
            close(src_fd);
            close(dest_fd);
            return;
        }

        memset(&read_aiocb, 0, sizeof(struct aiocb));

        read_aiocb.aio_fildes = src_fd;
        read_aiocb.aio_buf = buffer;
        read_aiocb.aio_nbytes = BUFFER_SIZE;

        while (aio_read(&read_aiocb) == -1) {
            if (errno != EAGAIN && errno != EINTR) {
                perror("Error initiating async read");
                close(src_fd);
                close(dest_fd);
                return;
            }
        }
    }

    close(src_fd);
    close(dest_fd);

    printf("File copied successfully\n");
}

int main() {
    const char* src_file = "./source.txt";
    const char* dest_file = "./destination.txt";

    copy_file_async(src_file, dest_file);

    return 0;
}

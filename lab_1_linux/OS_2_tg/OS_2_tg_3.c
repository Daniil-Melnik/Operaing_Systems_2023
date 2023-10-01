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

void copy_file(const char* src_file_path, const char* dest_file_path, int num_operations, size_t cluster_size) {
    int src_file = open(src_file_path, O_RDONLY);
    if (src_file == -1) {
        perror("Failed to open source file");
        return;
    }

    int dest_file = open(dest_file_path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_file == -1) {
        perror("Failed to open destination file");
        close(src_file);
        return;
    }

    struct stat src_file_stat;
    if (fstat(src_file, &src_file_stat) == -1) {
        perror("Failed to get source file size");
        close(src_file);
        close(dest_file);
        return;
    }

    size_t src_file_size = src_file_stat.st_size;

    // Calculate the number of clusters in the source file
    size_t num_clusters = (src_file_size + cluster_size - 1) / cluster_size;

    struct aio_operation* operations = malloc(sizeof(struct aio_operation) * num_operations);

    off_t offset = 0;
    for (size_t i = 0; i < num_clusters; i++) {
        size_t remaining_bytes = src_file_size - offset;
        size_t bytes_to_read = remaining_bytes < cluster_size ? remaining_bytes : cluster_size;

        // Initialize the aio control block for the current operation
        memset(&(operations[i].aio), 0, sizeof(struct aiocb));
        operations[i].aio.aio_fildes = src_file;
        operations[i].aio.aio_buf = operations[i].buffer;
        operations[i].aio.aio_nbytes = bytes_to_read;
        operations[i].aio.aio_offset = offset;

        // Allocate buffer for the current operation
        operations[i].buffer = malloc(bytes_to_read);

        if (i != num_clusters - 1) {
            operations[i].write_operation = 1; // Set as write operation for all operations except the last one
        } else {
            operations[i].write_operation = 0; // Set as read operation for the last operation
        }

        if (i != num_clusters - 1) {
            operations[i].next_operation = &(operations[i + 1]); // Set the next operation as the next struct in the array
        } else {
            operations[i].next_operation = NULL; // Set the last operation's next_operation as NULL
        }

        // Start the aio operation
        if (aio_read(&(operations[i].aio)) == -1) {
            perror("Failed to start aio operation");
            close(src_file);
            close(dest_file);
            free(operations);
            return;
        }

        offset += cluster_size;
    }

    // Wait for the last operation to complete
    while (aio_error(&(operations[num_clusters - 1].aio)) == EINPROGRESS) {
        // Allow the calling thread to do some other work while waiting
        usleep(100);
    }

    // Copy the read data to the destination file
    struct aio_operation* current_operation = &(operations[0]);
    while (current_operation != NULL) {
        if (current_operation->write_operation) {
            ssize_t bytes_written = aio_write(dest_file, current_operation->buffer, current_operation->aio.aio_nbytes);
            if (bytes_written == -1) {
                perror("Failed to write data to destination file");
                break;
            }

            if (aio_error(&(current_operation->aio)) != 0) {
                perror("Error occurred during aio operation");
                break;
            }

            aio_return(&(current_operation->aio));
            }

        current_operation = current_operation->next_operation;
    }

    // Free resources
    for (size_t i = 0; i < num_clusters; i++) {
        free(operations[i].buffer);
    }
    free(operations);

    // Close the files
    close(src_file);
    close(dest_file);
}

int main() {
    const char* src_file_path = "source.txt";
    const char* dest_file_path = "destination.txt";
    int num_operations = 4;
    size_t cluster_size = 4096;

    copy_file(src_file_path, dest_file_path, num_operations, cluster_size);

    return 0;
}

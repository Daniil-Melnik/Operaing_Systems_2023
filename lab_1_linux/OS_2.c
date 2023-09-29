/* aio_cp.c */

#include <unistd.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define INPUT_FILE "./1.txt"
#define OUTPUT_FILE "./2.txt"

#define XFER_SIZE 1024
#define MAX 3

void populate_aiocb(struct aiocb *aio, int fd, off_t offset,

                    int bytes, char *buf){

  aio->aio_fildes = fd;
  aio->aio_offset = offset;
  aio->aio_sigevent.sigev_notify = SIGEV_NONE;
  aio->aio_nbytes = bytes;
  aio->aio_buf = buf;
}

int main(){

  int fd_r , fd_w;
  struct aiocb a_write, a_read;
  struct aiocb *cblist[MAX];
  int err_r, err_w;
  int read_n = 0;
  int quit = 0;
  char read_buf[XFER_SIZE];
  char write_buf[XFER_SIZE];

  fd_r = open(INPUT_FILE, O_RDONLY, 0444);
  fd_w = open(OUTPUT_FILE, O_CREAT | O_WRONLY, 0644);

  memset(&a_write, 0 , sizeof(struct aiocb));
  memset(&a_read, 0 , sizeof(struct aiocb));
  populate_aiocb(&a_read, fd_r, 0, XFER_SIZE, read_buf);
  populate_aiocb(&a_write, fd_w, 0, XFER_SIZE, write_buf);
  aio_read(&a_read);

  while((err_r = aio_error(&a_read)) == EINPROGRESS){

    cblist[0] = &a_read;

    aio_suspend(cblist, 1, NULL);

  }

  if (err_r == 0){
    read_n = aio_return(&a_read);
    if (read_n == XFER_SIZE)
      a_read.aio_offset += XFER_SIZE;
    else {
      printf("Source file size < %d\n", XFER_SIZE);
      exit(1);
    }
  }

  while(1){
    memcpy(write_buf, read_buf, read_n);
    a_write.aio_nbytes = read_n;
    aio_write(&a_write);
    a_read.aio_nbytes = XFER_SIZE;
    aio_read(&a_read);

    while((err_r = aio_error(&a_read)) == EINPROGRESS ||
          (err_w = aio_error(&a_write)) == EINPROGRESS){
      cblist[0] = &a_read;
      cblist[1] = &a_write;
      aio_suspend(cblist, 2, NULL);
    }

    if (quit)
      break;
    a_write.aio_offset += aio_return(&a_write);
    read_n = aio_return(&a_read);
    if (read_n == XFER_SIZE)
      a_read.aio_offset += XFER_SIZE;
    else
      quit = 1;
  }

  /* Очистка */

  close(fd_r);
  close(fd_w);

}

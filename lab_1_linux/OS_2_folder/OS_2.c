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

#define MAX 3


/* Функция для заполнения aiocb значениями */

void populate_aiocb(struct aiocb *aio, int fd, off_t offset,

                    int bytes, char *buf){
  aio->aio_fildes = fd;
  aio->aio_offset = offset;

  /*

   * Мы не используем здесь механизм уведомления, чтобы уделить

   * больше внимания интерфейсам AIO. Для получения уведомления

   * после завершения AIO мы можем использовать механизмы

   * уведомления либо SIGEV_SIGNAL, либо SIGEV_THREAD

   */
  aio->aio_sigevent.sigev_notify = SIGEV_NONE;
  aio->aio_nbytes = bytes;
  aio->aio_buf = buf;

}

/*

 * Это приложение копирует один файл в другой

 */

 int is_true(int point);

int main(){
    int BLOCK_SIZE;
    int s;
    printf("\nВыберите объём класера: ");
    printf("\n1) 512");
    printf("\n2) 1024");
    printf("\n3) 2048");
    printf("\n4) 4096");
    printf("\n5) 8192");
    printf("\n6) 16384");
    printf("\n7) 32768");
    printf("\n8) 65536");
    printf("\n9) 131072");
    printf("\n10) 262144\n");
    printf("\n=>");
    scanf("%d", &s);
    while (is_true(s) == 0)
    {
        printf("\nТакого варианта нетб попробуйте ещё: ");
        printf("\nВыберите объём класера: ");
        printf("\n1) 512");
        printf("\n2) 1024");
        printf("\n3) 2048");
        printf("\n4) 4096");
        printf("\n5) 8192");
        printf("\n6) 16384");
        printf("\n7) 32768");
        printf("\n8) 65536");
        printf("\n9) 131072");
        printf("\n10) 262144\n");
        printf("\n=>");
        scanf("%d", &s);
    }
    switch (s)
    {
    case 1:
        BLOCK_SIZE = 512;
        break;
    case 2:
        BLOCK_SIZE = 1024;
        break;
    case 3:
        BLOCK_SIZE = 2048;
        break;
    case 4:
        BLOCK_SIZE = 4096;
        break;
    case 5:
        BLOCK_SIZE = 8192;
        break;
    case 6:
        BLOCK_SIZE = 16384;
        break;
    case 7:
        BLOCK_SIZE = 32768;
        break;
    case 8:
        BLOCK_SIZE = 65536;
        break;
    case 9:
        BLOCK_SIZE = 131072;
        break;
    case 10:
        BLOCK_SIZE = 262144;
        break;
    default:
        BLOCK_SIZE = 512;
        break;
    }
    printf("\nВыбран размер кластера: %d\n", BLOCK_SIZE);


  /* Файловые дескрипторы чтения/записи */
  int fd_r , fd_w;

  /* Блоки управления AIO для чтения и записи */
  struct aiocb a_write, a_read;

  /*

   * Этот список используется для хранения блоков

   * управления исходящих запросов чтения или записи

   */

  struct aiocb *cblist[MAX];

  /* Статус операции чтения или записи */
  int err_r, err_w;

  /* число на самом деле считанных байтов */
  int read_n = 0;

  /* Метка завершения потока для файла-источника */
  int quit = 0;

  /* Используются для передачи данных между источником

   * и файлом назначения

   */
  char read_buf[BLOCK_SIZE];
  char write_buf[BLOCK_SIZE];

  /*

   * Открываем файлы источника и места назначения. Вызываем

   * функцию populate_aiocb, чтобы проинициализировать блоки

   * управления AIO для операции чтения и записи. Хорошей

   * практикой является очистка блоков aiocb перед

   * их использованием

   */

  fd_r = open(INPUT_FILE, O_RDONLY, 0444);
  fd_w = open(OUTPUT_FILE, O_CREAT | O_WRONLY, 0644);

  memset(&a_write, 0 , sizeof(struct aiocb));
  memset(&a_read, 0 , sizeof(struct aiocb));

  /* Заполняем блоки aiocb значениями по умолчанию */

  populate_aiocb(&a_read, fd_r, 0, BLOCK_SIZE, read_buf);

  populate_aiocb(&a_write, fd_w, 0, BLOCK_SIZE, write_buf);

  /*

   * Запускаем асинхронное чтение из файла-источника с помощью

   * функции aio_read. Эта функция читает a_read.aio_nbytes

   * байтов из файла a_read.aio_fildes, начиная со смещения

   * a_read.aio_offset в буфер a_read.aio_buf. В случае успеха

   * возвращается 0. Эта функция возвращается сразу после

   * помещения запроса в очередь.

   */

  aio_read(&a_read);

  /*

   * Ожидаем завершения чтения. После старта любой асинхронной

   * операции (чтения или записи), можно получить её статус с

   * помощью функции aio_error. Если запрос не завершён, эта

   * функция возвращает EINPROGRESS, если запрос завершён

   * успешно, она возвращает 0, в противном случае возвращается

   * значение ошибки. Если aio_read возвращает EINPROGRESS, вызов

   * aio_suspend ожидает завершения операции.

   */

  while((err_r = aio_error(&a_read)) == EINPROGRESS){

    /*

     * Функция aio_suspend приостанавливает вызывающий процесс,

     * пока не завершится по крайней мере один из асинхронных

     * запросов ввода/вывода в списке cblist, или не будет

     * доставлен сигнал. Здесь мы ждём завершения aio_read

     * для блока a_read.

     */
    cblist[0] = &a_read;
    aio_suspend(cblist, 1, NULL);
  }

  /*

   * Если возвращаемое значение функции aio_error равно 0, то

   * операция чтения была успешной. Вызываем aio_return, чтобы

   * узнать количество прочитанных байтов. Функция должна быть

   * вызвана только один раз после того, как aio_error

   * возвращает нечто, отличное от EINPROGRESS.

   */

  if (err_r == 0){
    read_n = aio_return(&a_read);
    if (read_n == BLOCK_SIZE)
      /* Смещениями должны управлять мы */
      a_read.aio_offset += BLOCK_SIZE;
    else {

      /*
       * Для простоты предположим, что размер файла-источника
       * больше, чем XFER_SIZE
       */
      printf("Source file size < %d\n", BLOCK_SIZE);
      exit(1);
    }
  }

  /*
   * В этом цикле мы копируем данные, считанные выше, в буфер
   * записи и запускаем операцию асинхронной записи. Также,
   * думаем вперёд и помещаем в очередь запрос на чтение для
   * следующего цикла.
   */

  while(1){
    memcpy(write_buf, read_buf, read_n);

    /*
     * Настраиваем блок управления записи. Чтобы поместить в
     * очередь запрос на запись вызываем aio_write. Эта функция
     * запишет a_write.aio_nbytes байтов из буфера
     * a_write.aio_buf в файл a_write.aio_fildes со смещением
     * a_write.aio_offset. В случае успеха возвращает 0.
     */
    a_write.aio_nbytes = read_n;
    aio_write(&a_write);

    /* Помещаем в очередь следующий запрос на чтение */
    a_read.aio_nbytes = BLOCK_SIZE;
    aio_read(&a_read);

    /*
     * Перед началом обработки ожидаем завершения и чтения,
     * и записи
     */
    while((err_r = aio_error(&a_read)) == EINPROGRESS ||
          (err_w = aio_error(&a_write)) == EINPROGRESS){
      cblist[0] = &a_read;
      cblist[1] = &a_write;
      aio_suspend(cblist, 2, NULL);
    }

    /* Это конец? */
    if (quit)
      break;

    /* Увеличиваем на единицу указатель записи */
    a_write.aio_offset += aio_return(&a_write);

    /* Увеличиваем на единицу указатель чтения */
    read_n = aio_return(&a_read);
    if (read_n == BLOCK_SIZE)
      a_read.aio_offset += BLOCK_SIZE;
    else
      /* Это последний блок */
      quit = 1;

  }



  /* Очистка */

  close(fd_r);

  close(fd_w);

}

int is_true(int point)
{
    int us = 0;
    for (int i = 0; i <= 10; i++)
    {
        if (point == i)
            us = 1;
    }
    return us;
}

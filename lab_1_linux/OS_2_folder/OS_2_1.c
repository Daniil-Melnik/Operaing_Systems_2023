/*
 * Command line to build the program:
 * cc -o aio_copy aio_copy.c -laio -pthread
 */

           /* * * *  aio_copy.c  * * * */

#include <unistd.h>
#include <aio.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <signal.h>
#include <errno.h>
#include <malloc.h>

#define BUF_CNT 2     /* number of buffers */

/* To run completion code in the signal handler, define the following: */
#define COMPLETION_IN_HANDLER

struct sigaction sig_act;
volatile int sigcnt = 0;
volatile int total = 0;

           /* * * *  Signal handler * * * */

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
void sig_action(signo,info,context)
int signo;
siginfo_t *info;
void *context;
{
        printf("Entered sig_action\n");
        printf("  signo = %d \n",signo);
        printf("  si_code  = %d \n",info->si_code);

#ifndef COMPLETION_IN_HANDLER
        printf("  si_value.sival_int = %d decimal\n",
               info->si_value.sival_int);

#else
        printf("  si_value.sival_ptr = %lx hex \n",info->si_value.sival_ptr);

        /* Call aio_error and aio_return from the signal handler.
         * Note that si_value is the address of the write aiocb.
         */
        while (aio_error((struct aiocb *)info->si_value.sival_ptr) ==
               EINPROGRESS);

        /* * * * Update total bytes written to set new file offset * * * */
        total += aio_return((struct aiocb *)info->si_value.sival_ptr);
#endif

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
        sigcnt++;
        return;
}

void sig_handler(signo)
int signo;
{
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
        printf("Entered sig_handler, signo = %d \n",signo);
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
        sigcnt++;
        return;
}

           /* * * * Main Routine * * * */

main(int argc, char **argv)
{
        int             in_file, out_file, rec_cnt = 0;
        typedef char   *buf_p;
        buf_p           buf[BUF_CNT];
        aiocb_t         a_write;
        size_t          xfer_size;
        int             buf_index, ret;

        /* * * * Check number of input arguments * * * */

        if (argc < 4) {
         fprintf(stderr, "Usage: %s input-file output-file buf-size-in-Kb\n",
               argv[0]);
          exit(0);
        }

           /* * * * Open input file * * * */

        if ((in_file = open(argv[1], O_RDONLY)) == -1) {
                perror(argv[1]);
                exit(errno);
        }
        printf("Opened Input File\n");

           /* * * * Open output file * * * */

        /* If O_APPEND is added to flags, all writes will appear at end */
        if ((out_file = open(argv[2], O_WRONLY|O_CREAT, 0777)) == -1) {
                perror(argv[2]);
                exit(errno);
        }
        printf("Opened Output File \n");

           /* * * * Calculate transfer size (# bufs * 1024) * * * */

        xfer_size = atol(argv[3]) * 1024;

           /* * * * Allocate buffers for file copy * * * */

        for (buf_index = 0; buf_index < BUF_CNT; buf_index++)
                buf[buf_index] = (buf_p) malloc(xfer_size);

        buf_index = 0;

           /* * * *  Init. signal action structure for SIGUSR1  * * * */

/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
        sigemptyset(&sig_act.sa_mask);  /* block only current signal */

        /*  If the SA_SIGINFO flag is set in the sa_flags field then
         *  the sa_sigaction field of sig_act structure specifies the
         *  signal catching function:
         */
        sig_act.sa_flags = SA_SIGINFO;
        sig_act.sa_sigaction = sig_action;

        /*  If the SA_SIGINFO flag is NOT set in the sa_flags field
         *  then the the sa_handler field of sig_act structure specifies
         *  the signal catching function, and the signal handler will be
         *  invoked with 3 arguments instead of 1:
         *     sig_act.sa_flags = 0;
         *     sig_act.sa_handler = sig_handler;
         */

           /* * * *  Estab. signal handler for SIGUSR1 signal * * * */

        printf("Establish Signal Handler for SIGUSR1\n");
        if (ret = sigaction (SIGUSR1, /* Set action for SIGUSR1       */
            &sig_act,                 /* Action to take on signal     */
            0))                       /* Don't care about old actions */
            perror("sigaction");
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

           /* * * * Init. aio control block (aiocb) * * * */

        a_write.aio_fildes = out_file;
        a_write.aio_offset = 0;                   /* write from current */
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
        a_write.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        a_write.aio_sigevent.sigev_signo  = SIGUSR1; /* completion signal */

#ifdef COMPLETION_IN_HANDLER
        /* Fill in a user-specified value which will be the si_value
         * component of the generated signal.  sigev_value is a union
         * of either an int (sival_int) or a void * (sival_ptr).
         * In this example, we use the sival_ptr field, and pass
         * the address of the aiocbp into the signal handler, so that
         * the signal handler can call aio_error and aio_return directly:
         */
        a_write.aio_sigevent.sigev_value.sival_ptr = &a_write;
#else
        /* Pass an integer value into the signal handler: */
        a_write.aio_sigevent.sigev_value.sival_int = 1;
#endif
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

           /* * * * Copy from in_file to out_file * * * */

        while (in_file != -1) {
                int buf_len;

           /* * * * Read next buffer of information * * * */

                buf_len = read( in_file, buf[buf_index], xfer_size);

#ifdef COMPLETION_IN_HANDLER
                if (rec_cnt) {  /* will be >1 on all but first write... */
                        aiocb_t *wait_list = &a_write;

                        /* Wait until previous write completes */
                                aio_suspend(&wait_list,1,NULL);

                }  /* if (rec_cnt) */

#else

                if (rec_cnt) {  /* will be >1 on all but first write... */

                        /* previous write completed? If not, wait */
                        while (aio_error(&a_write) == EINPROGRESS) {
                                aiocb_t *wait_list = &a_write;
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
                                /* No timeout specified */
                                aio_suspend(&wait_list,1,NULL);
                                /* aio_suspend(1, &wait_list); */
/*^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
                        }

           /* * * * Update total bytes written to set new file offset * * * */

                        total += aio_return(&a_write);
                }  /* if (rec_cnt) */
#endif

           /* * * * Check for end-of-file (won't have filled buffer) * * */

                if (buf_len <= 0)
                        break;

           /* * * * Set buffer up for next write * * * */

                a_write.aio_nbytes = buf_len;
                a_write.aio_buf = buf[buf_index];

                /* if file is opened for append, can ignore offset field */

                a_write.aio_offset = total;
                ret = aio_write(&a_write);
                if (ret) { perror ("aio_write"); exit(ret); }

           /* * * Update record count, and position to next buffer * * */

                rec_cnt++;
                buf_index ^= 1;
        }

        printf("total number of bytes written to output file = %d\n",total);

           /* * * * Close files * * * */

        close(in_file);
        printf("Closed Input File\n");
        close(out_file);
        printf("Closed Output File\n");
        printf("Copied: %d records, %d signals taken\n", rec_cnt, sigcnt);
}

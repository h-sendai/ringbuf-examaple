#include <sys/time.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "get_num.h"
#include "set_cpu.h"

#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_LOOP_COUNT 10000000

pthread_mutex_t mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  not_full  = PTHREAD_COND_INITIALIZER;
pthread_cond_t  not_empty = PTHREAD_COND_INITIALIZER;

#define MAX_BUF 10000

int buf[MAX_BUF];
int write_index = 0;
int read_index = 0;
int n_used = 0;
int writer_cpu = 1;
int reader_cpu = 2;
int debug = 0;

void *writer(void *arg)
{
    int s;
    int loop_counter = 0;

    set_cpu(writer_cpu);

    if (debug) {
        fprintf(stderr, "writer start\n");
    }
    while (loop_counter < MAX_LOOP_COUNT) {
    //for ( ; ; ) {
        s = pthread_mutex_lock(&mutex);
loop:
        if (n_used == MAX_BUF) {
            if (debug) {
                fprintf(stderr, "n_used == MAX_BUF ( %d )\n", loop_counter);
            }
            pthread_cond_wait(&not_full, &mutex);
            goto loop;
        }
        buf[write_index] = write_index;
        write_index ++;
        if (write_index >= MAX_BUF) {
            write_index = 0;
            //fprintf(stderr, "write_index full\n");
        }
        n_used ++;
        pthread_cond_signal(&not_empty);
        s = pthread_mutex_unlock(&mutex);
        // fprintf(stderr, "unlock in writer\n");
        loop_counter ++;
    }

    return NULL;
}

void *reader(void *arg)
{
    int s;
    int loop_counter = 0;

    set_cpu(reader_cpu);

    if (debug) {
        fprintf(stderr, "reader start\n");
    }

    while (loop_counter < MAX_LOOP_COUNT) {
    //for ( ; ; ) {
        s = pthread_mutex_lock(&mutex);
        //fprintf(stderr, "got lock in reader\n");
loop:
        if (n_used == 0) {
            if (debug) {
                fprintf(stderr, "n_used == 0 ( %d )\n", loop_counter);
            }
            pthread_cond_wait(&not_empty, &mutex);
            goto loop;
        }
        if (buf[read_index] != read_index) {
            errx(1, "read_index error");
        }
        buf[read_index] = -1;
        read_index ++;
        if (read_index >= MAX_BUF) {
            read_index = 0;
            //fprintf(stderr, "read_index full\n");
        }
        n_used --;
        pthread_cond_signal(&not_full);
        s = pthread_mutex_unlock(&mutex);
        
        loop_counter++;
    }

    return NULL;
}


int main(int argc, char *argv[])
{
    int c;
    int s;
    pthread_t reader_id;
    pthread_t writer_id;

    while ( (c = getopt(argc, argv, "c:C:d")) != -1) {
        switch (c) {
            case 'c':
                writer_cpu = get_num(optarg);
                break;
            case 'C':
                reader_cpu = get_num(optarg);
                break;
            case 'd':
                debug = 1;
                break;
        }
    }
    argc -= optind;
    argv += optind;

    s = pthread_create(&reader_id, 0, reader, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }
    s = pthread_create(&writer_id, 0, writer, NULL);
    if (s != 0) {
        handle_error_en(s, "pthread_create");
    }

    s = pthread_join(writer_id, 0);
    if (s != 0) {
        handle_error_en(s, "pthread_join for writer");
    }
    s = pthread_join(reader_id, 0);
    if (s != 0) {
        handle_error_en(s, "pthread_join for writer");
    }

    return 0;
}

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>
#include <stdint.h>


void timespec_add(struct timespec *res, struct timespec *a, struct timespec *b);
void timespec_subtract(struct timespec *res, struct timespec *a, struct timespec *b);
void log_time();
void create_logger();
void destroy_logger();


FILE *log_file;


int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage ./%s <samples_num> <total_time>\n", argv[0]);
        exit(-1);
    }

    long samples = atol(argv[1]);    // Number of samples requested.
    int sampling_time = atoi(argv[2]);  // Sampling frame in seconds.

    long total_time = sampling_time * 1000;  // Sampling frame in ms.
    // Assert that period is integral multiplier of samples.
    long period = total_time / samples;

    // Express period in struct timespec format.
    struct timespec period_spec;
    period_spec.tv_sec = period / 1000;  // ms -> sec
    period_spec.tv_nsec = (period % 1000) * 1000000;  // ms -> nsec

    // printf("period: %ld secs %ld nsecs\n", (long) period_spec.tv_sec, period_spec.tv_nsec);

    create_logger();

    struct timespec target;
    clock_gettime(CLOCK_MONOTONIC, &target);
    // printf("init_time: %ld secs %ld nsecs\n", (long) target.tv_sec, target.tv_nsec);
    timespec_add(&target, &target, &period_spec);

    struct timespec cur_time;
    struct timespec diff;
    // struct timespec sleep_time;

    for (long i = 0; i < samples; i++) {
        // printf("target: %ld secs %ld nsecs\n", (long) target.tv_sec, target.tv_nsec);
        clock_gettime(CLOCK_MONOTONIC, &cur_time);
        // printf("cur_time: %ld secs %ld nsecs\n", (long) cur_time.tv_sec, cur_time.tv_nsec);
        timespec_subtract(&diff, &target, &cur_time);

        // printf("diff: %ld secs %ld nsecs\n", (long) diff.tv_sec, diff.tv_nsec);
        if (diff.tv_sec < 0 || diff.tv_nsec < 0) printf("In past.\n");

        int rc = nanosleep(&diff, NULL);
        if (rc != 0) {
            perror("Nanosleep not completed properly.\n");
        }

        // Do work here
        log_time();

        timespec_add(&target, &target, &period_spec);
    }

    destroy_logger();

    return 0;
}

void timespec_add(struct timespec *res, struct timespec *a, struct timespec *b)
{
    res->tv_nsec = a->tv_nsec + b->tv_nsec;
    res->tv_sec = a->tv_sec + b->tv_sec;
    if (res->tv_nsec >= 1000000000) {
        res->tv_nsec -= 1000000000;
        res->tv_sec += 1;
    }
}

void timespec_subtract(struct timespec *res, struct timespec *a, struct timespec *b)
{
    if (a->tv_nsec < b->tv_nsec) {
        res->tv_nsec = a->tv_nsec - b->tv_nsec + 1000000000;
        res->tv_sec = a->tv_sec - b->tv_sec - 1;
    }
    else {
        res->tv_nsec = a->tv_nsec - b->tv_nsec;
        res->tv_sec = a->tv_sec - b->tv_sec;
    }
}

void create_logger()
{
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    char filename[256];
    sprintf(&filename, "timestamps_%ld", (long) cur_time.tv_sec);

    log_file = fopen(filename, "w");
    if (log_file == NULL) {
        perror("Failed to open log file.\n");
        exit(-1);
    }
}

void destroy_logger()
{
    fclose(log_file);
}

void log_time()
{
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);

    // fprintf(log_file, "%ld secs %ld usecs\n", (long) cur_time.tv_sec, cur_time.tv_usec);

    int64_t secs = (int64_t) cur_time.tv_sec;
    int64_t usecs = (int64_t) cur_time.tv_usec;
    fwrite(&secs, sizeof(int64_t), 1, log_file);
    fwrite(&usecs, sizeof(int64_t), 1, log_file);

    // struct timespec cur_time_s;
    // clock_gettime(CLOCK_MONOTONIC, &cur_time_s);
    // fprintf(log_file, "mono: %ld secs %ld nsecs\n", (long) cur_time_s.tv_sec, cur_time_s.tv_nsec);

    fflush(log_file);
}

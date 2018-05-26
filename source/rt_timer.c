/**
 * rt_timer.c
 *
 * Created by Dimitrios Karageorgiou,
 *  for course "Embedded And Realtime Systems".
 *  Electrical and Computers Engineering Department, AuTh, GR - 2017-2018
 *
 * A simple implementation of a real time timer that avoids shifting.
 *
 * This demo at each timeout of the timer logs the returned value of
 * gettimeofday() into a file named 'timestamps_<timestamp>'
 *
 * Usage:
 *  ./executable <N> <Toverall>
 * where:
 *  -N : Number of samples to get.
 *  -Toverall : Overall time during which N samples will be taken. Time distance
 *          between each sample is going to be Toverall/N.
 *
 * version: 0.1
 */

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


FILE *log_file;  // File for logging of timestamps.


int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage ./%s <samples_num> <total_time>\n", argv[0]);
        exit(-1);
    }

    long samples = atol(argv[1]);       // Number of samples requested.
    int sampling_time = atoi(argv[2]);  // Sampling frame in seconds.

    long total_time = sampling_time * 1000;  // Sampling frame in ms.
    // Assert that period is integral multiplier of samples.
    long period = total_time / samples;

    // Express period in struct timespec format.
    struct timespec period_spec;
    period_spec.tv_sec = period / 1000;  // ms -> sec
    period_spec.tv_nsec = (period % 1000) * 1000000;  // ms -> nsec

    create_logger();

    struct timespec target;     // Target time for next timeout.
    struct timespec cur_time;   // Current time.
    struct timespec diff;       // Difference between current time and target.

    // Set first target to current time plus a period.
    clock_gettime(CLOCK_MONOTONIC, &target);
    timespec_add(&target, &target, &period_spec);

    for (long i = 0; i < samples; i++) {
        // Calculate the difference between current time and target.
        clock_gettime(CLOCK_MONOTONIC, &cur_time);
        timespec_subtract(&diff, &target, &cur_time);

        if (diff.tv_sec < 0 || diff.tv_nsec < 0) printf("In past.\n");

        // Sleep for calculated diff time.
        int rc = nanosleep(&diff, NULL);
        if (rc != 0) {
            perror("Nanosleep not completed properly.\n");
        }

        // Do work here.
        log_time();

        // Set next target. Everything is integral, so no error accumulation.
        timespec_add(&target, &target, &period_spec);
    }

    destroy_logger();

    return 0;
}

/**
 * Adds two timespec structs.
 *
 * Operation: res = a + b
 *
 * Parameters:
 *  -a : First operand.
 *  -b : Second operand.
 *  -res : Target in which the result will be written.
 */
void timespec_add(struct timespec *res, struct timespec *a, struct timespec *b)
{
    res->tv_nsec = a->tv_nsec + b->tv_nsec;
    res->tv_sec = a->tv_sec + b->tv_sec;
    if (res->tv_nsec >= 1000000000) {
        res->tv_nsec -= 1000000000;
        res->tv_sec += 1;
    }
}

/**
 * Subtracts two timespec structs.
 *
 * Operation: res = a - b
 *
 * Parameters:
 *  -a : First operand.
 *  -b : Second operand.
 *  -res : Target in which the result will be written.
 */
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

/**
 * Initializes global log_file file stream.
 */
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

/**
 * Destroys global log_file file stream.
 */
void destroy_logger()
{
    fclose(log_file);
}

/**
 * Logs the value of gettimeofday() into global log_file file stream.
 */
void log_time()
{
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);

    // Make sure that 64-bit ints are used.
    int64_t secs = (int64_t) cur_time.tv_sec;
    int64_t usecs = (int64_t) cur_time.tv_usec;

    fwrite(&secs, sizeof(int64_t), 1, log_file);
    fwrite(&usecs, sizeof(int64_t), 1, log_file);
    fflush(log_file);
}

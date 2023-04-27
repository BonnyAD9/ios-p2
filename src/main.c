#include <inttypes.h>  // SIZE_MAX, size_t
#include <stdlib.h>    // strtol, EXIT_FAILURE, EXIT_SUCCESS, srand, rand, NULL
#include <unistd.h>    // fork, usleep
#include <ctype.h>     // isspace
#include <sys/wait.h>  // wait, WIFEXITED, WEXITSTATUS
#include <time.h>      // time
#include <string.h>

#include "logger.h"   // eprintf, init_log_file, close_log_file, wprintf
#include "mem_mgr.h"  // mmgr_init, mmgr_close, pid_t, mmgr_g_queue,
                      // mmgr_r_queue, mmgr_queue, kill, SIGUSR2,
                      // mmgr_a_active_clerks
#include "customer.h" // customer_main
#include "clerk.h"    // clerk_main

// inclusive
#define TZ_MAX 10000
#define TU_MAX 100
#define  F_MAX 10000

#define LOG_FILENAME "proj2.out"

/**
 * @brief Converts the string to positive number.
 *
 * @param str string to parse
 * @param min inclusive allowed minimum
 * @param max inclusive allowed maximum
 * @param out where to store the number
 * @return _Bool true if the whole string is valid number, otherwise false
 */
static _Bool parse_num(const char *str, size_t min, size_t max, size_t *out);

/**
 * @brief Waits for all childern
 *
 * @return size_t number of childern that exited with error
 */
static size_t wait4all_childern(void);

int main(int argc, char **argv) {
    // in case the process crashes this can be used to free the shared memory
    if (argc == 2 && strcmp(argv[1], "--free-my-memory") == 0) {
        mmgr_close(1);
        wprintf("Memory freed, the following error is expected");
    }
    if (argc != 6)
        return eprintf("invalid number of arguments");

    mmgr_stats stats;

    // process arguments
    if (!parse_num(argv[1], 1, SIZE_MAX - 1, &stats.nz))
        return eprintf("Invalid value for argument NZ");
    if (!parse_num(argv[2], 1, SIZE_MAX - 1, &stats.nu))
        return eprintf("Invalid value for argument NU");
    if (!parse_num(argv[3], 0, TZ_MAX, &stats.tz))
        return eprintf("Invalid value for argument TZ");
    if (!parse_num(argv[4], 0, TU_MAX, &stats.tu))
        return eprintf("Invalid value for argument TU");
    if (!parse_num(argv[5], 0, F_MAX, &stats.f))
        return eprintf("Invalid value for argument F");

    // convert to microseconds
    stats.tz *= 1000;
    stats.tu *= 1000;
    stats.f  *= 1000;

    // init shared resources
    if (!mmgr_init(&stats, 1)) {
        return eprintf("Failed to create shared memory");
    }
    if (!init_log_file(LOG_FILENAME)) {
        mmgr_close(1);
        return eprintf("Failed to create log file");
    }

    // fork customers
    for (size_t i = 0; i < stats.nz; ++i) {
        pid_t pid = fork();

        if (pid == 0)
            return customer_main(i + 1);
        if (pid == -1) {
            eprintf("Failed to create customer with id %zu", i + 1);
            goto on_fail;
        }
    }

    // fork clerks
    for (size_t i = 0; i < stats.nu; ++i) {
        pid_t pid = fork();

        if (pid == 0)
            return clerk_main(i + 1);
        if (pid == -1) {
            eprintf("Failed to create clerk with id %zu", i + 1);
            goto on_fail;
        }
    }

    // sleep for (F/2 - F)
    srand(time(NULL) + 23);
    const size_t fo2 = stats.f / 2;
    usleep(rand() % fo2 + fo2);

    // close the bank
    mmgr_s_close();
    flog("closing");

    // 1 if any returned failure, otherwise 0
    int ret = (_Bool)wait4all_childern();

    close_log_file();
    mmgr_close(1);

    return ret;

    // jump here on error, frees all resources and exits with failure
on_fail:
    // have the log correct even when there is error
    mmgr_s_close();
    flog("closing");

    wait4all_childern();

    close_log_file();
    mmgr_close(1);

    return EXIT_FAILURE;
}

static void send_customers_home(void) {
    for (int i = 0; i < Q_COUNT; ++i) {
        mmgr_queue q = mmgr_g_queue(i + 1);
        pid_t pid;
        while ((pid = mmq_pop(q)))
            kill(pid, SIGUSR2);
        mmgr_r_queue(i + 1);
    }
}

static size_t wait4all_childern(void) {
    int wstatus = -1;
    size_t err_count = 0;

    sig_atomic_t ca = *mmgr_a_active_clerks();
    // if there are no active clerks, send all customers home
    // this usually happens when customer fork fails so there are no clerks
    // at all
    if (!ca)
        send_customers_home();

    while (wait(&wstatus) != -1) {
        // count the number of errors
        err_count +=
            WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != EXIT_SUCCESS;

        // if there are no active clerks, send all customers home
        if (!*mmgr_a_active_clerks())
            send_customers_home();
    }

    if (err_count)
        wprintf("%zu processes exited with failure", err_count);
    return err_count;
}

static _Bool parse_num(const char *str, size_t min, size_t max, size_t *out) {
    // check for empty string or negative value
    if (!*str || *str == '-' || isspace(*str))
        return 0;

    char *end;
    unsigned long long num = strtoull(str, &end, 10);
    *out = num;

    // check if the whole string was consumed and the value is in bounds
    return *end == 0 && num >= min && num <= max;
}

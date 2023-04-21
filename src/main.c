#include <inttypes.h> // SIZE_MAX, size_t
#include <stdlib.h>   // strtol
#include <unistd.h>   // fork
#include <ctype.h>    // isspace

#include "logger.h"  // eprintf, init_log_file, close_log_file
#include "mem_mgr.h" // mmgr_init, mmgr_close

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

int main(int argc, char **argv) {
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
        // TODO: fork
    }

    close_log_file();
    mmgr_close(1);
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

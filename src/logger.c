#include "logger.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vfprintf, stderr, FILE
#include <stdlib.h> // EXIT_FAILURE
#include <assert.h> // assert

#include "mem_mgr.h"

// ansi escape codes
#ifndef NO_COLOR

#define RESET "\x1b[0m"
#define RED "\x1b[91m"

#else // ifndef NO_COLOR

#define RESET ""
#define RED ""

#endif // ifndef NO_COLOR

static FILE *log_file = NULL;

int eprintf(const char *format, ...) {
    va_list val;
    va_start(val, format);

    fprintf(stderr, RED "error:" RESET " ");
    vfprintf(stderr, format, val);
    fprintf(stderr, "\n");

    va_end(val);

    return EXIT_FAILURE;
}

_Bool init_log_file(const char *filename) {
    if (log_file)
        fclose(log_file);
    log_file = fopen(filename, "a");
    if (!log_file)
        return 0;
    return 1;
}

void flog(const char *format, ...) {
    assert(log_file);

    size_t *ctr = mmgr_g_log_file();

    va_list val;
    va_start(val, format);

    fprintf(log_file, "%zu: ", *ctr);
    vfprintf(log_file, format, val);
    fprintf(log_file, "\n");
    // the logger expects that the contents are always flusshed
    fflush(log_file);

    va_end(val);
}

void close_log_file(void) {
    if (log_file)
        fclose(log_file);
    log_file = NULL;
}

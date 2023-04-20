#include "logger.h"

#include <stdarg.h> // va_list, va_start, va_end
#include <stdio.h>  // vfprintf, stderr
#include <stdlib.h> // EXIT_FAILURE

// ansi escape codes
#ifndef NO_COLOR

#define RESET "\x1b[0m"
#define RED "\x1b[91m"

#else // ifndef NO_COLOR

#define RESET ""
#define RED ""

#endif // ifndef NO_COLOR


int eprintf(const char *format, ...) {
    va_list val;
    va_start(val, format);

    fprintf(stderr, RED "error:" RESET " ");
    vfprintf(stderr, format, val);
    fprintf(stderr, "\n");

    va_end(val);

    return EXIT_FAILURE;
}

void init_log_file(const char *filename);

void log(const char *format, ...);

void close_log_file();

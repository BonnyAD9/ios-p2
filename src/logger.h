// useful helpers for the second project

#ifndef LOGGER_INCLUDED
#define LOGGER_INCLUDED

/**
 * @brief
 * synchronously prints "error: " in red followed by the given message and
 * newline to stderr.
 *
 * @param format the message to print
 * @param ... additional arguments to the message
 * @return int EXIT_FAILURE
 */
int eprintf(const char *format, ...);

/**
 * @brief
 * synchronously prints "warning: " in magenta followed by the given message
 * and newline to stderr.
 *
 * @param format the message to print
 * @param ... additional arguments to the message
 */
void wprintf(const char *format, ...);

/**
 * @brief Set the log file, called for every process
 *
 * @param filename path to the log file
 * @return _Bool true on success, otherwise false
 */
_Bool init_log_file(const char *filename);

/**
 * @brief writes message to the log file shared by multiple processes
 *
 * @param format message to write
 * @param ...
 */
void flog(const char *format, ...);

/**
 * @brief closes the shared log file opened by set_log_file
 *
 */
void close_log_file(void);

#endif // LOGGER_INCLUDED

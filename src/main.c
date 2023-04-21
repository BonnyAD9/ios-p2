#include <inttypes.h> // SIZE_MAX, size_t
#include <stdlib.h>   // strtol

#include "logger.h" // eprintf

// inclusive
#define TZ_MAX 10000
#define TU_MAX 100
#define  F_MAX 10000

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

    size_t nz;
    size_t nu;
    size_t tz;
    size_t tu;
    size_t f ;

    // process arguments
    if (!parse_num(argv[1], 1, SIZE_MAX, &nz))
        return eprintf("Invalid value for argument NZ");
    if (!parse_num(argv[2], 1, SIZE_MAX, &nu))
        return eprintf("Invalid value for argument NU");
    if (!parse_num(argv[3], 0, TZ_MAX, &tz))
        return eprintf("Invalid value for argument TZ");
    if (!parse_num(argv[4], 0, TU_MAX, &tu))
        return eprintf("Invalid value for argument TU");
    if (!parse_num(argv[5], 0, F_MAX, &f))
        return eprintf("Invalid value for argument F");
}

static _Bool parse_num(const char *str, size_t min, size_t max, size_t *out) {
    // check for empty string or negative value
    if (!*str || *str == '-')
        return 0;

    char *end;
    *out = strtoul(str, &end, 10);

    // check if the whole string was consumed and the value is in bounds
    return *end == 0 && *out >= min && *out <= max;
}

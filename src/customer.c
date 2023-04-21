#include "customer.h" // size_t

#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // usleep

#include "logger.h" // flog

int customer_main(size_t id) {
    usleep(id * 80);
    flog("Customer id: %zu", id);
    return EXIT_SUCCESS;
}

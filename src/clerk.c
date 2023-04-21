#include "clerk.h"  // size_t

#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // usleep

#include "logger.h" // flog

int clerk_main(size_t id) {
    usleep(id * 50);
    flog("Clerk id: %zu", id);
    return EXIT_SUCCESS;
}

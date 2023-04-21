#include "customer.h" // size_t

#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // usleep
#include <stdlib.h> // srand, rand
#include <time.h>   // time

#include "logger.h"  // flog
#include "mem_mgr.h" // mmgr_g_stats

int customer_main(size_t id) {
    // make unique seed
    srand(time(NULL) * id + 17);

    usleep(rand() % 100000);
    flog("Customer id: %zu/%zu", id, mmgr_g_stats()->nz);
    return EXIT_SUCCESS;
}

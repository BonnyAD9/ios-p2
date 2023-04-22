#include "clerk.h"  // size_t

#include <stdlib.h> // EXIT_SUCCESS
#include <unistd.h> // usleep
#include <stdlib.h> // srand, rand
#include <time.h>   // time
#include <signal.h> // kill, SIGUSR1

#include "logger.h"  // flog
#include "mem_mgr.h" // mmgr_g_stats, Q_COUNT, pid_t, mmgr_queue, mmgr_g_queue,
                     // mmq_pop, mmgr_g_closed

int clerk_main(const size_t id) {
    // make unique seed
    srand(time(NULL) * id + 11);

    // 1)
    flog("U %zu: started", id);

    for (;;) {
        // 2)
        int x = rand() % Q_COUNT + 1;
        size_t i;
        pid_t customer;
        for (i = 0; i < Q_COUNT; ++i) {
            mmgr_queue q = mmgr_g_queue(x);
            customer = mmq_pop(q);
            if (customer)
                break;
            mmgr_r_queue(x);
            x = (x + 1) % Q_COUNT;
        }

        if (i == Q_COUNT) {
            // 4)
            if (mmgr_g_closed()) {
                // 4.1)
                flog("U %zu: going home", id);

                // 4.2)
                return EXIT_SUCCESS;
            }

            // 3.1)
            flog("U %zu: taking break", id);

            // 3.2)
            usleep(rand() % mmgr_g_stats()->tu);

            // 3.3)
            flog("U %zu: brake finished", id);

            // 3.4)
            continue;
        }

        // 2.1)
        flog("U %zu: serving a service of type %d", id, x);

        // 2)
        kill(customer, SIGUSR1);

        // 2.2)
        usleep(rand() % mmgr_g_stats()->nu);

        // 2.3
        flog("U %zu: service finished", id);
    }
}

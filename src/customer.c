#include "customer.h" // size_t, NULL

#include <stdlib.h>  // EXIT_SUCCESS
#include <unistd.h>  // usleep, getpid
#include <stdlib.h>  // srand, rand
#include <time.h>    // time
#include <signal.h>  // sigset_t, sigemptyset, sigaddset, SIGUSR1, SIG_BLOCK,
                     // sigwait
#include <pthread.h> // pthread_sigmask

#include "logger.h"  // flog
#include "mem_mgr.h" // mmgr_g_stats, mmgr_g_queue, mmgr_queue, mmq_push,
                     // mmgr_r_queue

int customer_main(const size_t id) {
    // make unique seed
    srand(time(NULL) * id + 17);

    // 1)
    flog("Z %zu: started", id);

    // 2)
    usleep(rand() % mmgr_g_stats()->tz);

    if (mmgr_g_closed()) {
        // 3.1)
        flog("Z %zu: going home", id);

        // 3.2)
        return EXIT_SUCCESS;
    }

    // 4)
    const int x = rand() % 3 + 1;

    // 4.1)
    flog("Z %zu: entering office for a service %d", id, x);

    // 4.2)
    pid_t pid = getpid();

    sigset_t sset;
    sigemptyset(&sset);
    sigaddset(&sset, SIGUSR1);
    pthread_sigmask(SIG_BLOCK, &sset, NULL);

    mmgr_queue q = mmgr_g_queue(x);
    mmq_push(q, pid);
    mmgr_r_queue(x);

    int sig;
    sigwait(&sset, &sig);

    // 4.3)
    flog("Z %zu: called by office worker", id);

    // 4.4)
    int syn = rand() % (10 * 1000);
    usleep(syn);

    // 4.5)
    flog("Z %zu: going home", id);

    // 4.6)
    return EXIT_SUCCESS;
}

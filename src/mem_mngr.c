#include "mem_mgr.h" // mgr_stats, size_t

#include <semaphore.h> // sem_t
#include <sys/mman.h>  // shm_open
#include <fcntl.h>     // O_RDWR, O_CREAT
#include <sys/stat.h>  // S_IRUSR, S_IWUSR

#define SH_MEM_NAME "/ios-p2"

// all the things in the shared memory
typedef struct {
    mmgr_stats stats;
    sem_t log_file_cnt_sem;
    size_t log_file_cnt;
} shared_memory;

static shared_memory *mem = NULL;

_Bool mmgr_init(mmgr_stats *stats) {
    int fd = shm_open(SH_MEM_NAME, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    // TODO (change - stats cannot be NULL)
}

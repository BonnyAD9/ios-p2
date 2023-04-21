#include "mem_mgr.h" // mgr_stats, size_t

#include <semaphore.h> // sem_t, sem_init
#include <sys/mman.h>  // shm_open, PROT_READ, PROT_WRITE, MAP_SHARED,
                       // MAP_FAILED, shm_unlink, mmap
#include <fcntl.h>     // O_RDWR, O_CREAT, O_EXCL
#include <sys/stat.h>  // S_IRUSR, S_IWUSR
#include <unistd.h>    // ftruncate, close

#define SH_MEM_NAME "/ios-p2"

// all the things in the shared memory
typedef struct {
    size_t mem_size;
    mmgr_stats stats;
    sem_t log_file_cnt_sem;
    size_t customer_pid_len;
    size_t clerk_pid_len;
    size_t log_file_cnt;
} mmgr_memory;

static mmgr_memory *mem = NULL;
static int *customer_pids = NULL;
static int *clerk_pids = NULL;

static _Bool _mmgr_init(mmgr_stats *stats, size_t mem_size);

_Bool mmgr_init(mmgr_stats *stats, _Bool init) {
    if (mem)
        return 1;

    // calculate the required space and offsets
    size_t size = sizeof(mmgr_memory);

    size_t customer_pids_offset = size;
    size += stats->nz * sizeof(int);
    size_t clerk_pids_offset = size;
    size += stats->nu * sizeof(int);

    int oflags = O_RDWR;
    if (init)
        oflags |= O_EXCL | O_CREAT;

    // create shared memory
    int fd = shm_open(SH_MEM_NAME, oflags, S_IRUSR | S_IWUSR);
    if (fd == -1)
        return 0;

    // resize the memory if it has not been initialized
    if (init && ftruncate(fd, size) == -1) {
        close(fd);
        shm_unlink(SH_MEM_NAME);
        return 0;
    }

    // link the memory
    mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);
    if (mem == MAP_FAILED) {
        mem = NULL;
        if (init)
            shm_unlink(SH_MEM_NAME);
        return 0;
    }

    // get the memory locations
    customer_pids = (int *)((char *)mem + customer_pids_offset);
    clerk_pids = (int *)((char *)mem + clerk_pids_offset);

    // initialize the memory contents on the firs init
    if (init && !_mmgr_init(stats, size)) {
        munmap(mem, size);
        shm_unlink(SH_MEM_NAME);
        return 0;
    }

    return 1;
}

// initializes the memory contents
static _Bool _mmgr_init(mmgr_stats *stats, size_t mem_size) {
    // init the semaphores
    if (sem_init(&mem->log_file_cnt_sem, 1, 1) == -1)
        return 0;

    // set the default values
    mem->mem_size = mem_size;
    mem->customer_pid_len = 0;
    mem->clerk_pid_len = 0;
    mem->stats = *stats;
    mem->log_file_cnt = 0;

    return 1;
}

void mmgr_close(_Bool clear) {
    if (!clear) {
        if (mem)
            munmap(mem, mem->mem_size);
        return;
    }

    if (mem) {
        sem_close(&mem->log_file_cnt_sem);
        munmap(mem, mem->mem_size);
    }

    shm_unlink(SH_MEM_NAME);
}

const mmgr_stats *mmgr_g_stats(void) {
    return &mem->stats;
}

size_t *mmgr_g_log_file(void) {
    sem_wait(&mem->log_file_cnt_sem);
    return &mem->log_file_cnt;
}

void mmgr_r_log_file(void) {
    sem_post(&mem->log_file_cnt_sem);
}

mmgr_array mmgr_g_customer_pids(void) {
    return (mmgr_array) {
        .data = customer_pids,
        .len = &mem->customer_pid_len
    };
}

mmgr_array mmgr_g_clerk_pids(void) {
    return (mmgr_array) { .data = clerk_pids, .len = &mem->clerk_pid_len };
}

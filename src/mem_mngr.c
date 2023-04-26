#include "mem_mgr.h" // mgr_stats, size_t, pid_t, NULL

#include <semaphore.h> // sem_t, sem_init
#include <sys/mman.h>  // shm_open, PROT_READ, PROT_WRITE, MAP_SHARED,
                       // MAP_FAILED, shm_unlink, mmap
#include <fcntl.h>     // O_RDWR, O_CREAT, O_EXCL
#include <sys/stat.h>  // S_IRUSR, S_IWUSR
#include <unistd.h>    // ftruncate, close
#include <signal.h>    // sig_atomic_t

#define SH_MEM_NAME "/ios-p2-xstigl00"

// all the things in the shared memory
typedef struct {
    // basic data
    size_t mem_size;
    mmgr_stats stats;
    // semaphores
    sem_t log_file_cnt_sem;
    sem_t stderr_print;
    sem_t q_sem[Q_COUNT];
    // data
    size_t log_file_cnt;
    mmq_fields q_fields[Q_COUNT];
    // atomic data
    sig_atomic_t closed;
} mmgr_memory;

static mmgr_memory *mem;
static pid_t *q_data[Q_COUNT];

static _Bool _mmgr_init(mmgr_stats *stats, size_t mem_size);

_Bool mmgr_init(mmgr_stats *stats, _Bool init) {
    if (mem)
        return 1;

    // calculate the required space and offsets
    size_t size = sizeof(mmgr_memory);
    size_t q_size = mmq_predict_data_size(stats->nz);
    size += q_size * Q_COUNT;

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

    // get variable array positions
    char *moff = (char *)mem + sizeof(*mem);
    for (size_t i = 0; i < Q_COUNT; ++i) {
        q_data[i] = (pid_t *)moff;
        moff += q_size;
    }

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
        goto on_file_cnt_fail;
    if (sem_init(&mem->stderr_print, 1, 1) == -1)
        goto on_stderr_print_fail;
    size_t i;
    for (i = 0; i < Q_COUNT; ++i) {
        if (sem_init(&mem->q_sem[i], 1, 1) == -1)
            goto on_q_sem_fail;
    }

    // set the default values
    mem->mem_size = mem_size;
    mem->stats = *stats;

    mem->log_file_cnt = 0;

    for (i = 0; i < Q_COUNT; ++i) {
        mmq_init(&mem->q_fields[i], stats->nz);
    }

    mem->closed = 0;

    return 1;
    // free the allocated resources on failure
on_q_sem_fail:
    for (size_t j = 0; j < i; ++j) {
        sem_destroy(&mem->q_sem[j]);
    }
    sem_destroy(&mem->stderr_print);
on_stderr_print_fail:
    sem_destroy(&mem->log_file_cnt_sem);
on_file_cnt_fail:
    return 0;
}

void mmgr_close(_Bool clear) {
    if (!clear) {
        if (mem)
            munmap(mem, mem->mem_size);
        return;
    }

    if (mem) {
        sem_destroy(&mem->log_file_cnt_sem);
        munmap(mem, mem->mem_size);
    }

    mem = NULL;

    shm_unlink(SH_MEM_NAME);
}

_Bool mmgr_is_init(void) {
    return mem;
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

void mmgr_g_stderr(void) {
    sem_wait(&mem->stderr_print);
}

void mmgr_r_stderr(void) {
    sem_post(&mem->stderr_print);
}

// get value indicating whether the bank is closed
_Bool mmgr_g_closed(void) {
    return mem->closed;
}

// close the bank
void mmgr_s_close(void) {
    mem->closed = 1;
}

// locks queue with the given id, returns invalid queue if the ID is invalid
mmgr_queue mmgr_g_queue(int id) {
    --id;
    if (id < 0 || id >= Q_COUNT)
        return (mmgr_queue) { .data = NULL, .fields = NULL };

    sem_wait(&mem->q_sem[id]);
    return (mmgr_queue) { .data = q_data[id], .fields = &mem->q_fields[id] };
}

// unlocks queue with the given id, returns false if the id is invalid
_Bool mmgr_r_queue(int id) {
    --id;
    if (id < 0 || id >= Q_COUNT)
        return 0;

    sem_post(&mem->q_sem[id]);
    return 1;
}

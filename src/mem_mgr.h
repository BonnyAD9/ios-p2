// manages asybchronous access to shared memory

#ifndef MEM_MGR_INCLUDED
#define MEM_MGR_INCLUDED

#include <stdio.h> // size_t, FILE

// contains the basic information shared between processes
typedef struct {
    size_t nz;
    size_t nu;
    size_t tz;
    size_t tu;
    size_t f ;
} mmgr_stats;

/**
 * @brief initializes shared memory
 *
 * @param stats when NULL link to memory, otherwise initialize new memory based
 *              on the parameters in stats
 */
void mmgr_init(mmgr_stats *stats);

// get the stats (only for reading)
const mmgr_stats *mmgr_g_stats();

// lock the log file and the counter
size_t *mmgr_g_log_file();

// unlock the log file and the counter
void mmgr_r_log_file();

/**
 * @brief closes the shared memory
 *
 * @param clear if this is true, the memory will also be freed
 */
void mmgr_close(_Bool clear);

#endif MEM_MGR_INCLUDED

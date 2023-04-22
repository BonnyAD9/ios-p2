// manages asybchronous access to shared memory

#ifndef MEM_MGR_INCLUDED
#define MEM_MGR_INCLUDED

#include <stdio.h>     // size_t, FILE
#include <sys/types.h> // pid_t

#include "mmgr_queue.h" // mmgr_queue

// number of queues
#define Q_COUNT 3

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
 * @param stats initialize new memory based on the parameters in stats
 * @param init true if this is the first inicialization of the memory
 *             only one process should call this with init set to true
 * @return _Bool true on success, otherwise false
 */
_Bool mmgr_init(mmgr_stats *stats, _Bool init);

/**
 * @brief closes the shared memory
 *
 * @param clear if this is true, the memory will also be freed
 *              only one process should call this with true
 */
void mmgr_close(_Bool clear);

// returns true if the memory is initialized.
_Bool mmgr_is_init(void);

// get the stats (only for reading)
const mmgr_stats *mmgr_g_stats(void);

// lock the log file and the counter
size_t *mmgr_g_log_file(void);

// unlock the log file and the counter
void mmgr_r_log_file(void);

// lock the stderr
void mmgr_g_stderr(void);

// unlock the stderr
void mmgr_r_stderr(void);

// get value indicating whether the bank is closed
_Bool mmgr_g_closed(void);

// close the bank
void mmgr_s_close(void);

// locks queue with the given id, returns invalid queue if the ID is invalid
mmgr_queue mmgr_g_queue(int id);

// unlocks queue with the given id, returns false if the id is invalid
_Bool mmgr_r_queue(int id);

#endif // MEM_MGR_INCLUDED

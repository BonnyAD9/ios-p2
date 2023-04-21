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

typedef struct {
    size_t *len;
    int *data;
} mmgr_array;

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

// get the stats (only for reading)
const mmgr_stats *mmgr_g_stats(void);

// lock the log file and the counter
size_t *mmgr_g_log_file(void);

// unlock the log file and the counter
void mmgr_r_log_file(void);

// lock and get the customer pids should be used only by the main process
mmgr_array mmgr_g_customer_pids(void);

// lock and get the customer pids should be used only by the main process
mmgr_array mmgr_g_clerk_pids(void);

#endif // MEM_MGR_INCLUDED

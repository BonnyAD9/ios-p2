// this file defines the queue
// the queue is just a fixed size circular buffer

#ifndef MMGR_QUEUE_INCLUDED
#define MMGR_QUEUE_INCLUDED

#include <stddef.h>    // size_t
#include <sys/types.h> // pid_t

typedef struct {
    size_t length;
    size_t rd;
    size_t wr;
} mmq_fields;

typedef struct {
    mmq_fields *fields;
    pid_t *data;
} mmgr_queue;

// gets the required data size for the given count
size_t mmq_predict_data_size(size_t count);

// initializes the fields of the queue
void mmq_init(mmq_fields *q_fields, size_t size);

// pushes pid, returns false if the queue is full
_Bool mmq_push(mmgr_queue q, pid_t value);

// pop pid from the queue, 0 if the queue is empty
pid_t mmq_pop(mmgr_queue q);

// check whether the given queue is valid
_Bool mmq_is_valid(mmgr_queue q);

#endif // MMGR_QUEUE_INCLUDED

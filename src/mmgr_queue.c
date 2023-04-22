#include "mmgr_queue.h" // size_t, pid_t

// gets the required data size for the given count
size_t mmq_predict_data_size(size_t count) {
    return sizeof(pid_t) * (count + 1);
}

// initializes the fields of the queue
void mmq_init(mmq_fields *q_fields, size_t size) {
    q_fields->length = size;
    q_fields->rd = 0;
    q_fields->wr = 0;
}

// pushes pid, returns false if the queue is full
_Bool mmq_push(mmgr_queue q, pid_t value) {
    size_t new_wr = (q.fields->wr + 1) % q.fields->length;
    if (new_wr == q.fields->rd) {
        return 0;
    }

    q.data[q.fields->wr] = value;
    q.fields->wr = new_wr;

    return 1;
}

// pop pid from the queue, 0 if the queue is empty
pid_t mmq_pop(mmgr_queue q) {
    if (q.fields->wr == q.fields->rd)
        return 0;

    pid_t value = q.data[q.fields->rd];

    q.fields->rd = (q.fields->rd + 1) % q.fields->length;

    return value;
}

// check whether the given queue is valid
_Bool mmq_is_valid(mmgr_queue q) {
    return q.data && q.fields;
}

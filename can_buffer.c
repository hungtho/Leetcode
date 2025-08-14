#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int id;
    float data;
    int dlc;
} CANMessage;

typedef struct {
    CANMessage *msg;
    int head_index;
    int tail_index;
    int max_size;
    bool is_full;
    int count;
} CircularBuffer;


void print_msg(const char* prefix, CANMessage msg)
{
    printf("%s ID=%d, Data=%.2f, DLC=%d\n",
           prefix, msg.id, msg.data, msg.dlc);
}

CircularBuffer *cb_init(int size)
{
    if (size <= 0) return NULL;
    CircularBuffer *cb = malloc(sizeof(CircularBuffer));
    if (!cb) return NULL;

    cb->msg = malloc(size * sizeof(CANMessage));
    if (!cb->msg) { free(cb); return NULL; }

    cb->head_index = 0;
    cb->tail_index = 0;
    cb->max_size = size;
    cb->is_full = false;
    cb->count = 0;

    return cb;
}

void cb_free(CircularBuffer *cb)
{
    if (cb) {
        free(cb->msg);
        free(cb);
    }
}


bool cb_is_empty(const CircularBuffer *cb)
{
    return cb == NULL || (!cb->is_full && cb->head_index == cb->tail_index);
}

bool cb_is_full(const CircularBuffer *cb)
{
    return cb && cb->is_full;
}
// Cac ham peek va clear
bool cb_peek_front(const CircularBuffer *cb, CANMessage *msg)
{
    if(!cb || !msg || cb_is_empty(cb))
        return false;

    *msg = cb->msg[cb->head_index];

    return true;
}

bool cb_peek_back(const CircularBuffer *cb, CANMessage *msg)
{
    if(!cb || !msg || cb_is_empty(cb))
        return false;

    int back_index = (cb->tail_index - 1 + cb->max_size) % cb->max_size;
    *msg = cb->msg[back_index];

    return true;
}
bool cb_push(CircularBuffer *cb, CANMessage msg)
{
    if (!cb || cb_is_full(cb)) return false;

    cb->msg[cb->tail_index] = msg;
    cb->tail_index = (cb->tail_index + 1) % cb->max_size;
    cb->count++;

    if (cb->head_index == cb->tail_index)
        cb->is_full = true;

    return true;
}

bool cb_pop(CircularBuffer *cb, CANMessage *msg)
{
    if (!cb || !msg || cb_is_empty(cb)) return false;

    *msg = cb->msg[cb->head_index];
    cb->head_index = (cb->head_index + 1) % cb->max_size;
    cb->is_full = false;
    cb->count--;

    return true;
}

bool can_transmit(CANMessage msg)
{
    printf("Transmitting CAN ID: 0x%X, Data: %.2f, DLC: %d\n",
           msg.id, msg.data, msg.dlc);
    return true;
}

// Design a circular deque for CAN messages
// and transmitting messages.
bool cb_pushfront(CircularBuffer *cb, CANMessage msg)
{
    if (!cb || cb_is_full(cb)) return false;   
    cb->head_index = (cb->head_index - 1 + cb->max_size)% cb->max_size;
    cb->msg[cb->head_index] = msg;
    cb->count++;
    if (cb->tail_index == cb->head_index)
        cb->is_full = true;

    return true;
}

bool cb_popback(CircularBuffer *cb, CANMessage *msg)
{
    if (!cb || !msg || cb_is_empty(cb)) return false;   
    cb->tail_index = (cb->tail_index - 1 + cb->max_size) % cb->max_size;
    *msg = cb->msg[cb->tail_index];
    cb->is_full = false;
    cb->count--;

    return true;
}

bool cb_clear( CircularBuffer *cb)
{
    if(!cb) return;

    cb->head_index = 0;
    cb->tail_index = 0;
    cb->count = 0;
    cb->is_full = false;
}

int main()
{
    CircularBuffer *cb = cb_init(5);

    printf("=== Test cb_push (back) ===\n");
    for (int i = 0; i < 3; i++) {
        CANMessage msg = { i, i * 1.5f, 8 };
        cb_push(cb, msg);
        printf("head: %d\n", cb->head_index);
        printf("tail: %d\n", cb->tail_index);
        printf("Push back: ID=%d\n", msg.id);
        
    }

    printf("=== Test cb_pushfront (front) ===\n");
    for (int i = 100; i < 103; i++) {
        CANMessage msg = { i, i * 1.5f, 8 };
        cb_pushfront(cb, msg);
        printf("head: %d\n", cb->head_index);
        printf("tail: %d\n", cb->tail_index);
        printf("Push front: ID=%d\n", msg.id);
    }

    printf("=== Pop all with cb_popback (back) ===\n");
    CANMessage msg;
    while (cb_popback(cb, &msg)) {
        printf("Pop back: ID=%d\n", msg.id);
    }

    printf("=== Test cb_push (back) ===\n");
    for (int i = 0; i < 3; i++) {
        CANMessage msg = { i, i * 1.5f, 8 };
        cb_push(cb, msg);
        printf("Push back: ID=%d\n", cb->head_index);
    }

    // Peek front & back
    CANMessage peek;
    if (cb_peek_front(cb, &peek))
        print_msg("Peek Front", peek);

    if (cb_peek_back(cb, &peek))
        print_msg("Peek Back", peek);

    cb_free(cb);
    return 0;
}
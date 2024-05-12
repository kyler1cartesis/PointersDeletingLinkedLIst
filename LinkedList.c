#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"

void handleMutexError (int mtxOpResult) {
    if (mtxOpResult != thrd_success)
        exit(-4);
}

int traverseAndDelete (DoublyLinkedList* list, MutexWithNodePtr thread_locks[2], enum Direction direction) {
    int threadIndex = direction == Forth ? 1 : 0;
    Node* start = threadIndex ? list->head : list->tail;
    MutexWithNodePtr* thisThreadMutex  = threadIndex ? &thread_locks[0] : &thread_locks[1];
    MutexWithNodePtr* otherThreadMutex = threadIndex ? &thread_locks[1] : &thread_locks[0];
    mtx_lock(&thisThreadMutex->mutex);
    if (start == NULL) {
        perror("Error: Pointer is invalid!");
        return 1;
    }
    Node* current = start;
    thisThreadMutex->position = current;
    mtx_unlock(&thisThreadMutex->mutex);
    size_t bits_count = 0, visited = 0;
    while (current) {
        int to_break = 0;
        handleMutexError(
            mtx_lock(&otherThreadMutex->mutex)
        );
        // if (threadIndex && current == list->tail ||
        //     direction == Back && current == list->head) {
        //     handleMutexError(mtx_unlock(&otherThreadMutex->mutex));
        //     break;
        // }
        if (otherThreadMutex->position) {
            if (otherThreadMutex->position == current) {
                handleMutexError(mtx_unlock(&otherThreadMutex->mutex));
                break;
            }
            else if (current == (threadIndex ? otherThreadMutex->position->prev
                : otherThreadMutex->position->next)) {
                to_break = 1;
            }
        }
        handleMutexError(mtx_unlock(&otherThreadMutex->mutex));
        handleMutexError(mtx_lock(&thisThreadMutex->mutex));
        thisThreadMutex->position = current;
        handleMutexError(mtx_unlock(&thisThreadMutex->mutex));
        uint32_t value = current->data;
        size_t ones_count = 0;
        for (size_t idx = sizeof(value) * 8; idx--; value >>= 1)
            ones_count += value & 1;
        if (threadIndex)
            bits_count += ones_count;
        else
            bits_count += sizeof(value) * 8 - ones_count;
        Node* to_delete = current;
        ++visited;
        current = threadIndex ? current->next : current->prev;
        free(to_delete);
        if (threadIndex)
            list->head = current;
        else
            list->tail = current;
        if (to_break) break;
    }
    if (threadIndex)
        printf("Thread result: %zu zeros, visited %zu nodes\n", bits_count, visited);
    else
        printf("Thread result: %zu ones, visited %zu nodes\n", bits_count, visited);
    return 0;
}

int launchFunction(void* params) {
    const ThreadFunctionParameters* parameters = params;
    return traverseAndDelete(parameters->list, parameters->thread_locks, parameters->direction);
}
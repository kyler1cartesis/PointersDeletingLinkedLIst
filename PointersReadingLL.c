#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "LinkedList.h"

DoublyLinkedList* createList (size_t n) {
    DoublyLinkedList* linkedList = calloc(1, sizeof(struct DoublyLinkedList));

    Node* newNode = malloc(sizeof(struct Node));
    newNode->data = rand();
    newNode->prev = NULL;
    linkedList->head = newNode;

    for (size_t i = 0; i < n - 1; i++) {
        Node* prevNode = newNode;
        newNode = malloc(sizeof(Node));
        newNode->data = rand();
        newNode->prev = prevNode;
        prevNode->next = newNode;
    }
    newNode->next = NULL;
    linkedList->tail = newNode;
    linkedList->size = n;
    return linkedList;
}

char* explainThreadReturnValue (int threadReturnValue) {
    switch (threadReturnValue) {
        case  1: return "Thread has received an invalid pointer!";
        case -4: return "Thread couldn't lock or unlock mutex!";
        default: return NULL;
    }
}

void deleteElementsInTwoThreads (DoublyLinkedList* list) {
    thrd_t threads[2];
    MutexWithNodePtr threadLocks[2];
    ThreadFunctionParameters params[2] = { {.list = list, .direction = Forth, .thread_locks = threadLocks },
                                           {.list = list, .direction = Back,  .thread_locks = threadLocks } };
    for (size_t i = 0; i < 2; ++i) {
        threadLocks[i].position = NULL;
    	int mtxInitResult = mtx_init(&threadLocks[i].mutex, mtx_plain);
        if (mtxInitResult != thrd_success) {
            perror("Error: Failed to initialize mutex!\n");
            exit(-3);
        }
    }
    for (size_t i = 2; i--; ) {
        int createThreadResult = thrd_create(&threads[i], launchFunction, &params[i]);
        if (createThreadResult != thrd_success) {
            perror("Error: Failed to create thread!\n");
            exit(-2);
        }
    }
    for (size_t i = 2; i--; ) {
        int threadReturnValue;
        int threadResult = thrd_join(threads[i], &threadReturnValue);
        if (threadResult != thrd_success)
            printf("Error: Thread %zu has crashed!\n", i);
        char* returnMessage = explainThreadReturnValue(threadReturnValue);
        if (returnMessage)
            printf("Error: [Thread %zu] %s\n", i, returnMessage);
        mtx_destroy(&threadLocks[i].mutex);
    }
}

size_t parseNumberFromCmd(int argc, char** argv) {
    if (argc < 2) {
        puts("Warning: Length has not been provided, defaulting to 101 !\n");
        return 101;
    }
    ptrdiff_t n = strtoll(argv[1], NULL, 10);
    if (n <= 0) {
        perror("Error: Invalid number provided!\n");
        exit(-1);
    }
    return n;
}

int main(int argc, char* argv[]) {
    size_t n = parseNumberFromCmd(argc, argv);
    DoublyLinkedList* list = createList(n);
    deleteElementsInTwoThreads(list);
    return 0;
}

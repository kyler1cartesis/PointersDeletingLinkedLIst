#pragma once
// #include <stdio.h>
#include <stdint.h>
// #include <stdlib.h>
#include <threads.h>

typedef struct DoublyLinkedList {
    struct Node* head, *tail;
    size_t size;
} DoublyLinkedList;

typedef struct Node {
    uint32_t data;
    struct Node* next, *prev;
} Node;

enum Direction { Forth, Back };

typedef struct { mtx_t mutex; Node* position; } MutexWithNodePtr;

typedef struct { DoublyLinkedList* list; MutexWithNodePtr* thread_locks; enum Direction direction; }
ThreadFunctionParameters;

void handleMutexError(int mtxOpResult);
int traverseAndDelete(DoublyLinkedList* list, MutexWithNodePtr thread_locks[2], enum Direction direction);
int launchFunction(void* params);
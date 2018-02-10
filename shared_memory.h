#ifndef _SHARED_MEMORY_H_
#define _SHARED_MEMORY_H_

#include <stdlib.h>
#include <semaphore.h>

typedef int value_t;

typedef struct Shared_Memory {
    size_t size;
    void* all;
    sem_t* odd_semaphore_array;
    sem_t* even_semaphore_array;
    value_t* value_array;
} Shared_Memory_t;

void allocate_shared_memory(Shared_Memory_t* shared_memory, size_t N);
void initialize_shared_memory(Shared_Memory_t* shared_memory, size_t N);
void destroy_shared_memory(Shared_Memory_t* shared_memory, size_t N);

#endif //_SHARED_MEMORY_H_

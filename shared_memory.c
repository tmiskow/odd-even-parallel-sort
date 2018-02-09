#include <sys/mman.h>
#include <semaphore.h>
#include "shared_memory.h"

void allocate_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    shared_memory->size = (2*N - 1) * sizeof(sem_t) + 2*N * sizeof(value_t);
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;

    shared_memory->all = mmap(NULL, shared_memory->size, prot, flags, -1, 0);
    shared_memory->odd_semaphore_array = (sem_t*) (shared_memory->all);
    shared_memory->even_semaphore_array = (sem_t*) (shared_memory->all + (N-1) * sizeof(sem_t));
    shared_memory->value_array = (value_t*) (shared_memory->all + (2*N - 1) * sizeof(sem_t));
}

void initialize_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < N; i++) {
        sem_init(&shared_memory->even_semaphore_array[i], 1, 0);
    }

    for (size_t i = 0; i < N - 1; i++) {
        sem_init(&shared_memory->odd_semaphore_array[i], 1, 0);
    }
}

void swap_shared_memory_values(Shared_Memory_t* shared_memory, size_t index1, size_t index2) {
    value_t temp;
    temp = shared_memory->value_array[index1];
    shared_memory->value_array[index1] = shared_memory->value_array[index2];
    shared_memory->value_array[index2] = temp;
}

void destroy_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < N; i++) {
        sem_destroy(&shared_memory->even_semaphore_array[i]);
    }

    for (size_t i = 0; i < N - 1; i++) {
        sem_destroy(&shared_memory->odd_semaphore_array[i]);
    }

    munmap(shared_memory->all, shared_memory->size);
}
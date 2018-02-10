#include <sys/mman.h>
#include <semaphore.h>
#include "shared_memory.h"
#include "error.h"

void allocate_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    // calculate size of both semaphore and values arrays
    shared_memory->size = (2*N - 1) * sizeof(sem_t) + 2*N * sizeof(value_t);

    // allocate anonymous shared memory
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    shared_memory->all = mmap(NULL, shared_memory->size, prot, flags, -1, 0);
    if (shared_memory->all == MAP_FAILED) {
        error_exit("Error in mmap()");
    }

    // map regions of the shared memory to specific pointers
    shared_memory->odd_semaphore_array = (sem_t*) (shared_memory->all);
    shared_memory->even_semaphore_array = (sem_t*) (shared_memory->all + (N-1) * sizeof(sem_t));
    shared_memory->value_array = (value_t*) (shared_memory->all + (2*N - 1) * sizeof(sem_t));
}

void initialize_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < N; i++) {
        if (sem_init(&shared_memory->even_semaphore_array[i], 1, 0) == -1) {
            error_exit("Error in sem_init()");
        }
    }

    for (size_t i = 0; i < N - 1; i++) {
        if (sem_init(&shared_memory->odd_semaphore_array[i], 1, 0) == -1) {
            error_exit("Error in sem_init()");
        }
    }
}

void destroy_shared_memory(Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < N; i++) {
        if (sem_destroy(&shared_memory->even_semaphore_array[i]) == -1) {
            error_exit("Error in sem_destroy()");
        }
    }

    for (size_t i = 0; i < N - 1; i++) {
        if (sem_destroy(&shared_memory->odd_semaphore_array[i])) {
            error_exit("Error in sem_destroy()");
        }
    }

    if (munmap(shared_memory->all, shared_memory->size) == -1) {
        error_exit("Error in munmap()");
    }
}
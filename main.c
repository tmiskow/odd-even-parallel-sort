#include <stdio.h>
#include <zconf.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <wait.h>

#include "shared_memory.h"

typedef void (*process_t)(size_t, Shared_Memory_t*, size_t);
void spawn_process(process_t process, size_t index, Shared_Memory_t* shared_memory, size_t N) {
    switch (fork()) {
        case -1:
            assert(false);
            exit(-1);

        case 0:
            process(index, shared_memory, N);
            exit(0);

        default:
            break;
    }
}

void even(size_t index, Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < 2*N; i++) {
        // compare and swap
        if (shared_memory->value_array[2*index] > shared_memory->value_array[2*index + 1]) {
            swap_shared_memory_values(shared_memory, 2*index, 2*index + 1);
        }
        // signal odd processes
        if (index > 0) {
            sem_post(&shared_memory->odd_semaphore_array[index - 1]);
        }

        if (index < N - 1) {
            sem_post(&shared_memory->odd_semaphore_array[index]);
        }

        // wait for signal from odd processes
        if (index > 0) {
            sem_wait(&shared_memory->even_semaphore_array[index]);
        }

        if (index < N - 1) {
            sem_wait(&shared_memory->even_semaphore_array[index]);
        }
    }
}

void odd(size_t index, Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < 2*N; i++) {
        // wait for signal from even processes
        for (size_t j = 0; j < 2; j++) {
            sem_wait(&shared_memory->odd_semaphore_array[index]);
        }

        // compare and swap
        if (shared_memory->value_array[2*index + 1] > shared_memory->value_array[2*index + 2]) {
            swap_shared_memory_values(shared_memory, 2*index + 1, 2*index + 2);
        }

        // signal even processes
        sem_post(&shared_memory->even_semaphore_array[index]);
        sem_post(&shared_memory->even_semaphore_array[index + 1]);
    }
}

int main() {
    size_t N;
    scanf("%zu", &N);

    Shared_Memory_t shared_memory;
    allocate_shared_memory(&shared_memory, N);
    initialize_shared_memory(&shared_memory, N);

    for (size_t i = 0; i < 2*N; i++) {
        scanf("%d", &shared_memory.value_array[i]);
    }

    for (size_t i = 0; i < N; i++) {
        spawn_process(even, i, &shared_memory, N);
    }

    for (size_t i = 0; i < N - 1; i++) {
        spawn_process(odd, i, &shared_memory, N);
    }

    // wait for children to die
    wait(NULL);

    // print results
    for (size_t i = 0; i < 2*N; i++) {
        printf("%d\n", shared_memory.value_array[i]);
    }

    destroy_shared_memory(&shared_memory, N);
}
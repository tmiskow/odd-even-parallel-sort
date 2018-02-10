#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <wait.h>

#include "shared_memory.h"
#include "error.h"

typedef void (*process_t)(size_t, Shared_Memory_t*, size_t);
void spawn_process(process_t process, size_t index, Shared_Memory_t* shared_memory, size_t N) {
    switch (fork()) {
        case -1:
            error_exit("Error in fork()");

        case 0:
            process(index, shared_memory, N);
            exit(0);

        default:
            break;
    }
}

void swap(value_t* value1, value_t * value2) {
    value_t temp;
    temp = *value1;
    *value1 = *value2;
    *value2 = temp;
}

void even(size_t index, Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < 2*N; i++) {
        // compare and swap
        if (shared_memory->value_array[2*index] > shared_memory->value_array[2*index + 1]) {
            swap(&shared_memory->value_array[2*index], &shared_memory->value_array[2*index + 1]);
        }
        // signal odd processes
        if (index > 0) {
            if (sem_post(&shared_memory->odd_semaphore_array[index - 1]) == -1) {
                error_exit("Error in sem_post()");
            }
        }

        if (index < N - 1) {
            if (sem_post(&shared_memory->odd_semaphore_array[index]) == -1) {
                error_exit("Error in sem_post()");
            }
        }

        // wait for signal from odd processes
        if (index > 0) {
            if (sem_wait(&shared_memory->even_semaphore_array[index]) == -1) {
                error_exit("Error in sem_wait()");
            }
        }

        if (index < N - 1) {
            if (sem_wait(&shared_memory->even_semaphore_array[index])) {
                error_exit("Error in sem_wait()");
            }
        }
    }
}

void odd(size_t index, Shared_Memory_t* shared_memory, size_t N) {
    for (size_t i = 0; i < 2*N; i++) {
        // wait for signal from even processes
        for (size_t j = 0; j < 2; j++) {
            if (sem_wait(&shared_memory->odd_semaphore_array[index])) {
                error_exit("Error in sem_wait()");
            }
        }

        // compare and swap
        if (shared_memory->value_array[2*index + 1] > shared_memory->value_array[2*index + 2]) {
            swap(&shared_memory->value_array[2*index + 1], &shared_memory->value_array[2*index + 2]);
        }

        // signal even processes
        if (sem_post(&shared_memory->even_semaphore_array[index]) == -1) {
            error_exit("Error in sem_post()");
        }

        if (sem_post(&shared_memory->even_semaphore_array[index + 1]) == -1) {
            error_exit("Error in sem_post()");
        }
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
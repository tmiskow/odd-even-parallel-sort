# odd-even-parallel-sort

Simple parallel sorting algorithm written in C using semaphores and shared memory for Concurrent Programming course at University of Warsaw (2017/2018).

## Description
Process Sort reads `N` integers from `stdin` and allocates `t[0...2N-1]` and O(N) semaphores in shared memory. Then it spawns A(i) and B(i) processes.

#### Process A(i):
* compares and (if necessary) swaps `t[2*i]` and `t[2*i+1]`
* lets B(i) and B(i-1) do their phase
* waits for B(i) and B(i-1) to finish

#### Process B(i):
* waits for A(i) and A(i+1) to finish
* compares and (if necessary) swaps `t[2*i+1]` and `t[2*i+2]`
* lets A(i) and A(i+1) do their phase

In the end process Sort frees shared memory and prints the sorted sequence.

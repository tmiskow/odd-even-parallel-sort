#include <stdio.h>
#include <zconf.h>
#include <errno.h>
#include <stdlib.h>
#include "error.h"

void error_exit(const char* message) {
    fprintf(stderr, "pid: %d\nerror: %s (%d)\n", getpid(), message, errno);
    exit(errno);
}

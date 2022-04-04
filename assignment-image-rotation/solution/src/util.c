#include "../include/rotate.h"
#include "../include/bmp_functions.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


_Noreturn void error_log( const char* log) {
    fprintf(stderr,"%s", log);
    exit(1);
}

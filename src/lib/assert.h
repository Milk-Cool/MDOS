#ifndef ASSERT_H_
#define ASSERT_H_

#include <stdbool.h>
#include <stdlib.h>

void assert(bool expr) {
    if(!expr) abort();
}

#endif
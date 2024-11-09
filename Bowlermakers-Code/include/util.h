#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void nano_wait(unsigned int n);
void convolve(const bool signalArr[], size_t signalLen,
              const int8_t kernelArr[], size_t kernelLen, int8_t result[]);

#endif // UTIL_H
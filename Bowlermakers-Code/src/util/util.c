#include "util/util.h"

void nano_wait(unsigned int n) {
  asm("        mov r0,%0\n"
      "repeat: sub r0,#83\n"
      "        bgt repeat\n"
      :
      : "r"(n)
      : "r0", "cc");
}

// modified version of conv function from
// https://stackoverflow.com/a/8425094/13224686
void convolve(const bool signalArr[], size_t signalLen,
              const int8_t kernelArr[], size_t kernelLen, int8_t result[]) {
  for (size_t n = 0; n < signalLen + kernelLen - 1; n++) {
    result[n] = 0;

    size_t kmin = (n >= kernelLen - 1) ? n - (kernelLen - 1) : 0;
    size_t kmax = (n < signalLen - 1) ? n : signalLen - 1;

    for (size_t k = kmin; k <= kmax; k++) {
      result[n] += signalArr[k] * kernelArr[n - k];
    }
  }
}
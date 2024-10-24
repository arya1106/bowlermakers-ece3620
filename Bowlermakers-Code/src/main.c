// #include <AK9753.h>
// #include <malloc.h>
// #include <stdio.h>
#include <stdbool.h>
#include <stm32f091xc.h>
#include <uart.h>

#include "I2C.h"
#include "clock.h"
#include "uart.h"

void nano_wait(unsigned int n) {
  asm("        mov r0,%0\n"
      "repeat: sub r0,#83\n"
      "        bgt repeat\n"
      :
      : "r"(n)
      : "r0", "cc");
}

int main(void) {
  internal_clock();
  setup_serial();
  enable_ports_I2C();
  init_I2C();

  i2c_waitidle();
  i2c_start(0x56, 1, false);
  i2c_stop();
  nano_wait(1000);

  // setup_I2C();
  // send_byte_I2C(0x64, 0x64);
  // for (int i = 0; i < 10; i++) {
  //   i--;
  // }
  // char* string = malloc(sizeof(char) * 9);
  // snprintf(string, sizeof(char) * 9, "12345678");
  // serial_print(string);
  // free(string);
  return 0;
}
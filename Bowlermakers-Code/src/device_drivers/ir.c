#include <i2c.h>
#include <ir.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util.h>

void ir_read(uint8_t reg_addr, uint8_t data[], uint8_t len) {
  uint8_t bytes[1];
  bytes[0] = reg_addr;
  int res = i2c_senddata(IRSENSOR_ADDR, bytes, 1);
  int resR = i2c_recvdata(IRSENSOR_ADDR, data, len);
  if (res || resR) {
    printf("i2c error\n");
  }
}

void ir_single_write(uint8_t reg_addr, const uint8_t data) {
  uint8_t bytes[2];
  bytes[0] = reg_addr;
  bytes[1] = data;
  int res = i2c_senddata(IRSENSOR_ADDR, bytes, 2);
  if (res) {
    printf("i2c error\n");
  }
}

void ir_write(uint8_t reg_addr, const uint8_t *data, uint8_t len) {
  uint8_t bytes[len + 1];
  bytes[0] = reg_addr;
  memcpy(bytes + 1, data, len);
  int res = i2c_senddata(IRSENSOR_ADDR, bytes, len + 1);
  if (res) {
    printf("i2c error\n");
  }
}

SWIPE_DIRECTION parse_conv_arr(bool *right, bool *left) {
  extern int ir_cooldown_flag;
  int8_t kernel[2] = {1, -1};
  int8_t *leftResult = malloc(sizeof(int16_t) * CONV_WINDOW_SIZE + 1);
  int8_t *rightResult = malloc(sizeof(int16_t) * CONV_WINDOW_SIZE + 1);
  convolve(left, CONV_WINDOW_SIZE, kernel, 2, leftResult);
  convolve(right, CONV_WINDOW_SIZE, kernel, 2, rightResult);

  printf("left CONV>");
  for (int i = 0; i < CONV_WINDOW_SIZE + 1; i++) {
    printf("%d\t", leftResult[i]);
  }
  printf("\n");

  printf("right CONV>");
  for (int i = 0; i < CONV_WINDOW_SIZE + 1; i++) {
    printf("%d\t", rightResult[i]);
  }
  printf("\n");

  for (int i = 0; i < CONV_WINDOW_SIZE + 1; i++) {
    if (leftResult[i] == 1) {
      memset(right, 0, CONV_WINDOW_SIZE);
      memset(left, 0, CONV_WINDOW_SIZE);
      ir_cooldown_flag = IR_COOLDOWN_CYCLES;
      return RIGHT_SWIPE;
    }
    if (rightResult[i] == 1) {
      memset(right, 0, CONV_WINDOW_SIZE);
      memset(left, 0, CONV_WINDOW_SIZE);
      ir_cooldown_flag = IR_COOLDOWN_CYCLES;
      return LEFT_SWIPE;
    }
  }

  return NONE_SWIPE;
}
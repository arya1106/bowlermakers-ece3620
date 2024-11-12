#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device_drivers/ir.h"
#include "peripheral_drivers/i2c.h"
#include "util/util.h"
#include <device_drivers/AK9753.h>

bool IR1_history[CONV_WINDOW_SIZE];
bool IR2_history[CONV_WINDOW_SIZE];
bool IR3_history[CONV_WINDOW_SIZE];
bool IR4_history[CONV_WINDOW_SIZE];
extern SWIPE_DIRECTION current_swipe;
extern uint8_t history_idx;

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
      free(leftResult);
      free(rightResult);
      return RIGHT_SWIPE;
    }
    if (rightResult[i] == 1) {
      memset(right, 0, CONV_WINDOW_SIZE);
      memset(left, 0, CONV_WINDOW_SIZE);
      ir_cooldown_flag = IR_COOLDOWN_CYCLES;
      free(leftResult);
      free(rightResult);
      return LEFT_SWIPE;
    }
  }

  free(leftResult);
  free(rightResult);
  return NONE_SWIPE;
}

void processIRData() {
  uint8_t ready = 0;
  ir_read(AK975X_ST1, &ready, 1);
  if (ready & 0x01) {
    uint8_t IR_data[8];
    ir_read(AK975X_IR1, IR_data, 8); // load dist data from IR1-IR4
    ir_read(AK975X_ST2, NULL, 0); // read dummy reg (required after data recv)

    // flip endianness of all IR measurements and cast to correct type
    int16_t IR1 = (IR_data[0]) + (IR_data[1] << 8);
    int16_t IR2 = (IR_data[2]) + (IR_data[3] << 8);
    int16_t IR3 = (IR_data[4]) + (IR_data[5] << 8);
    int16_t IR4 = (IR_data[6]) + (IR_data[7] << 8);

    printf(">IR1:%d\n", IR1);
    printf(">IR2:%d\n", IR2);

    // fill history array for convolution of measurements
    IR1_history[history_idx] = IR1 > DIST_CUTOFF;
    IR2_history[history_idx] = IR2 > DIST_CUTOFF;
    IR3_history[history_idx] = IR3 > DIST_CUTOFF;
    IR4_history[history_idx] = IR4 > DIST_CUTOFF;

    // loop circular buffer index
    history_idx = (history_idx + 1) % CONV_WINDOW_SIZE;

    // check for swipe when buffer is full with cooldown after swipe
    if (history_idx == CONV_WINDOW_SIZE - 1) {
      if (ir_cooldown_flag) {
        ir_cooldown_flag--;
        current_swipe = NONE_SWIPE;
      } else {
        current_swipe = parse_conv_arr(IR3_history, IR1_history);
      }
    }

  } else {
    // printf("not ready\n");
  }
}
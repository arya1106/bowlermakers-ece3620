#include <stdbool.h>
#include <stdio.h>
#include <stm32f091xc.h>

#include "AK9753.h"
#include "I2C.h"
#include "clock.h"
#include "ir.h"
#include "lcd.h"
#include "uart.h"
#include "util.h"

enum STATE {
  WELCOME_SCREEN,
  HIGHSCORE_DISPLAY,
  GAMEPLAY_SET_POSITION,
  GAMEPLAY_SET_ANGLE,
  GAMEPLAY_BOWL_ANIMATION,
  GAMEPLAY_SHOW_PIN_RESULT,
  GAMEPLAY_SHOW_SCORE_RESULT
};

void welcome_screen_handle_button();
void welcome_screen_check_highlighted();

uint8_t ir_cooldown_flag = 0;
bool IR1_history[CONV_WINDOW_SIZE];
bool IR2_history[CONV_WINDOW_SIZE];
bool IR3_history[CONV_WINDOW_SIZE];
bool IR4_history[CONV_WINDOW_SIZE];
uint8_t history_idx = 0;
SWIPE_DIRECTION current_swipe = NONE_SWIPE;

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

int main(void) {

  // set clock to 48 MHz
  internal_clock();

  // I2C setup
  enable_ports_i2c();
  init_i2c();

  // UART Setup
  init_usart5();
  enable_tty_interrupt();
  setbuf(stdin, 0);
  setbuf(stdout, 0);
  setbuf(stderr, 0);

  // Software reset for sensor
  ir_single_write(AK975X_CNTL2, 0xff);
  // Configure sensor for continous read at 8 hz
  ir_single_write(AK975X_ECNTL1, 0xAC);

  LCD_Setup();
  LCD_Clear(0);
  LCD_DrawFillRectangle(0, 0, 200, 200, WHITE);

  for (;;) {
    processIRData();
    printf(">SWIPE:%d\n", current_swipe);
    switch (current_swipe) {
    case LEFT_SWIPE:
      /* code */
      LCD_Clear(0);
      LCD_DrawFillRectangle(0, 0, 200, 200, RED);
      current_swipe = NONE_SWIPE;
      break;

    case RIGHT_SWIPE:
      /* code */
      LCD_Clear(0);
      LCD_DrawFillRectangle(0, 0, 200, 200, BLUE);
      current_swipe = NONE_SWIPE;
      break;

    case NONE_SWIPE:
      /* code */
      break;

    default:
      break;
    }
  }
}

#include <stdbool.h>
#include <stdio.h>
#include <stm32f091xc.h>

#include "assets/bowling_pmu.h"
// #include "assets/welcome_none_high.h"
// #include "assets/welcome_score_high.h"
// #include "assets/welcome_start_high.h"
#include "device_drivers/AK9753.h"
#include "device_drivers/ir.h"
#include "device_drivers/lcd.h"
#include "graphics.h"
#include "peripheral_drivers/gpio.h"
#include "peripheral_drivers/i2c.h"
#include "peripheral_drivers/uart.h"
#include "util/clock.h"
#include "util/util.h"

typedef enum {
  WELCOME_SCREEN_START_HIGHLIGHTED,
  WELCOME_SCREEN_SCORES_HIGHLIGHTED,
  HIGHSCORE_DISPLAY,
  GAMEPLAY_SET_POSITION,
  GAMEPLAY_SET_ANGLE,
  GAMEPLAY_BOWL_ANIMATION,
  GAMEPLAY_SHOW_PIN_RESULT,
  GAMEPLAY_SHOW_SCORE_RESULT,
  IDLE,
} STATE;

void welcome_screen_handle_button();
void welcome_screen_check_highlighted();

uint8_t ir_cooldown_flag = 0;
uint8_t history_idx = 0;
uint8_t highlight_timer = 0;
SWIPE_DIRECTION current_swipe = NONE_SWIPE;
STATE current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
volatile bool button_pressed = false;

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

  // GPIO
  enable_gpio_ports();
  gpio_interrupt_init();

  // Software reset for sensor
  ir_single_write(AK975X_CNTL2, 0xff);
  // Configure sensor for continous read at 8 hz
  ir_single_write(AK975X_ECNTL1, 0xAC);

  LCD_Setup();
  LCD_Clear(0);
  LCD_DrawPicture(0, 0, &bowling_pmu);

  for (;;) {
    processIRData();
    printf(">SWIPE:%d\n", current_swipe);
    printf(">button:%d\n", button_pressed);

    switch (current_state) {
    case WELCOME_SCREEN_START_HIGHLIGHTED:
      if (current_swipe == RIGHT_SWIPE) {
        current_state = WELCOME_SCREEN_SCORES_HIGHLIGHTED;
        break;
      }
      if (button_pressed) {
        current_state = GAMEPLAY_SET_POSITION;
        break;
      }

      if (highlight_timer == 0) {
        LCD_DrawFillRectangle(28, 180, 100, 185, YELLOW);
      } else if (highlight_timer == FLASH_TIMER / 2) {
        LCD_DrawPicture(0, 0, &bowling_pmu);
      }
      break;

    case WELCOME_SCREEN_SCORES_HIGHLIGHTED:
      if (current_swipe == LEFT_SWIPE) {
        current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
        break;
      }
      if (button_pressed) {
        current_state = HIGHSCORE_DISPLAY;
      }

      if (highlight_timer == 0) {
        LCD_DrawFillRectangle(200, 180, 290, 185, YELLOW);
      } else if (highlight_timer == FLASH_TIMER / 2) {
        LCD_DrawPicture(0, 0, &bowling_pmu);
      }
      break;

    case HIGHSCORE_DISPLAY:
      LCD_Clear(BLACK);
      LCD_DrawString(20, 20, WHITE, BLACK, "high score display", 16, false);
      current_state = IDLE;
      break;

    case GAMEPLAY_SET_POSITION:
      LCD_Clear(BLACK);
      LCD_DrawString(20, 20, WHITE, BLACK, "gameplay set position", 16, false);
      current_state = IDLE;
      break;

    case IDLE:
      break;

    default:
      break;
    }

    highlight_timer++;
    if (highlight_timer == FLASH_TIMER) {
      highlight_timer = 0;
    }
  }
}

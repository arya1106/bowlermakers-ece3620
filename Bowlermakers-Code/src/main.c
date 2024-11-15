#include <stdbool.h>
#include <stdio.h>
#include <stm32f091xc.h>

#include "assets/ball.h"
#include "assets/bowling_pmu.h"
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
uint8_t frame_timer = 0;
SWIPE_DIRECTION current_swipe = NONE_SWIPE;
STATE current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
volatile bool confirm_button_pressed = false;
volatile bool back_button_pressed = false;
bool inital_draw = false;

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
  LCD_DrawPicture(0, 0, &bowling_pmu, false);

  for (;;) {
    processIRData();
    printf(">SWIPE:%d\n", current_swipe);
    printf(">button:%d\n", confirm_button_pressed);

    switch (current_state) {
    case WELCOME_SCREEN_START_HIGHLIGHTED:
      if (current_swipe == RIGHT_SWIPE) {
        current_state = WELCOME_SCREEN_SCORES_HIGHLIGHTED;
        break;
      }
      if (confirm_button_pressed) {
        current_state = GAMEPLAY_SET_POSITION;
        TempPicturePtr(tile, 80, 80);
        for (uint8_t i = 0; i < 5; i++) {
          for (uint8_t j = 0; j < 4; j++) {
            alley(i * 80, j * 60, 80, 80, &tile);
            LCD_DrawPicture(i * 80, j * 60, &tile, false);
          }
        }

        // bool back = false;
        // for (uint8_t i = 0; i < 5; i++) {
        //   for (uint8_t j = 0; j < 4; j++) {
        //     back = !back;
        //     LCD_DrawPicture(i * 80, j * 60, &bowling_lane, false);
        //   }
        // }
        // for (uint8_t i = 0; i < 50; i++) {
        //   back = !back;
        //   LCD_DrawPicture(rand() % 320, rand() % 240, &bowling_lane, back);
        // }
        break;
      }

      if (highlight_timer == 0) {
        LCD_DrawFillRectangle(28, 180, 100, 185, YELLOW);
      } else if (highlight_timer == FLASH_TIMER / 2) {
        LCD_DrawPicture(0, 0, &bowling_pmu, false);
      }
      break;

    case WELCOME_SCREEN_SCORES_HIGHLIGHTED:
      if (current_swipe == LEFT_SWIPE) {
        current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
        break;
      }
      if (confirm_button_pressed) {
        current_state = HIGHSCORE_DISPLAY;
      }

      if (highlight_timer == 0) {
        LCD_DrawFillRectangle(200, 180, 290, 185, YELLOW);
      } else if (highlight_timer == FLASH_TIMER / 2) {
        LCD_DrawPicture(0, 0, &bowling_pmu, false);
      }
      break;

    case HIGHSCORE_DISPLAY:
      if (!inital_draw) {
        LCD_Clear(BLACK);
        // LCD_DrawString(20, 20, WHITE, BLACK, "high score display", 16,
        // false);
        inital_draw = true;
      }
      break;

    case GAMEPLAY_SET_POSITION:
      if (frame_timer == FRAME_TICK) {
        uint16_t x = ((float)history_idx / CONV_WINDOW_SIZE) * 320;
        uint16_t y = ((float)history_idx / CONV_WINDOW_SIZE) * 240;
        TempPicturePtr(tmp, 45, 45);
        if (x + (ball.width) < LCD_H && y + (ball.width) < LCD_W) {
          alley(x - (tmp->width / 2), y - (tmp->height / 2), tmp->width,
                tmp->height, tmp);
          pic_overlay(tmp, (tmp->width / 2) - (ball.width / 2),
                      (tmp->height / 2) - (ball.height / 2), &ball, WHITE);
          LCD_DrawPicture(x - (tmp->width / 2), y - (tmp->height / 2), tmp,
                          false);
        }
        frame_timer = 0;
      } else {
        frame_timer++;
      }
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

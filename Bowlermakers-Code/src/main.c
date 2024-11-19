#include <stdbool.h>
#include <stdio.h>
#include <stm32f091xc.h>

#include "assets/bowling_pmu.h"
#include "device_drivers/AK9753.h"
#include "device_drivers/ir.h"
#include "device_drivers/lcd.h"
#include "graphics.h"
#include "graphics/ball.h"
#include "peripheral_drivers/gpio.h"
#include "peripheral_drivers/i2c.h"
#include "peripheral_drivers/uart.h"
#include "physics.h"
#include "pins.h"
#include "scoring.h"

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
volatile bool vertSwipe = false;
SWIPE_DIRECTION current_swipe = NONE_SWIPE;
STATE current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
volatile bool confirm_button_pressed = false;
volatile bool back_button_pressed = false;
bool ball_reset = false;
pin_position pin_presets[] = PIN_COORDS;
bool pins_hit[10] = {false};

float ballX_f = 0;
float ballY_f = 0;
float angle = 0;
u16 ballX = 20;
u16 ballY = (LCD_H / 2) - 19;
u16 newBallX = 20;
u16 newBallY = (LCD_H / 2) - 19;
int rolls = 0;
int frames = 0;
int score = 0;

void draw_alley() {
  TempPicturePtr(tile, 80, 80);
  for (uint8_t i = 0; i < 4; i++) {
    for (uint8_t j = 0; j < 4; j++) {
      alley(i * 80, j * 60, 80, 80, &tile);
      LCD_DrawPicture(i * 80, j * 60, &tile, false);
    }
  }
  create_pin_rectangles(pins_hit);
  move_ball(0, 0, ballX, ballY);
}

int main(void) {
  ballX_f = ballX;
  ballY_f = ballY;

  // set clock to 48 MHz
  internal_clock();

  // I2C setup
  enable_ports_i2c();
  init_i2c();

  // UART Setup
  //init_usart5();
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

  write_high_score_f(0);

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
        confirm_button_pressed = false;
        current_state = GAMEPLAY_SET_POSITION;
        // vertSwipe = true;
        draw_alley();

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
        confirm_button_pressed = false;
        current_state = HIGHSCORE_DISPLAY;
      }

      if (highlight_timer == 0) {
        LCD_DrawFillRectangle(200, 180, 290, 185, YELLOW);
      } else if (highlight_timer == FLASH_TIMER / 2) {
        LCD_DrawPicture(0, 0, &bowling_pmu, false);
      }
      break;

    case HIGHSCORE_DISPLAY:
      if (!ball_reset) {
        LCD_Clear(BLACK);
        LCD_DrawString(20, 20, WHITE, BLACK, "High Score: ", 16, true);
        char s[4];
        sprintf(s, "%d", read_high_score());
        LCD_DrawString(120, 20, WHITE, BLACK, s, 16, true);
        ball_reset = true;
      }
      if (confirm_button_pressed) {
        confirm_button_pressed = false;
        current_state = WELCOME_SCREEN_START_HIGHLIGHTED;
      }
      break;

    case GAMEPLAY_SET_POSITION:
      if (confirm_button_pressed) {
        confirm_button_pressed = false;
        ballX_f = ballX;
        ballY_f = ballY;
        current_state = GAMEPLAY_SET_ANGLE;
      }
      if (current_swipe != NONE_SWIPE) {
        if (current_swipe == LEFT_SWIPE) {
          newBallY += 10;
          current_swipe = NONE_SWIPE;
        } else if (current_swipe == RIGHT_SWIPE) {
          newBallY -= 10;
          current_swipe = NONE_SWIPE;
        }
        move_ball(ballX, ballY, newBallX, newBallY);
        ballX = newBallX;
        ballY = newBallY;
      }
      break;

    case GAMEPLAY_SET_ANGLE:
      if (ball_reset) {
        move_ball(ballX, ballY, ballX, ballY);
        ball_reset = false;
      }

      if (current_swipe == LEFT_SWIPE) {
        angle += 6;
        current_swipe = NONE_SWIPE;
        ball_reset = true;
      }
      if (current_swipe == RIGHT_SWIPE) {
        angle -= 6;
        current_swipe = NONE_SWIPE;
        ball_reset = true;
      }

      float potential_x_f = ballX_f;
      float potential_y_f = ballY_f;
      calculate_ball_position(&potential_x_f, &potential_y_f, angle, 20);
      LCD_DrawLine(ballX, ballY, potential_x_f, potential_y_f, RED);

      if (confirm_button_pressed) {
        confirm_button_pressed = false;
        ballX_f = potential_x_f;
        ballY_f = potential_y_f;
        current_state = GAMEPLAY_BOWL_ANIMATION;
      }
      break;

    case GAMEPLAY_BOWL_ANIMATION:
      calculate_ball_position(&ballX_f, &ballY_f, angle, 4);
      move_ball(ballX, ballY, (u16)ballX_f, (u16)ballY_f);
      ballX = ballX_f;
      ballY = ballY_f;
      scan_pins(ballX, ballY);
      clear_hit_pins();
      if (ballX + 19 > LCD_H || ballY + 19 > LCD_W || ballX - 19 < 0 ||
          ballY - 19 < 0) {
        current_state = GAMEPLAY_SHOW_PIN_RESULT;
      }
      break;

    case GAMEPLAY_SHOW_PIN_RESULT:
      LCD_Clear(BLACK);
      LCD_DrawString(20, 20, WHITE, BLACK, "You knocked down pins: ", 16, true);
      printf("Pins hit: ");
      for (int i = 0; i < 10; i++) {
        printf("%d ", pins_hit[i]);
      }
      printf("\n");
      for (int i = 0; i < 10; i++) {
        char str[3];
        snprintf(str, 3, "%d", i + 1);
        if (pins_hit[i]) {
          LCD_DrawString(20 + i * 30, 40, YELLOW, BLACK, str, 16, true);
        } else {
          LCD_DrawString(20 + i * 30, 40, GRAY, BLACK, str, 16, true);
        }
      }
      if (confirm_button_pressed) {
        confirm_button_pressed = false;
        rolls++;

        ballX = 20;
        ballY = (LCD_H / 2) - 19;
        newBallX = 20;
        newBallY = (LCD_H / 2) - 19;
        angle = 0;
        ballX_f = 0;

        int kd = 0;
        for (int i = 0; i < 10; i++) {
          kd += pins_hit[i];
        }
        if (kd == 10 || rolls == 2) {
          if (rolls == 1) {
            score += 5;
          }
          score += kd;
          for (int i = 0; i < 10; i++) {
            rolls = 0;
            pins_hit[i] = false;
          }
          frames++;
        }
        draw_alley();
        if (frames == 10) {
          write_high_score(score);
          score = 0;
          frames = 0;
          current_state = WELCOME_SCREEN_SCORES_HIGHLIGHTED;
        } else {
          current_state = GAMEPLAY_SET_POSITION;
        }

      }
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

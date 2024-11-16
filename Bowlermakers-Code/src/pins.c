#include "pins.h"
#include "graphics.h"

void create_pin_rectangles(void) {
  extern pin_position pin_presets[];
  for (int i = 0; i < NUM_PINS; i++) {
    LCD_DrawFillRectangle(pin_presets[i].top_left_x, pin_presets[i].top_left_y,
                          pin_presets[i].top_left_x + PIN_DIM,
                          pin_presets[i].top_left_y + PIN_DIM, WHITE);
  }
}

void clear_hit_pins(void) {
  extern pin_position pin_presets[];
  extern bool pins_hit[];
  for (int i = 0; i < NUM_PINS; i++) {
    if (pins_hit[i]) {
      TempPicturePtr(background, PIN_DIM + 2, PIN_DIM + 2);
      alley(pin_presets[i].top_left_x, pin_presets[i].top_left_y, PIN_DIM + 2,
            PIN_DIM + 2, background);
      LCD_DrawPicture(pin_presets[i].top_left_x, pin_presets[i].top_left_y,
                      background, false);
    }
  }
}

static void update_pin_array(u16 x, u16 y) {
  extern pin_position pin_presets[];
  extern bool pins_hit[];
  for (int i = 0; i < NUM_PINS; i++) {
    if (!pins_hit[i]) {
      bool xInPin = (pin_presets[i].top_left_x <= x) &&
                    (x <= pin_presets[i].top_left_x + PIN_DIM);
      bool yInPin = (pin_presets[i].top_left_y <= y) &&
                    (y <= pin_presets[i].top_left_y + PIN_DIM);
      if (xInPin && yInPin) {
        pins_hit[i] = true;
      }
    }
  }
}

void scan_pins(u16 ballX, u16 ballY) {
  for (u16 i = ballX - 30 / 2; i <= ballX + 30 / 2; i++) {
    for (u16 j = ballY - 30 / 2; j <= ballY + 30 / 2; j++) {
      update_pin_array(i, j);
    }
  }
}
#include "include/pins.h"

void create_pin_rectangles(void) {
    for (int i = 0; i < NUM_PINS; i++) {
        LCD_DrawFillRectangle(pin_presets[i].top_left_x, pin_presets[i].top_left_y, pin_presets[i].top_left_x + PIN_DIM, pin_presets[i].top_left_y + PIN_DIM, WHITE);
    }
}


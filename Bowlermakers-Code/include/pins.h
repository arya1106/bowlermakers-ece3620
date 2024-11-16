#ifndef PINS_H
#define PINS_H

#include "device_drivers/lcd.h"
#include <stdint.h>

typedef struct {
  u16 top_left_x;
  u16 top_left_y;
} pin_position;

#define NUM_PINS 10
#define PIN_DIM 10
#define PIN_COORDS                                                             \
  {{.top_left_x = 142, .top_left_y = 115},                                     \
   {.top_left_x = 158, .top_left_y = 107},                                     \
   {.top_left_x = 158, .top_left_y = 123},                                     \
   {.top_left_x = 174, .top_left_y = 99},                                      \
   {.top_left_x = 174, .top_left_y = 115},                                     \
   {.top_left_x = 174, .top_left_y = 131},                                     \
   {.top_left_x = 190, .top_left_y = 91},                                      \
   {.top_left_x = 190, .top_left_y = 107},                                     \
   {.top_left_x = 190, .top_left_y = 123},                                     \
   {.top_left_x = 190, .top_left_y = 139}}

void create_pin_rectangles(void);
void scan_pins(u16 ballX, u16 ballY);
void clear_hit_pins(void);
#endif /* PINS_H */
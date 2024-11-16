#include "device_drivers/lcd.h"
#include <stdint.h>

typedef struct {
  u16 top_left_x;
  u16 top_left_y;
} pin_position;

pin_position pin_presets[] = {
    {142, 115}, // pin1
    {158, 107}, // pin2
    {158, 123}, // pin3
    {174, 99},  // pin4
    {174, 115}, // pin5
    {174, 131}, // pin6
    {190, 91},  // pin7
    {190, 107}, // pin8
    {190, 123}, // pin9
    {190, 139}  // pin10
};

#define NUM_PINS 10
#define PIN_DIM 10

void create_pin_rectangles(void);
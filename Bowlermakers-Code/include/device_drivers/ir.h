#ifndef IR_H
#define IR_H

#include <stdbool.h>
#include <stdint.h>

#define IRSENSOR_ADDR 0x64
#define CONV_WINDOW_SIZE 35
#define DIST_CUTOFF 10000
#define IR_COOLDOWN_CYCLES 1

typedef enum { NONE_SWIPE, LEFT_SWIPE, RIGHT_SWIPE } SWIPE_DIRECTION;

void ir_read(uint8_t reg_addr, uint8_t data[], uint8_t len);
void ir_single_write(uint8_t reg_addr, const uint8_t data);
void ir_write(uint8_t reg_addr, const uint8_t *data, uint8_t len);
void process_ir_data();
SWIPE_DIRECTION parse_conv_arr(bool *right, bool *left);

#endif // IR_H
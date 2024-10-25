#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>  // rand()

#define MAX_FRAMES 10
#define MAX_PINS 10
#define MAX_HIGH_SCORES 5
#define EEPROM_HIGH_SCORE_ADDR 0x0000

typedef struct {
    int roll1;
    int roll2;
    int roll3;
    int score;
} Frame;

typedef struct {
    Frame frames[MAX_FRAMES];
    int pins[MAX_PINS];
    int current_frame;
    int current_roll;
    int total_score;
} Game;

extern Game current_game;

void init_game(void);
void reset_pins(void);
int read_gesture_control(void);
void roll_ball(void);
int calculate_pins_hit(int starting_position);
void update_pins(int pins_hit);
void update_score(int pins_hit);
void calculate_frame_score(void);
void advance_game(void);
void end_game(void);

// Aryaaman write for I2C setup 
extern int get_gesture_value(void);

#endif 
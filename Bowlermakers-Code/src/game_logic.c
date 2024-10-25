#include "game_logic.h"
#include <stdlib.h>
#include <string.h>

Game current_game;

void init_game(void) {
    current_game.current_frame = 0;
    current_game.current_roll = 0;
    current_game.total_score = 0;
    for (int i = 0; i < MAX_FRAMES; i++) {
        current_game.frames[i].roll1 = 0;
        current_game.frames[i].roll2 = 0;
        current_game.frames[i].roll3 = 0;  // For 10th frame
        current_game.frames[i].score = 0;
    }
    reset_pins();
}

void reset_pins(void) {
    for (int i = 0; i < MAX_PINS; i++) {
        current_game.pins[i] = 1; // 1 means standing 0 means knocked down
    }
}

int read_gesture_control(void) {
    int gesture_value = get_gesture_value();  // Aryamaan writes function get gesture value from i2c sensor
    return (gesture_value * 100) / 65535;  // Map to 0-100 range
}

void roll_ball(void) {
    int starting_position = read_gesture_control();
    int pins_hit = calculate_pins_hit(starting_position);
    update_pins(pins_hit);
    update_score(pins_hit);
    advance_game();
}

int calculate_pins_hit(int starting_position) {
    int center_deviation = abs(50 - starting_position);
    if (center_deviation <= 5) {
        return 10;  // Strike
    } else if (center_deviation <= 10) {
        return 7 + (rand() % 3);  // 7-9 pins
    } else if (center_deviation <= 20) {
        return 4 + (rand() % 3);  // 4-6 pins
    } else if (center_deviation <= 30) {
        return 1 + (rand() % 3);  // 1-3 pins
    } else {
        return 0;  // Miss
    }
}

void update_pins(int pins_hit) {
    int standing_pins = 0;
    for (int i = 0; i < MAX_PINS; i++) {
        if (current_game.pins[i] == 1) {
            standing_pins++;
        }
    }
    
    int pins_to_knock;
    if (pins_hit > standing_pins) {
        pins_to_knock = standing_pins;
    } else {
        pins_to_knock = pins_hit;
    }
    
    for (int i = 0; i < MAX_PINS && pins_to_knock > 0; i++) {
        if (current_game.pins[i] == 1) {
            current_game.pins[i] = 0;
            pins_to_knock--;
        }
    }
}

void update_score(int pins_hit) {
    Frame *frame = &current_game.frames[current_game.current_frame];
    if (current_game.current_roll == 0) {
        frame->roll1 = pins_hit;
    } else if (current_game.current_roll == 1) {
        frame->roll2 = pins_hit;
    } else {
        frame->roll3 = pins_hit;  // Only for 10th frame
    }
    
    calculate_frame_score();
}

void calculate_frame_score(void) {
    for (int i = 0; i <= current_game.current_frame; i++) {
        Frame *frame = &current_game.frames[i];
        Frame *next_frame;
        if (i < MAX_FRAMES - 1) {
            next_frame = &current_game.frames[i + 1];
        } else {
            next_frame = NULL;
        }

        Frame *after_next_frame;
        if (i < MAX_FRAMES - 2) {
            after_next_frame = &current_game.frames[i + 2];
        } else {
            after_next_frame = NULL;
        }

        if (i < current_game.current_frame || (i == current_game.current_frame && current_game.current_roll > 0)) {
            if (frame->roll1 == 10) {  // Strike
                frame->score = 10;
                if (next_frame) {
                    frame->score += next_frame->roll1;
                    if (next_frame->roll1 == 10 && after_next_frame) {
                        frame->score += after_next_frame->roll1;
                    } else {
                        frame->score += next_frame->roll2;
                    }
                }
            } else if (frame->roll1 + frame->roll2 == 10) {  // Spare
                frame->score = 10;
                if (next_frame) {
                    frame->score += next_frame->roll1;
                }
            } else {
                frame->score = frame->roll1 + frame->roll2;
            }
        }
    }

    // Special case for 10th frame
    if (current_game.current_frame == MAX_FRAMES - 1) {
        Frame *last_frame = &current_game.frames[MAX_FRAMES - 1];
        // Should check for strike/spare conditions
        if (last_frame->roll1 == 10 || last_frame->roll1 + last_frame->roll2 == 10) {
            last_frame->score = last_frame->roll1 + last_frame->roll2 + last_frame->roll3;
        } else {
            last_frame->score = last_frame->roll1 + last_frame->roll2;
        }
    }  

    // Calculate total score
    current_game.total_score = 0;
    for (int i = 0; i <= current_game.current_frame; i++) {
        current_game.total_score += current_game.frames[i].score;
    }
}

void advance_game(void) {
    Frame *frame = &current_game.frames[current_game.current_frame];
    
    if (current_game.current_frame < MAX_FRAMES - 1) {
        // Frames 1-9
        if (frame->roll1 == 10 || current_game.current_roll == 1) {
            current_game.current_roll = 0;
            current_game.current_frame++;
            reset_pins();
        } else {
            current_game.current_roll++;
        }
    } else {
        // 10th frame
        if (current_game.current_roll == 0) {
            current_game.current_roll++;
            if (frame->roll1 < 10) {
                reset_pins();
            }
        } else if (current_game.current_roll == 1) {
            current_game.current_roll++;
            if (frame->roll1 == 10 || frame->roll1 + frame->roll2 == 10) {
                reset_pins();
            } else {
                end_game();
            }
        } else if (current_game.current_roll == 2) {
            end_game();
        }
    }
}

void end_game(void) {
    current_game.current_frame++;  // Ensure the game loop terminates
}

//chris will write read_eeprom in eeprom.c (wrote basic for simulation)
// for load_high_scores function

//chris will write write_eeprom in eeprom.c (wrote basic for simulation)
// for save_high_scores and check_high_score function
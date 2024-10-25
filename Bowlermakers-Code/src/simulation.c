#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"
#include "eeprom.h"

// simulate gesture control
int get_gesture_value() {
    return rand() % 65536;  // random value between 0 and 65535
}

// simulate EEPROM operations
void eeprom_i2c_write(uint16_t address, const uint8_t *data, uint16_t length) {
    // don't need to actually write to EEPROM
    printf("Writing %d bytes to EEPROM address 0x%04X\n", length, address);
}

void eeprom_i2c_read(uint16_t address, uint8_t *data, uint16_t length) {
    // can initialize with some default values
    for (int i = 0; i < length; i++) {
        data[i] = 0;
    }
    printf("Reading %d bytes from EEPROM address 0x%04X\n", length, address);
}

// print the current game state
void print_game_state() {
    printf("\nCurrent Frame: %d, Current Roll: %d\n", current_game.current_frame + 1, current_game.current_roll + 1);
    printf("Total Score: %d\n", current_game.total_score);
    printf("Pins: ");
    for (int i = 0; i < MAX_PINS; i++) {
        printf("%d ", current_game.pins[i]);
    }
    printf("\n");
}

int main() {
    srand(time(NULL));  // init random number generator

    init_game();
    load_high_scores();

    printf("Welcome to the Bowling Game Simulation!\n");

    while (current_game.current_frame < MAX_FRAMES) {
        print_game_state();
        printf("Press Enter to roll the ball...");
        getchar();  // wait for user input

        roll_ball();

        // check if game ended
        if (current_game.current_frame == MAX_FRAMES && current_game.current_roll == 0) {
            break;
        }
    }

    printf("\nGame Over!\n");
    printf("Final Score: %d\n", current_game.total_score);

    // print frame scores
    for (int i = 0; i < MAX_FRAMES; i++) {
        printf("Frame %d: %d\n", i + 1, current_game.frames[i].score);
    }

    // print high scores
    printf("\nHigh Scores:\n");
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        printf("%d. %s: %d\n", i + 1, high_scores[i].name, high_scores[i].score);
    }

    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game_logic.h"

// simulate gesture control
int get_gesture_value() {
    return rand() % 65536;  // random value between 0 and 65535
}

// simulate EEPROM operations
// eeprom_i2c_write
// eeprom_i2c_read

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

    return 0;
}
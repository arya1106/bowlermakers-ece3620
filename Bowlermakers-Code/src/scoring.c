#include "scoring.h"

int read_high_score(void) {
    int s;
    eeprom_read(SCORE_LOC, &s, 4);
    return s;
}

void write_high_score(int s) {
    int p = read_high_score();
    if (s > p)
    {
        eeprom_write(SCORE_LOC, &s, 4);
    }
}
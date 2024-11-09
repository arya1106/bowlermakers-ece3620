#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

#define EEPROM_ADDR 0x51

void eeprom_write(uint16_t loc, const char *data, uint8_t len);
void eeprom_read(uint16_t loc, char data[], uint8_t len);

#endif // EEPROM_H
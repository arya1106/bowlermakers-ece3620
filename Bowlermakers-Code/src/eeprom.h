#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

// read data from EEPROM
void read_eeprom(uint16_t address, uint8_t *data, uint16_t length);

// write data to EEPROM
void write_eeprom(uint16_t address, const uint8_t *data, uint16_t length);

// assuming existing I2C setup
extern void eeprom_i2c_write(uint16_t address, const uint8_t *data, uint16_t length);
extern void eeprom_i2c_read(uint16_t address, uint8_t *data, uint16_t length);

#endif 
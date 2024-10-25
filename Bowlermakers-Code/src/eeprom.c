#include "eeprom.h"

#define EEPROM_PAGE_SIZE 64  // typical page size for EEPROM (can adjust)

void read_eeprom(uint16_t address, uint8_t *data, uint16_t length) {
    eeprom_i2c_read(address, data, length);
}

void write_eeprom(uint16_t address, const uint8_t *data, uint16_t length) {
    uint16_t i;
    uint16_t page_start, page_end, page_remain;

    for (i = 0; i < length; i += page_remain) {
        page_start = (address + i) & ~(EEPROM_PAGE_SIZE - 1);
        page_end = page_start + EEPROM_PAGE_SIZE;
        page_remain = page_end - (address + i);

        if (page_remain > length - i)
            page_remain = length - i;

        eeprom_i2c_write(address + i, &data[i], page_remain);

        // wait for write to complete (5ms write cycle time?)
        // will need to write to use a timer or check the EEPROM status
        for (volatile int j = 0; j < 1000; j++) {}
    }
}
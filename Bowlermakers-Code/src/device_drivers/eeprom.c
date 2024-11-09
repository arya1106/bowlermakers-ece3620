#include <eeprom.h>
#include <i2c.h>
#include <stdint.h>

void eeprom_write(uint16_t loc, const char *data, uint8_t len) {
  uint8_t bytes[34];
  bytes[0] = loc >> 8;
  bytes[1] = loc & 0xFF;
  for (int i = 0; i < len; i++) {
    bytes[i + 2] = data[i];
  }
  i2c_senddata(EEPROM_ADDR, bytes, len + 2);
}

void eeprom_read(uint16_t loc, char data[], uint8_t len) {
  // ... your code here
  uint8_t bytes[2];
  bytes[0] = loc >> 8;
  bytes[1] = loc & 0xFF;
  i2c_senddata(EEPROM_ADDR, bytes, 2);
  i2c_recvdata(EEPROM_ADDR, data, len);
}
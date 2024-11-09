#include <stm32f091xc.h>

void enable_ports_i2c(void);

void init_i2c(void);

void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir);

void i2c_stop(void);

void i2c_waitidle(void);

int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size);

int i2c_recvdata(uint8_t targadr, void *data, uint8_t size);

void i2c_clearnack(void);

int i2c_checknack(void);
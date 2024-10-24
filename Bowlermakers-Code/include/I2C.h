#include <stm32f091xc.h>

void enable_ports_I2C();
void init_I2C();
void i2c_start(uint8_t target_addr, uint8_t size, uint8_t read);
void i2c_waitidle();
void i2c_stop();
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size);
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size);
void i2c_clearnack(void);
int i2c_checknack(void);
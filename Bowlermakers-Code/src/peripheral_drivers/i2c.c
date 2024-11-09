#include <I2C.h>
#include <stm32f091xc.h>

//===========================================================================
// Configure SDA and SCL.
//===========================================================================
void enable_ports_i2c(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
  GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
  GPIOA->AFR[1] &= ~(GPIO_AFRH_AFSEL9 | GPIO_AFRH_AFSEL10);
  GPIOA->AFR[1] |= 0x4 << GPIO_AFRH_AFSEL9_Pos;
  GPIOA->AFR[1] |= 0x4 << GPIO_AFRH_AFSEL10_Pos;
  GPIOA->OTYPER |= GPIO_OTYPER_OT_9;
  GPIOA->OTYPER |= GPIO_OTYPER_OT_10;
}

//===========================================================================
// Configure I2C1.
//===========================================================================
void init_i2c(void) {
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  I2C1->CR1 &= ~I2C_CR1_PE;
  I2C1->CR1 |= I2C_CR1_ANFOFF | I2C_CR1_ERRIE | I2C_CR1_NOSTRETCH;
  I2C1->TIMINGR &= ~(I2C_TIMINGR_PRESC);
  I2C1->TIMINGR |= (5 << I2C_TIMINGR_PRESC_Pos);
  I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLL);
  I2C1->TIMINGR |= 0x9 << I2C_TIMINGR_SCLL_Pos;
  I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLH);
  I2C1->TIMINGR |= 0x3 << I2C_TIMINGR_SCLH_Pos;
  I2C1->TIMINGR &= ~(I2C_TIMINGR_SDADEL);
  I2C1->TIMINGR |= 0x3 << I2C_TIMINGR_SDADEL_Pos;
  I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLDEL);
  I2C1->TIMINGR |= 0x3 << I2C_TIMINGR_SCLDEL_Pos;

  // I2C1 -> TIMINGR = (uint32_t) 0x50330309;
  I2C1->CR2 &= ~I2C_CR2_ADD10;
  I2C1->CR2 |= I2C_CR2_AUTOEND;
  I2C1->CR1 |= I2C_CR1_PE;
}

//===========================================================================
// Send a START bit.
//===========================================================================
void i2c_start(uint32_t targadr, uint8_t size, uint8_t dir) {
  uint32_t tempReg = I2C1->CR2;
  tempReg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START |
               I2C_CR2_STOP);
  if (dir) {
    tempReg |= (I2C_CR2_RD_WRN);
  } else {
    tempReg &= ~(I2C_CR2_RD_WRN);
  }
  tempReg |= ((targadr << 1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
  tempReg |= I2C_CR2_START;
  I2C1->CR2 = tempReg;
}

//===========================================================================
// Send a STOP bit.
//===========================================================================
void i2c_stop(void) {
  if (I2C1->ISR & I2C_ISR_STOPF) {
    return;
  }
  I2C1->CR2 |= I2C_CR2_STOP;

  while ((I2C1->ISR & I2C_ISR_STOPF)) {
    // asm("nop");
  }
  I2C1->ICR |= I2C_ICR_STOPCF;
}

//===========================================================================
// Wait until the I2C bus is not busy. (One-liner!)
//===========================================================================
void i2c_waitidle(void) {
  while (I2C1->ISR & I2C_ISR_BUSY) {
    asm("nop");
  }
}

//===========================================================================
// Send each char in data[size] to the I2C bus at targadr.
//===========================================================================
int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size) {
  i2c_waitidle();
  i2c_start(targadr, size, 0);
  for (int i = 0; i < size; i++) {
    int count = 0;
    while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
      count += 1;
      if (count > 1000000)
        return -1;
      if (i2c_checknack()) {
        i2c_clearnack();
        i2c_stop();
        return -1;
      }
    }
    data[i] &= I2C_TXDR_TXDATA;
    I2C1->TXDR = data[i];
  }
  while ((I2C1->ISR & I2C_ISR_TC) && (I2C1->ISR & I2C_ISR_NACKF)) {
    asm("nop");
  }
  if (I2C1->ISR & I2C_ISR_NACKF) {
    return -1;
  }
  return 0;
}

//===========================================================================
// Receive size chars from the I2C bus at targadr and store in data[size].
//===========================================================================
int i2c_recvdata(uint8_t targadr, void *data, uint8_t size) {
  i2c_waitidle();
  i2c_start(targadr, size, 1);
  for (int i = 0; i < size; i++) {
    int count = 0;
    while ((I2C1->ISR & I2C_ISR_RXNE) == 0) {
      count += 1;
      if (count > 1000000)
        return -1;
      if (i2c_checknack()) {
        i2c_clearnack();
        i2c_stop();
        return -1;
      }
    }
    ((uint8_t *)data)[i] = (I2C1->RXDR & I2C_RXDR_RXDATA);
  }
  return 0;
}

//===========================================================================
// Clear the NACK bit. (One-liner!)
//===========================================================================
void i2c_clearnack(void) { I2C1->ICR |= I2C_ICR_NACKCF; }

//===========================================================================
// Check the NACK bit. (One-liner!)
//===========================================================================
int i2c_checknack(void) { return (I2C1->ISR & I2C_ISR_NACKF); }
#include <I2C.h>
#include <stm32f091xc.h>

void enable_ports_I2C() {
  RCC->AHBENR |= RCC_AHBENR_GPIOFEN;
  GPIOF->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
  GPIOF->MODER |= GPIO_MODER_MODER0_1 | GPIO_MODER_MODER1_1;
  GPIOF->AFR[0] &= ~(GPIO_AFRL_AFSEL0 | GPIO_AFRL_AFSEL1);
  GPIOF->AFR[0] |= 0x1 << GPIO_AFRL_AFSEL0_Pos;
  GPIOF->AFR[0] |= 0x1 << GPIO_AFRL_AFSEL1_Pos;
}

void init_I2C() {
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
  I2C1->TIMINGR |= 0x1 << I2C_TIMINGR_SDADEL_Pos;
  I2C1->TIMINGR &= ~(I2C_TIMINGR_SCLDEL);
  I2C1->TIMINGR |= 0x3 << I2C_TIMINGR_SCLDEL_Pos;
  I2C1->CR2 |= I2C_CR2_ADD10;
  I2C1->CR2 |= I2C_CR2_AUTOEND;
  I2C1->CR1 |= I2C_CR1_PE;
}

// void I2C1_IRQHandler() {
//   // Verify interrupt status.
//   if ((I2C1->ISR & I2C_ISR_RXNE) == I2C_ISR_RXNE) {
//     // Read byte (which clears RXNE flag).
//     uint8_t i2c_rxb = I2C1->RXDR;
//     char* output_str = malloc(10);
//     snprintf(output_str, 10, "0x%x\n", i2c_rxb);
//     serial_print(output_str);
//     free(output_str);
//   }
// }

// void send_byte_I2C(uint32_t slave_addr, uint8_t data) {
//   while (!((I2C1->ISR & I2C_ISR_TXE) == I2C_ISR_TXE)) {
//     asm("nop");
//   }

//   I2C1->TXDR = data;
//   I2C1->CR2 |= I2C_CR2_START;
// }

void i2c_start(uint8_t target_addr, uint8_t size, uint8_t read) {
  uint32_t tempReg = I2C1->CR2;
  tempReg &= ~(I2C_CR2_SADD | I2C_CR2_NBYTES | I2C_CR2_RD_WRN | I2C_CR2_START |
               I2C_CR2_STOP);
  if (read) {
    tempReg |= (I2C_CR2_RD_WRN);
  }
  tempReg |=
      ((target_addr << 1) & I2C_CR2_SADD) | ((size << 16) & I2C_CR2_NBYTES);
  tempReg |= I2C_CR2_START;
  I2C1->CR2 = tempReg;
}

void i2c_stop() {
  if (I2C1->ISR & I2C_ISR_STOPF) {
    return;
  }
  I2C1->CR2 |= I2C_CR2_STOP;

  while ((I2C1->ISR & I2C_ISR_STOPF)) {
    asm("nop");
  }
  I2C1->ICR |= I2C_ICR_STOPCF;
}

void i2c_waitidle() {
  while (I2C1->ISR & I2C_ISR_BUSY) {
    asm("nop");
  }
}

int8_t i2c_senddata(uint8_t targadr, uint8_t data[], uint8_t size) {
  i2c_waitidle();
  i2c_start(targadr, size, 0);
  for (int i = 0; i < size - 1; i++) {
    int count = 0;
    while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
      count += 1;
      if (count > 1000000) return -1;
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
}

int i2c_recvdata(uint8_t targadr, void *data, uint8_t size) {}

void i2c_clearnack(void) { I2C1->ICR |= I2C_ICR_NACKCF; }

int i2c_checknack(void) { return (I2C1->ISR & I2C_ISR_NACKF); }
#include <stm32f091xc.h>

void setup_serial(void) {
  RCC->AHBENR |= 0x00180000;
  GPIOC->MODER |= 0x02000000;
  GPIOC->AFR[1] |= 0x00020000;
  GPIOD->MODER |= 0x00000020;
  GPIOD->AFR[0] |= 0x00000200;
  RCC->APB1ENR |= 0x00100000;
  USART5->CR1 &= ~0x00000001;
  USART5->CR1 |= 0x00008000;
  USART5->BRR = 0x340;
  USART5->CR1 |= 0x0000000c;
  USART5->CR1 |= 0x00000001;
}

void serial_print(char* str) {
  int i = 0;
  char curr = str[i];
  while (curr != '\0') {
    if ((USART5->ISR & USART_ISR_TC) == USART_ISR_TC) {
      USART5->TDR = str[i];
      i++;
      curr = str[i];
    }
  }
  USART5->ICR |= USART_ICR_TCCF;
}
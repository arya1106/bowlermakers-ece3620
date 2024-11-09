#include "fifo.h"
#include "tty.h"
#include <stdio.h>
#include <stm32f091xc.h>

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;

//===========================================================================
// init_usart5
//===========================================================================
void init_usart5() {
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  RCC->AHBENR |= RCC_AHBENR_GPIODEN;
  GPIOC->MODER |= GPIO_MODER_MODER12_1;
  GPIOD->MODER |= GPIO_MODER_MODER2_1;
  GPIOC->AFR[1] |= (0x2 << GPIO_AFRH_AFSEL12_Pos);
  GPIOD->AFR[0] |= (0x2 << GPIO_AFRL_AFSEL2_Pos);

  RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
  USART5->CR1 &= ~(USART_CR1_UE);
  USART5->CR1 &= ~(USART_CR1_M1);
  USART5->CR1 &= ~(USART_CR1_M0);
  USART5->CR2 &= ~(USART_CR2_STOP);
  USART5->CR1 &= ~(USART_CR1_PCE);
  USART5->CR1 &= ~(USART_CR1_OVER8);
  USART5->BRR = 0x1A1;
  USART5->CR1 |= USART_CR1_TE;
  USART5->CR1 |= USART_CR1_RE;
  USART5->CR1 |= USART_CR1_UE;
  while (
      !((USART5->ISR & USART_ISR_REACK) && (USART5->ISR & USART_ISR_TEACK))) {
  }
}

//===========================================================================
// enable_tty_interrupt
//===========================================================================
void enable_tty_interrupt(void) {
  NVIC->ISER[0] |= (1 << 29);
  USART5->CR1 |= USART_CR1_RXNEIE;
  USART5->CR3 |= USART_CR3_DMAR;
  RCC->AHBENR |= RCC_AHBENR_DMA2EN;
  DMA2->CSELR |= DMA2_CSELR_CH2_USART5_RX;
  DMA2_Channel2->CCR &= ~DMA_CCR_EN;
  DMA2_Channel2->CMAR = (uint32_t)serfifo;
  DMA2_Channel2->CPAR = (uint32_t)&(USART5->RDR);
  DMA2_Channel2->CNDTR = FIFOSIZE;
  DMA2_Channel2->CCR &= ~(DMA_CCR_DIR | DMA_CCR_TCIE | DMA_CCR_HTIE |
                          DMA_CCR_MSIZE | DMA_CCR_PSIZE);
  DMA2_Channel2->CCR |= (DMA_CCR_CIRC | DMA_CCR_PL | DMA_CCR_MINC);
  DMA2_Channel2->CCR &= ~(DMA_CCR_MEM2MEM);
  DMA2_Channel2->CCR |= DMA_CCR_EN;
}

//===========================================================================
// interrupt_getchar
//===========================================================================
char interrupt_getchar() {
  while (fifo_newline(&input_fifo) == 0) {
    asm volatile("wfi");
  }
  // Return a character from the line buffer.
  char ch = fifo_remove(&input_fifo);
  return ch;
}

//===========================================================================
// __io_putchar
//===========================================================================
int __io_putchar(int c) {
  while (!(USART5->ISR & USART_ISR_TXE))
    ;
  if (c == '\n') {
    USART5->TDR = '\r';
    while (!(USART5->ISR & USART_ISR_TXE))
      ;
  }
  USART5->TDR = c;
  return c;
}

//===========================================================================
// __io_getchar
//===========================================================================
int __io_getchar(void) { return interrupt_getchar(); }

//===========================================================================
// IRQHandler for USART5
//===========================================================================
void USART3_8_IRQHandler(void) {
  while (DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
    if (!fifo_full(&input_fifo))
      insert_echo_char(serfifo[seroffset]);
    seroffset = (seroffset + 1) % sizeof serfifo;
  }
}
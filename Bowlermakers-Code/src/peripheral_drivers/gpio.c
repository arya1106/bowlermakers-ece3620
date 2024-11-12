#include <stdbool.h>
#include <stm32f091xc.h>

#include "peripheral_drivers/gpio.h"

void enable_gpio_ports() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
  RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

  GPIOC->MODER |= GPIO_MODER_MODER6_0;
  GPIOC->BSRR |= GPIO_BSRR_BS_6;
  GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_0;
}

void gpio_interrupt_init() {
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
  EXTI->FTSR |= EXTI_FTSR_FT0;
  EXTI->IMR |= EXTI_IMR_IM0;
  NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void EXTI0_1_IRQHandler() {
  EXTI->PR = EXTI_PR_PR0;
  extern bool button_pressed;
  button_pressed = true;
}

#include <stdbool.h>
#include <stm32f091xc.h>

#include "peripheral_drivers/gpio.h"

void enable_gpio_ports() {
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_0;
  GPIOB->PUPDR |= GPIO_PUPDR_PUPDR2_1;
}

void gpio_interrupt_init() {
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PB;
  EXTI->FTSR |= EXTI_FTSR_FT0;
  EXTI->RTSR |= EXTI_RTSR_RT2;
  EXTI->IMR |= EXTI_IMR_IM0;
  EXTI->IMR |= EXTI_IMR_IM2;
  NVIC_EnableIRQ(EXTI0_1_IRQn);
  NVIC_EnableIRQ(EXTI2_3_IRQn);
}

void EXTI0_1_IRQHandler() {
  EXTI->PR = EXTI_PR_PR0;
  extern bool confirm_button_pressed;
  confirm_button_pressed = true;
}

void EXTI2_3_IRQHandler() {
  EXTI->PR = EXTI_PR_PR2;
  extern bool back_button_pressed;
  back_button_pressed = true;
}

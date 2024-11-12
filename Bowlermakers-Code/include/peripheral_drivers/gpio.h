#ifndef GPIO_H
#define GPIO_H

#include <stm32f091xc.h>

void enable_gpio_ports();
void gpio_interrupt_init();
void EXTI0_1_IRQHandler();
void togglexn(GPIO_TypeDef *port, int n);

#endif // GPIO_H
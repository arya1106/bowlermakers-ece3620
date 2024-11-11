#ifndef UART_H
#define UART_H
void init_usart5(void);
void enable_tty_interrupt(void);
char interrupt_getchar(void);
int __io_putchar(int c);
int __io_getchar(void);
void USART3_8_IRQHandler(void);

#endif /* UART_H */
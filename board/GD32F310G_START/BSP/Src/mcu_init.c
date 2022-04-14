#include "mcu_init.h"

#define USART_CONSOLE (USART0)

int fputc(int ch, FILE *f)
{
  while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
  if (ch == '\n') {
    /* Auto complete return char */
    usart_data_transmit(USART_CONSOLE, '\r');
  }
  usart_data_transmit(USART_CONSOLE, ch);
  return ch;
}

int _write(int fd, char *ptr, int len)
{
    int i = 0;
    for (; i < len; i++)
    {
        while(RESET == usart_flag_get(USART_CONSOLE, USART_FLAG_TBE));
        usart_data_transmit(USART_CONSOLE, *(ptr + i));
    }
    return len;
}

int fgetc(FILE *f)
{
    uint8_t ch = 0;
    ch = (uint8_t)usart_data_receive(USART_CONSOLE);
    return ch;
}

void board_init(void)
{
    board_gpio_init();
    board_usart_init();
}

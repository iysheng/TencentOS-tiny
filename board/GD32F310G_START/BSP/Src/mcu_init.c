#include "mcu_init.h"

#define USART_CONSOLE (USART0)

int fputc(int ch, FILE *f)
{
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
  __attribute__((unused)) char *str = "TencentOS tiny";

  /* TODO system clock config */
  SystemClock_Config();
  
  /* TODO print welcome message */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

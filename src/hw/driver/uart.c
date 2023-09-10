#include <string.h>

#include "usart.h"

#include "uart.h"

#define UART_TIMEOUT 1000

bool uart_init(void) {
  return true;
}

void uart_send_string(uint8_t *buff) {
  if (buff == NULL) {
    return;
  }

  HAL_UART_Transmit(&huart2, buff, strlen(buff) * sizeof(uint8_t), UART_TIMEOUT);
}
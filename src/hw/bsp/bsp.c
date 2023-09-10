#include "bsp.h"

#include "dma.h"
#include "gpio.h"
#include "i2c.h"
#include "usart.h"

extern void SystemClock_Config(void);

bool bsp_init(void) {
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();

  MX_I2C1_Init();
  MX_USART2_UART_Init();

  return true;
}
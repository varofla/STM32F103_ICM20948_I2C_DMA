#include "stm32f1xx_hal.h"

#include "icm20948.h"

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (icm20948_isr_gpio(GPIO_Pin)) {
    return;
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_Pin);
  }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  if (icm20948_isr_i2c_read_fin(hi2c)) {
    return;
  }
}

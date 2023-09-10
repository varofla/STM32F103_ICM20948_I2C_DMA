#ifndef SRC_HW_DRIVER_ICM20948_H_
#define SRC_HW_DRIVER_ICM20948_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
  double x;
  double y;
  double z;
} axis_data_t;

typedef struct {
  axis_data_t acc;
  axis_data_t gyr;
  axis_data_t mag;
} icm20948_data_t;

bool icm20948_init(void);

icm20948_data_t *icm20948_get_data(icm20948_data_t *buff);

bool icm20948_isr_gpio(uint16_t pin);
bool icm20948_isr_i2c_read_fin(void *hi2c);

#endif /* SRC_HW_DRIVER_ICM20948_H_ */
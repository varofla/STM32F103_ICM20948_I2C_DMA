#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "app.h"

#include "icm20948.h"
#include "uart.h"

bool app_init(void) {
  return true;
}

void app_main(void) {
  icm20948_data_t imu_data;
  uint8_t send_buff[200] = {0};

  for (;;) {
    if (icm20948_get_data(&imu_data) == NULL) {
      continue;
    }

    sprintf(send_buff, "#IIU|%.4f|%.4f|%.4f|%.4f|%.4f|%.4f|%.2f|%.2f|%.2f\n",
            imu_data.acc.x, imu_data.acc.y, imu_data.acc.z,
            imu_data.gyr.x, imu_data.gyr.y, imu_data.gyr.z,
            imu_data.mag.x, imu_data.mag.y, imu_data.mag.z);

    uart_send_string(send_buff);
  }
}
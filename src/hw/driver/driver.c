#include "driver.h"

#include "icm20948.h"
#include "uart.h"

bool driver_init(void) {
  int rtn = true;

  rtn &= uart_init();
  rtn &= icm20948_init();

  return rtn;
}
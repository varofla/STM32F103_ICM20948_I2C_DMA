#include "hw.h"

#include "bsp.h"
#include "driver.h"

bool hw_init(void) {
  bool rtn = true;

  rtn &= bsp_init();
  rtn &= driver_init();

  return rtn;
}
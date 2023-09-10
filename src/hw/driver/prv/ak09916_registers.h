/*
  code from 20948_ArduinoLibrary
  https://github.com/sparkfun/SparkFun_ICM-20948_ArduinoLibrary/blob/main/src/util/AK09916_REGISTERS.h
*/

#ifndef SRC_HW_DRIVER_AK09916_REGISTERS_H_
#define SRC_HW_DRIVER_AK09916_REGISTERS_H_

#include <stdint.h>

typedef enum {
  AK09916_REG_WIA1 = 0x00,
  AK09916_REG_WIA2,
  AK09916_REG_RSV1,
  AK09916_REG_RSV2, // Reserved register. We start reading here when using the DMP. Secret sauce...
  // discontinuity - containing another nine reserved registers? Secret sauce...
  AK09916_REG_ST1 = 0x10,
  AK09916_REG_HXL,
  AK09916_REG_HXH,
  AK09916_REG_HYL,
  AK09916_REG_HYH,
  AK09916_REG_HZL,
  AK09916_REG_HZH,
  // discontinuity
  AK09916_REG_ST2 = 0x18,
  // discontinuity
  AK09916_REG_CNTL2 = 0x31,
  AK09916_REG_CNTL3,
} AK09916_Reg_Addr_e;

#endif // SRC_HW_DRIVER_AK09916_REGISTERS_H_
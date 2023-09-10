#include "i2c.h"

#include "icm20948.h"

#include "prv/ak09916_registers.h"
#include "prv/icm20948_registers.h"

#define IMU_ADDRESS 0xD0 // AD0 = 0
// #define IMU_ADDRESS 0xD2 // AD0 = 1
#define MAG_ADDRESS 0x0C
#define I2C_TIMEOUT 3000

static uint8_t s_dma_buff[20];
static bool s_is_data_ready = false;
static bool s_is_imu_init = false;

static bool s_change_bank(uint8_t bank);

static bool s_read_byte(uint16_t address, uint8_t *data_p) {
  return HAL_I2C_Mem_Read(&hi2c1, IMU_ADDRESS, address, 1, data_p, 1, I2C_TIMEOUT) == HAL_OK;
}

static bool s_write_byte(uint16_t address, uint8_t data) {
  return HAL_I2C_Mem_Write(&hi2c1, IMU_ADDRESS, address, 1, &data, 1, I2C_TIMEOUT) == HAL_OK;
}

static bool s_read_bytes_dma(uint16_t address, uint8_t len, uint8_t *buff) {
  return HAL_I2C_Mem_Read_DMA(&hi2c1, IMU_ADDRESS, address, 1, buff, len) == HAL_OK;
}

static bool s_mag_write_byte(uint16_t address, uint8_t data) {
  bool ret = true;
  ret &= s_change_bank(3);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_ADDR, MAG_ADDRESS);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_REG, address);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_DO, data);
  HAL_Delay(50);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_CTRL, 0x80 | 0x01);
  HAL_Delay(50);

  return ret;
}

static bool s_mag_set_read_bytes(uint16_t start_address, uint8_t len) {
  bool ret = true;
  ret &= s_change_bank(3);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_ADDR, 0x80 | MAG_ADDRESS);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_REG, start_address);
  HAL_Delay(50);
  ret &= s_write_byte(AGB3_REG_I2C_PERIPH0_CTRL, 0x80 | len);
  HAL_Delay(50);

  return ret;
}

static bool s_change_bank(uint8_t bank) {
  bank = (bank << 4) & 0x30;
  return s_write_byte(REG_BANK_SEL, bank);
}

bool icm20948_init(void) {
  uint8_t buff = 0;
  bool ret = true;

  ret &= s_change_bank(0);
  ret &= s_read_byte(AGB0_REG_WHO_AM_I, &buff);

  if (ret != true || buff != 0xEA) { // 올바르게 인식되지 않음.
    return false;
  }

  ret &= s_write_byte(AGB0_REG_PWR_MGMT_1, 0x81); // reset
  HAL_Delay(100);

  ret &= s_write_byte(AGB0_REG_PWR_MGMT_1, 0x09); // power off temperature

  ret &= s_change_bank(2);
  ret &= s_write_byte(AGB2_REG_ODR_ALIGN_EN, 0x01); // ODR alignment enable

  ret &= s_write_byte(AGB2_REG_GYRO_SMPLRT_DIV, 0x0A);                          // 1.1 kHz/(1+10) = 100 Hz
  ret &= s_write_byte(AGB2_REG_GYRO_CONFIG_1, (2 << 3) | (00 << 1) | (1 << 0)); // GYRO_DLPFCFG 2, GYRO_FS_SEL 0, GYRO_FCHOICE 1

  ret &= s_write_byte(AGB2_REG_ACCEL_SMPLRT_DIV_1, 0x00);
  ret &= s_write_byte(AGB2_REG_ACCEL_SMPLRT_DIV_2, 0x0A);                      // 1.1 kHz/(1+10) = 100 Hz
  ret &= s_write_byte(AGB2_REG_ACCEL_CONFIG, (2 << 3) | (00 << 1) | (1 << 0)); // ACCEL_DLPFCFG 2, ACCEL_FS_SEL 0, ACCEL_FCHOICE 1

  ret &= s_change_bank(0);
  ret &= s_write_byte(AGB0_REG_USER_CTRL, 0x02); // reset i2c master mode
  HAL_Delay(100);
  ret &= s_write_byte(AGB0_REG_USER_CTRL, 0x20); // enable i2c master mode

  ret &= s_change_bank(3);
  ret &= s_write_byte(AGB3_REG_I2C_MST_CTRL, 0x07 | (1 << 4)); // I2C master clock 400 khz
  ret &= s_change_bank(0);
  ret &= s_write_byte(AGB0_REG_LP_CONFIG, 0x40); // I2C_MST_CYCLE enable
  ret &= s_change_bank(3);
  ret &= s_write_byte(AGB3_REG_I2C_MST_ODR_CONFIG, 0x03); // read freq as about 136

  // init mag
  ret &= s_mag_write_byte(AK09916_REG_CNTL3, 0x01); // reset
  HAL_Delay(100);
  ret &= s_mag_write_byte(AK09916_REG_CNTL2, 0x08); // Continuous measurement mode 4 (100 Hz)
  ret &= s_mag_set_read_bytes(AK09916_REG_HXL, 8);  // 데이터 시작점부터 ST1 레지스터까지 읽기 시작함.

  // interrupt set
  ret &= s_change_bank(0);
  ret &= s_write_byte(AGB0_REG_INT_PIN_CONFIG, (1 << 4));
  ret &= s_write_byte(AGB0_REG_INT_ENABLE_1, (1 << 0));

  if (ret != true) {
    return false;
  }

  s_is_imu_init = true;
  return true;
}

icm20948_data_t *icm20948_get_data(icm20948_data_t *buff) {
  if (buff == NULL || s_is_imu_init == false || s_is_data_ready == false) {
    return NULL;
  }

  buff->acc.x = (int16_t)(s_dma_buff[0] << 8 | s_dma_buff[1]) / 16384.0;
  buff->acc.y = (int16_t)(s_dma_buff[2] << 8 | s_dma_buff[3]) / 16384.0;
  buff->acc.z = (int16_t)(s_dma_buff[4] << 8 | s_dma_buff[5]) / 16384.0;

  buff->gyr.x = (int16_t)(s_dma_buff[6] << 8 | s_dma_buff[7]) / 131.0;
  buff->gyr.y = (int16_t)(s_dma_buff[8] << 8 | s_dma_buff[9]) / 131.0;
  buff->gyr.z = (int16_t)(s_dma_buff[10] << 8 | s_dma_buff[11]) / 131.0;

  buff->mag.x = (int16_t)(s_dma_buff[15] << 8 | s_dma_buff[14]) * 0.15;
  buff->mag.y = (int16_t)(s_dma_buff[17] << 8 | s_dma_buff[16]) * 0.15;
  buff->mag.z = (int16_t)(s_dma_buff[19] << 8 | s_dma_buff[18]) * 0.15;

  s_is_data_ready = false;

  return buff;
}

bool icm20948_isr_gpio(uint16_t pin) {
  if (pin != IMU_INT_Pin) {
    return false;
  }

  HAL_StatusTypeDef ret = s_read_bytes_dma(AGB0_REG_ACCEL_XOUT_H, 20, s_dma_buff);
  if (ret != HAL_OK) {
    volatile int a = 0;
  }
  return true;
}

bool icm20948_isr_i2c_read_fin(void *hi2c) {
  if ((I2C_HandleTypeDef *)hi2c != &hi2c1) {
    return false;
  }

  s_is_data_ready = true;
  return true;
}

#include "i2chelper.h"

#define MPU6050_ADDR 0x68
#define MPU6050_SLEEP 0b01000000
#define MPU6050_NOSLEEP 0x00
#define MPU6050_CLK_SRC_INTERNAL 0x00
#define MPU6050_CLK_SRC_REG 0x6B // Three LSB
#define MPU6050_ACCEL_CONFIG 0x1C
#define MPU6050_GYRO_CONFIG 0x1B
#define MPU6050_ACCEL_REG_BEGIN 0x3B
#define MPU6050_ACCEL_REG_END 0x40

#define MPU6050_ACCEL_RANGE_2G 0b00000000
#define MPU6050_ACCEL_RANGE_4G 0b00001000
#define MPU6050_ACCEL_RANGE_8G 0b00010000
#define MPU6050_ACCEL_RANGE_16G 0b00011000

#define MPU6050_GYRO_RANGE_250 0b00000000
#define MPU6050_GYRO_RANGE_500 0b00001000
#define MPU6050_GYRO_RANGE_1000 0b00010000
#define MPU6050_GYRO_RANGE_2000 0b00011000

void mpu6050_init(i2c_device *dev);
void mpu6050_set_clock_source(i2c_device *dev, uint8_t clk);
void mpu6050_set_sleep(i2c_device *dev, uint8_t slp);
void mpu6050_set_accel_range(i2c_device *dev, uint8_t range);
void mpu6050_set_gyro_range(i2c_device *dev, uint8_t range);

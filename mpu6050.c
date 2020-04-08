#include "mpu6050.h"

void mpu6050_init(i2c_device *dev)
{
	mpu6050_set_clock_source(dev, MPU6050_CLK_SRC_INTERNAL);
	mpu6050_set_accel_range(dev, MPU6050_ACCEL_RANGE_2G);
	mpu6050_set_gyro_range(dev, MPU6050_GYRO_RANGE_500);
	mpu6050_set_sleep(dev, MPU6050_NOSLEEP);

}
void mpu6050_set_clock_source(i2c_device *dev, uint8_t clk)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_CLK_SRC_REG, clk,
				0b00000111);
}
void mpu6050_set_accel_range(i2c_device *dev, uint8_t range)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_ACCEL_CONFIG, range,
				0b00011000);
}
void mpu6050_set_gyro_range(i2c_device *dev, uint8_t range)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_GYRO_CONFIG, range,
				0b00011000);
}
void mpu6050_set_sleep(i2c_device *dev, uint8_t slp)
{
	if (slp != MPU6050_SLEEP) {
		slp = MPU6050_NOSLEEP;
	}

	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_CLK_SRC_REG, slp,
				0b01000000);
}

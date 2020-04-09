#include "mpu6050.h"

void init_mpu6050(i2c_device *dev)
{
	whoami_mpu6050(dev);
	set_accel_range_mpu6050(dev, MPU6050_ACCEL_RANGE_2G);
	set_gyro_range_mpu6050(dev, MPU6050_GYRO_RANGE_500);
	set_sleep_mpu6050(dev, MPU6050_NOSLEEP);

}
void set_clock_source_mpu6050(i2c_device *dev, uint8_t clk)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_CLK_SRC_REG, clk,
				0b00000111);
}
void set_accel_range_mpu6050(i2c_device *dev, uint8_t range)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_ACCEL_CONFIG, range,
				0b00011000);
}
void set_gyro_range_mpu6050(i2c_device *dev, uint8_t range)
{
	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_GYRO_CONFIG, range,
				0b00011000);
}
void set_sleep_mpu6050(i2c_device *dev, uint8_t slp)
{
	if (slp != MPU6050_SLEEP) {
		slp = MPU6050_NOSLEEP;
	}

	write_partial_register8(dev->i2c, dev->i2c_addr, MPU6050_CLK_SRC_REG, slp,
				0b01000000);
}
uint8_t whoami_mpu6050(i2c_device *dev){
	uint8_t ret;
	read_register8(dev->i2c, dev->i2c_addr, MPU6050_WHO_AM_I, &ret);
	return ret;
}

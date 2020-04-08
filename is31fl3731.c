#include "support.h"
#include "i2chelper.h"
#include "bl.h"
#include "is31fl3731.h"
#include <stdint.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/i2c.h>

/*
 * Inspiried by the Adafruit lib for the IS31FL3731
 * basically a C port
 */

void init_is31fl3731(i2c_device *dev)
{
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG,
			MATRIX_BANK_FUNCTIONREG);
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_REG_SHUTDOWN, 0x00);
	delay(10);
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG,
			MATRIX_BANK_FUNCTIONREG);
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_REG_SHUTDOWN, 0x01);
	delay(10);

	write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG,
			MATRIX_BANK_FUNCTIONREG);
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_REG_CONFIG,
			MATRIX_REG_CONFIG_PICTUREMODE);

	write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG,
			MATRIX_BANK_FUNCTIONREG);
	write_register8(dev->i2c, dev->i2c_addr, MATRIX_REG_PICTURE_FRAME, 0x00);
}

// Clears out all 8 frame registers
void is31fl3731_clear(i2c_device *dev)
{
	for (uint8_t frame = 0; frame < 8; frame++) {
		for (uint8_t i = 0; i <= 0x11; i++) {
			write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG, frame);
			write_register8(dev->i2c, dev->i2c_addr, i, 0x00);
		}
	}
}

void is31fl3731_show(i2c_device *dev, uint8_t *buffer, unsigned int length)
{
	for (uint8_t frame = 0; frame < 8; frame++) {
		for (uint8_t i = 0; i <= 0x11; i++) {
			write_register8(dev->i2c, dev->i2c_addr, MATRIX_CMD_REG, frame);
			write_register8(dev->i2c, dev->i2c_addr, i, buffer[i]);
		}
	}
}

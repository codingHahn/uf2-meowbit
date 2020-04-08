#include <stdint.h>
#include <libopencm3/stm32/i2c.h>

#ifndef I2CHELPER_H
#define I2CHELPER_H
typedef struct {
	unsigned int i2c;
	uint8_t i2c_addr;
} i2c_device;

void write_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg,
		     uint8_t data);
void read_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg,
		    uint8_t *data);
void write_partial_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg,
			     uint8_t data, uint8_t bitmask);
#endif

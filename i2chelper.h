#include <stdint.h>
#include <libopencm3/stm32/i2c.h>

void write_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg, uint8_t data);

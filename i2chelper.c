#include <stdint.h>
#include <libopencm3/stm32/i2c.h>

/*
 * Very lean helper functions that handle reading and writing
 * to 8 bit registers common in i2c devices
 */

void write_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg, uint8_t data) {
  uint8_t buf[2];
  buf[0] = reg;
  buf[1] = data;

  i2c_transfer7(i2c, i2c_addr, buf, 2, NULL, 0);
}

void read_register8(unsigned int i2c, uint8_t i2c_addr, uint8_t reg, uint8_t* data) {
  i2c_transfer7(i2c, i2c_addr, &reg, 1, data, 1);
}

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


void init_is31fl3731(unsigned int i2c, uint8_t i2c_addr) {
  write_register8(i2c, i2c_addr, MATRIX_CMD_REG,  MATRIX_BANK_FUNCTIONREG);
  write_register8(i2c, i2c_addr, MATRIX_REG_SHUTDOWN, 0x00);
  delay(10);
  write_register8(i2c, i2c_addr, MATRIX_CMD_REG, MATRIX_BANK_FUNCTIONREG);
  write_register8(i2c, i2c_addr, MATRIX_REG_SHUTDOWN, 0x01);
  delay(10);

  write_register8(i2c, i2c_addr, MATRIX_CMD_REG, MATRIX_BANK_FUNCTIONREG);
  write_register8(i2c, i2c_addr, MATRIX_REG_CONFIG,
                  MATRIX_REG_CONFIG_PICTUREMODE);

  write_register8(i2c, i2c_addr, MATRIX_CMD_REG, MATRIX_BANK_FUNCTIONREG);
  write_register8(i2c, i2c_addr, MATRIX_REG_PICTURE_FRAME, 0x00);
}

// Clears out all 8 frame registers
void is31fl3731_clear(unsigned int i2c, uint8_t i2c_addr) {
  for (uint8_t frame = 0; frame < 8; frame++) {
    for (uint8_t i = 0; i <= 0x11; i++) {
      write_register8(i2c, i2c_addr, MATRIX_CMD_REG, frame);
      write_register8(i2c, i2c_addr, i, 0x00);
    }
  }
}

/* The matrix is internally divided into two matricies.
 * Those matricies alternate columnswise. To add to this,
 * col 0 - 8 are mirrored vertically and col 9-16 horizontally.
 * This means, that bank 0 starts at column 8.
 *
 * bank 0 => col 8	(8+0)
 * bank 1 => col 9	(8+1)
 * bank 2 => col 7	(8-1)
 * bank 3 => col 10	(8+2)
 * bank 4 => col 6	(8-2)
 *
 * etc.
 */

void adjust_buffer(uint8_t *buffer, unsigned int length) {
  
  uint8_t tmp;
  uint8_t result[length];
  result[0] = buffer[0];

  for (uint8_t i = 1; i < 9; i++) {
    result[2 * i] = buffer[i];
  }

  for (uint8_t i = 9; i < 17; i++) {
    result[((i - 9) * 2) + 1] = buffer[i];
  }

  memcpy(buffer, result, length);
  for (uint8_t i = 0; i < length; i++) {
    if (i % 2 != 0)
      buffer[i] = reverse(buffer[i]) >> 1;
    else {
      if (i < 7) {
        tmp = buffer[i];
        buffer[i] = buffer[16 - i];
        buffer[16 - i] = tmp;
      }
    }
  }
}

void is31fl3731_show(unsigned int i2c, uint8_t i2c_addr, uint8_t *buffer, unsigned int length) {
  uint8_t tmp[length];
  memcpy(tmp, buffer, length);
  //TODO: Move adjust_buffer out of is31fl3731.c
  adjust_buffer(tmp, length);
  for (uint8_t frame = 0; frame < 8; frame++) {
    for (uint8_t i = 0; i <= 0x11; i++) {
      write_register8(i2c, i2c_addr, MATRIX_CMD_REG, frame);
      write_register8(i2c, i2c_addr, i, tmp[i]);
    }
  }
}

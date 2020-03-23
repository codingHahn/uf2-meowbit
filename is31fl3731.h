#include "support.h"
#include "i2chelper.h"
#include "bl.h"
#include <stdint.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/stm32/i2c.h>

#define MATRIX_BANK_FUNCTIONREG 0x0B // page 'nine'
#define MATRIX_REG_SHUTDOWN 0x0A
#define MATRIX_REG_CONFIG 0x00
#define MATRIX_REG_CONFIG_PICTUREMODE 0x00
#define MATRIX_REG_PICTURE_FRAME 0x01
#define MATRIX_CMD_REG 0xFD

void init_is31fl3731(unsigned int i2c, uint8_t i2c_addr);
void is31fl3731_clear(unsigned int i2c, uint8_t i2c_addr); 
void is31fl3731_show(unsigned int i2c, uint8_t i2c_addr, uint8_t *buffer, unsigned int length);
void adjust_buffer(uint8_t *buffer, unsigned int length);

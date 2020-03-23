#include "support.h"
#include "bl.h"
#include "matrix_text.h"
#include "is31fl3731.h"
#include <libopencmsis/core_cm3.h>

void scroll_text(unsigned int i2c, uint8_t i2c_addr, char *text);


#include "bl.h"
#include "is31fl3731.h"
#include "matrix_text.h"
#include "support.h"
#include <libopencmsis/core_cm3.h>

void scroll_text(i2c_device *dev, char *text) {

  uint8_t len = (strlen(text) + 17) * 6;

  // allocating a buffer containing all characters to print in binary
  // representation, plus 17 more columns for a full screen of blank
  // space for more beautiful scrolling

  uint8_t text_buffer[len];
  memset(text_buffer, 0, len);

  uint8_t *offsetptr = text_to_buffer(text, text_buffer);
  uint8_t tmp[17];

  if (strlen(text) > 3) {
    // Send the data to the screen and increment the column each pass
    for (uint8_t i = 0; i < len - 17 * 6; i++, offsetptr++) {

      memcpy(tmp, offsetptr, 17);
      adjust_buffer(tmp, 17);
      is31fl3731_show(dev, tmp, 17);
      // Wait a bit before moving the screen the first time
      if (i == 0)
        delay(1700);
      delay(300);
    }
  } else {
    memcpy(tmp, offsetptr, 17);
    adjust_buffer(tmp, 17);
    is31fl3731_show(dev, tmp, 17);
  }
}

/* The matrix is internally divided into two matricies.
 * Those matricies alternate columnswise. Additionally,
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

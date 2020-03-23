#include "support.h"
#include "bl.h"
#include "matrix_text.h"
#include "is31fl3731.h"
#include <libopencmsis/core_cm3.h>

void scroll_text(unsigned int i2c, uint8_t i2c_addr, char *text) {

  uint8_t len = (strlen(text) + 17) * 6;

  // allocating a buffer containing all characters to print in binary
  // representation, plus 17 more columns for a full screen of blank 
  // space for more beautiful scrolling
  
  uint8_t text_buffer[len];
  memset(text_buffer, 0, len);

  uint8_t* offsetptr = text_to_buffer(text, text_buffer);

  if(strlen(text) > 3){
    // Send the data to the screen and increment the column each pass
    for (uint8_t i = 0; i < len - 17 * 6; i++, offsetptr++) {
 
      is31fl3731_show(i2c, i2c_addr, offsetptr, 17);
      // Wait a bit before moving the screen the first time
      if(i == 0)
          delay(1700);
      delay(300);
  }
  } else 
    is31fl3731_show(i2c, i2c_addr, offsetptr, 17);
}

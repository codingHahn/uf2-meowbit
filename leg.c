#include "leg.h"

leg_t leg1 = {.base = S2, .tip = S1, .base_deg = 0, .tip_deg = 0, .offset = 0};
leg_t leg2 = {.base = S4, .tip = S3, .base_deg = 0, .tip_deg = 0, .offset = 10};
leg_t leg3 = {.base = S6, .tip = S5, .base_deg = 0, .tip_deg = 0, .offset = 20};
leg_t leg4 = {.base = S8, .tip = S7, .base_deg = 0, .tip_deg = 0, .offset = 30};

void move_leg(i2c_device *dev, leg_t *leg) { 
	servo_set_deg(dev, leg->base, leg->base_deg, leg->offset);
	servo_set_deg(dev, leg->tip, leg->tip_deg, leg->offset+5);
}

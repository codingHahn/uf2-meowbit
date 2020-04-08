#include "leg.h"
#include "i2chelper.h"
#include "fastmath.h"

/*
 * Functions that affect the whole robot.
 * Includes standing up or twisting
 */

typedef struct {
	i2c_device *dev;
	leg_t *leg1;
	leg_t *leg2;
	leg_t *leg3;
	leg_t *leg4;
} robot_t;

void stand_up(robot_t *robot, uint8_t deg);
void swing(robot_t *robot, uint64_t repetitions);
void walk(robot_t *robot);
void walk_sin_int(robot_t *robot, uint64_t counter);
void spin(robot_t *robot);
void spin_sin(robot_t *robot);
void spin_sin_int(robot_t *robot, uint64_t counter);

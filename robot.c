#include "bl.h"
#include "robot.h"
#include "fastmath.h"
#include <math.h>

//TODO: Move movement functions into own header and figure out
//      For what this file should be used

void stand_up(robot_t *robot, uint8_t deg) { 
  servo_set_deg(robot->dev, robot->leg1->tip, deg*robot->leg1->multiplier, robot->leg1->offset);
  robot->leg1->tip_deg = deg*robot->leg1->multiplier;
  servo_set_deg(robot->dev, robot->leg2->tip, deg*robot->leg2->multiplier, robot->leg2->offset);
  robot->leg2->tip_deg = deg*robot->leg2->multiplier;
  servo_set_deg(robot->dev, robot->leg3->tip, deg*robot->leg3->multiplier, robot->leg3->offset);
  robot->leg3->tip_deg = deg*robot->leg3->multiplier;
  servo_set_deg(robot->dev, robot->leg4->tip, deg*robot->leg4->multiplier, robot->leg4->offset);
  robot->leg4->tip_deg = deg*robot->leg4->multiplier;
}

void swing(robot_t *robot, uint64_t repetitions) {
  double phaseshift = 90 * (M_PI / 180);
  uint64_t counter = 0;

  while (counter < repetitions) {
    // TODO: Count actual cycles and not just this while loop
    //	    Maybe through interrups?
    robot->leg1->tip_deg =
        sin(counter * (M_PI / 180)) * 80 * robot->leg1->multiplier;
    robot->leg4->tip_deg =
        sin(counter * (M_PI / 180)) * 80 * robot->leg2->multiplier;
    robot->leg2->tip_deg =
        sin(counter * (M_PI / 180) + phaseshift) * 80 * robot->leg4->multiplier;
    robot->leg3->tip_deg =
        sin(counter * (M_PI / 180) + phaseshift) * 80 * robot->leg3->multiplier;
    counter++;
    move_leg(robot->dev, robot->leg1);
    move_leg(robot->dev, robot->leg2);
    move_leg(robot->dev, robot->leg3);
    move_leg(robot->dev, robot->leg4);
    delay(5);
  }
}

//TODO: Implement a walk function using trigonomitry
void walk(robot_t *robot) {
  while (1) {
    lower_leg(robot->dev, robot->leg2);
    lower_leg(robot->dev, robot->leg4);
    delay(200);
    lift_leg(robot->dev, robot->leg1);
    lift_leg(robot->dev, robot->leg3);

    move_leg_on_ground(robot->dev, robot->leg1, -90);
    move_leg_on_ground(robot->dev, robot->leg3, 0);
    move_leg_on_ground(robot->dev, robot->leg2, 0);
    move_leg_on_ground(robot->dev, robot->leg4, -90);

    delay(400);
    lower_leg(robot->dev, robot->leg1);
    lower_leg(robot->dev, robot->leg3);
    delay(200);
    lift_leg(robot->dev, robot->leg2);
    lift_leg(robot->dev, robot->leg4);

    move_leg_on_ground(robot->dev, robot->leg2, -90);
    move_leg_on_ground(robot->dev, robot->leg4, 0);
    move_leg_on_ground(robot->dev, robot->leg1, 0);
    move_leg_on_ground(robot->dev, robot->leg3, -90);
    delay(400);
  }
}

void walk_sin_int(robot_t *robot, uint64_t counter) {
  float phaseshift = 90;
  float sin1 = fast_sin(counter);
  float sin_shift = fast_sin(counter + phaseshift);
  float trig = clamp((fast_triangle(counter) + 1) /2, 0, 1);
  float trig_shift = clamp((fast_triangle(counter + 180) + 1) / 2, 0, 1);

  // TODO: Count actual cycles and not just this while loop
  //	    Maybe through interrups?
  robot->leg2->tip_deg = trig * 90 * robot->leg2->multiplier;
  robot->leg4->tip_deg = trig * 90 * robot->leg4->multiplier;
  robot->leg1->tip_deg = trig_shift * 90 * robot->leg1->multiplier;
  robot->leg3->tip_deg = trig_shift * 90 * robot->leg3->multiplier;

  robot->leg1->base_deg = (sin_shift * -45 - 45) * robot->leg1->multiplier;
  robot->leg4->base_deg = (sin_shift * -45 - 45) * robot->leg4->multiplier;
  robot->leg2->base_deg = (sin1 * -45 - 45) * robot->leg2->multiplier;
  robot->leg3->base_deg = (sin1 * -45 - 45) * robot->leg3->multiplier;

  move_leg(robot->dev, robot->leg1);
  move_leg(robot->dev, robot->leg2);
  move_leg(robot->dev, robot->leg3);
  move_leg(robot->dev, robot->leg4);
}
void spin_sin(robot_t *robot) {

  double phaseshift = 90 * (M_PI / 180);

  uint64_t counter = 0;

  while (1) {
    // TODO: Count actual cycles and not just this while loop
    //	    Maybe through interrups?
    robot->leg1->tip_deg =
        sin(counter * (M_PI / 180)) * 90 * robot->leg1->multiplier;
    robot->leg3->tip_deg =
        sin(counter * (M_PI / 180)) * 90 * robot->leg3->multiplier;

    robot->leg4->tip_deg =
        sin(counter * (M_PI / 180) + phaseshift) * 90 * robot->leg4->multiplier;
    robot->leg2->tip_deg =
        sin(counter * (M_PI / 180) + phaseshift) * 90 * robot->leg2->multiplier;

    robot->leg2->base_deg = 
        (sin(counter * (M_PI / 180) + phaseshift) * -45 - 45)*  robot->leg2->multiplier;
    robot->leg4->base_deg = 
        (sin(counter * (M_PI / 180) + phaseshift) * -45 - 45)*  robot->leg4->multiplier;
    robot->leg1->base_deg = 
        (sin(counter * (M_PI / 180)) * -45 - 45)* robot->leg1->multiplier;
    robot->leg3->base_deg = 
        (sin(counter * (M_PI / 180)) * -45 - 45)* robot->leg3->multiplier;
    move_leg(robot->dev, robot->leg1);
    move_leg(robot->dev, robot->leg2);
    move_leg(robot->dev, robot->leg3);
    move_leg(robot->dev, robot->leg4);
    delay(5);
    counter++;
  }
}

void spin_sin_int(robot_t *robot, uint64_t counter) {
  float phaseshift = 90;
  float sin1 = fast_sin(counter);
  float sin_shift = fast_sin(counter + phaseshift);

  // TODO: Count actual cycles and not just this while loop
  //	    Maybe through interrups?
  robot->leg2->tip_deg =
      sin_shift * 90 * robot->leg2->multiplier;
  robot->leg4->tip_deg =
      sin_shift * 90 * robot->leg4->multiplier;

  robot->leg1->tip_deg =
      sin1 * 90 * robot->leg1->multiplier;
  robot->leg3->tip_deg =
      sin1 * 90 * robot->leg3->multiplier;

  robot->leg2->base_deg =
      (sin1 * -45 - 45) *
      robot->leg2->multiplier;
  robot->leg4->base_deg =
      (sin1 * -45 - 45) *
      robot->leg4->multiplier;
  robot->leg1->base_deg =
      (sin_shift * -45 - 45) * robot->leg1->multiplier;
  robot->leg3->base_deg =
      (sin_shift * -45 - 45) * robot->leg3->multiplier;
  move_leg(robot->dev, robot->leg1);
  move_leg(robot->dev, robot->leg2);
  move_leg(robot->dev, robot->leg3);
  move_leg(robot->dev, robot->leg4);
}

void spin(robot_t *robot) {
  while (1) {
    delay(200);
    lower_leg(robot->dev, robot->leg2);
    lower_leg(robot->dev, robot->leg4);
    delay(100);
    lift_leg(robot->dev, robot->leg1);
    lift_leg(robot->dev, robot->leg3);
    delay(200);

    move_leg_on_ground(robot->dev, robot->leg1, 0);
    move_leg_on_ground(robot->dev, robot->leg3, 0);
    move_leg_on_ground(robot->dev, robot->leg2, 0);
    move_leg_on_ground(robot->dev, robot->leg4, 0);
    delay(200);

    lower_leg(robot->dev, robot->leg1);
    lower_leg(robot->dev, robot->leg3);
    delay(100);
    lift_leg(robot->dev, robot->leg2);
    lift_leg(robot->dev, robot->leg4);
    delay(200);

    move_leg_on_ground(robot->dev, robot->leg1, -90);
    move_leg_on_ground(robot->dev, robot->leg3, -90);
    move_leg_on_ground(robot->dev, robot->leg2, -90);
    move_leg_on_ground(robot->dev, robot->leg4, -90);
    
  }
}

void dance(robot_t *robot) { 
  while (1) {
    move_leg_on_ground(robot->dev, robot->leg2, -90);
    move_leg_on_ground(robot->dev, robot->leg4, -90);

    delay(200);

    lower_leg(robot->dev, robot->leg1);
    lower_leg(robot->dev, robot->leg3);

    lift_leg(robot->dev, robot->leg2);
    lift_leg(robot->dev, robot->leg4);

    delay(200);

    move_leg_on_ground(robot->dev, robot->leg1, 0);
    move_leg_on_ground(robot->dev, robot->leg3, 0);
    delay(200);

    lower_leg(robot->dev, robot->leg2);
    lower_leg(robot->dev, robot->leg4);
    lift_leg(robot->dev, robot->leg1);
    lift_leg(robot->dev, robot->leg3);

    delay(200);

    move_leg_on_ground(robot->dev, robot->leg2, 0);
    move_leg_on_ground(robot->dev, robot->leg4, 0);

    delay(200);

    lift_leg(robot->dev, robot->leg2);
    lift_leg(robot->dev, robot->leg4);

    lower_leg(robot->dev, robot->leg1);
    lower_leg(robot->dev, robot->leg3);

    delay(200);
    
    move_leg_on_ground(robot->dev, robot->leg1, -90);
    move_leg_on_ground(robot->dev, robot->leg3, -90);

    delay(200);

    lower_leg(robot->dev, robot->leg2);
    lower_leg(robot->dev, robot->leg4);

    lift_leg(robot->dev, robot->leg1);
    lift_leg(robot->dev, robot->leg3);

    delay(200);
  }
}

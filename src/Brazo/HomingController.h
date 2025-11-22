#ifndef HOMING_CONTROLLER_H
#define HOMING_CONTROLLER_H

#include <Arduino.h>
#include "StepperLowLevel.h"
#include "Limites.h"
#include "MotionController.h"

class HomingController
{
public:
  HomingController(StepperLowLevel &driver, Limites &limite, MotionController *motion);

  void goHome();

private:
  StepperLowLevel &_driver;
  Limites &limite;
  MotionController *_motion;
};

#endif

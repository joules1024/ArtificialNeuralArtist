#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Limites.h"
#include "StepperLowLevel.h"

class MotionController
{
public:
  MotionController(AccelStepper &stepper1, AccelStepper &stepper2,
                   MultiStepper &steppers, Limites &limite);

  void setAccelVelocity(int stepper, long aceleracion, long velocidad);
  long getCurrentPosition(int motor);
  void setCurrentPosition(int motor, long pos);
  bool checkMotor(int motor);

  void moveMotor(int motor, long inc);
  void moveAbsolute(long posX, long posY);
  int getValidMode(long posX, long posY);

  void moveSimultaneous(long incX, long incY, long *positions);
  void movePaint(long incX, long incY, bool horizontal);
  void moveOscillating(long incX, long incY, int iterations);

private:
  AccelStepper &_stepper1;
  AccelStepper &_stepper2;
  MultiStepper &_steppers;
  Limites &_limite;

  void _stopOnLimit(int motor);
};

#endif

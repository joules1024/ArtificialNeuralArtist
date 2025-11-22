#ifndef Brazo_h
#define Brazo_h

#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include "Limites.h"
#include "StepperLowLevel.h"
#include "MotionController.h"
#include "../Config/Pinout.h"
#include "../Config/Constants.h"

class Brazo
{
public:
  Brazo(void);
  void goHome();
  void AVset(int stepper, long aceleracion, long velocidad);
  bool mover(long incX, long incY, long resize, int modo, bool info);
  long currentPos(int motor);
  void run();
  void desactivarMotores();
  void printStatus();
  bool getTocoLimiteExterno();

private:
  long _pAnt1, _pAnt2;     // Posicion1 anterior luego de un movimiento
  long _pFinal1, _pFinal2; // Posicion1 luego de aplicar incrementos
  long _positions[2];      // Array para el multistepper
  bool _isMotorEnabled;
  unsigned long _ArmLastTime;
  unsigned long _tArmAux;

  void _DesactivarMotores(unsigned long tiempo);

  StepperLowLevel _driver;
  AccelStepper stepper1 = AccelStepper(motorInterfaceType, STEP1, DIR1);
  AccelStepper stepper2 = AccelStepper(motorInterfaceType, STEP2, DIR2);
  MultiStepper steppers;
  Limites limite;
  MotionController *_motion;
};

#endif

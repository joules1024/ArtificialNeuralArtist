#include "HomingController.h"

HomingController::HomingController(StepperLowLevel &driver, Limites &limite, MotionController *motion)
    : _driver(driver), limite(limite), _motion(motion)
{
}

void HomingController::goHome()
{
  digitalWrite(ENABLE_MOT, LOW);
  delay(100);
  Serial.println("");
  Serial.print("Homing.... ");

  // Fase 1
  while (limite.sw3() == 0 && limite.sw2() == 0)
  {
    _driver.backwardStepCodo();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 2
  for (int cont = 0; cont < 50; cont++)
  {
    _driver.backwardStepHombro();
    delay(2);
  }

  // Fase 3
  while (limite.sw3() == 0 && limite.sw1() == 0)
  {
    _driver.backwardStepHombro();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 4
  while (limite.sw2() == 0)
  {
    _driver.backwardStepCodo();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 5
  while (limite.sw1() == 0)
  {
    _driver.backwardStepHombro();
    delay(2);
  }

  // Fase 6
  while (limite.sw1() > 0)
  {
    _driver.forwardStepHombro();
    delay(5);
  }
  Serial.print(".");

  // Fase 7
  while (limite.sw2() > 0)
  {
    _driver.forwardStepCodo();
    delay(5);
  }
  Serial.print(".");

  // Fase 8
  while (limite.sw3() == 0)
  {
    _driver.forwardStepCodo();
    delay(5);
  }
  Serial.print(".");

  // Fase 9
  while (limite.sw3() > 0)
  {
    _driver.forwardStepHombro();
    delay(5);
  }
  Serial.print(".");

  // Set home
  _motion->setCurrentPosition(1, 0);
  _motion->setCurrentPosition(2, 0);
  delay(500);
  digitalWrite(ENABLE_MOT, HIGH);

  Serial.println("Complete");
}

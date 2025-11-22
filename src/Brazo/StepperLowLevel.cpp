#include "StepperLowLevel.h"

StepperLowLevel::StepperLowLevel(uint8_t enablePin, uint8_t dirHombroPin, uint8_t stepHombroPin, uint8_t dirCodoPin, uint8_t stepCodoPin)
    : _enablePin(enablePin),
      _dirHombroPin(dirHombroPin),
      _stepHombroPin(stepHombroPin),
      _dirCodoPin(dirCodoPin),
      _stepCodoPin(stepCodoPin)
{
}

void StepperLowLevel::begin()
{
    pinMode(_enablePin, OUTPUT);
    pinMode(_dirHombroPin, OUTPUT);
    pinMode(_stepHombroPin, OUTPUT);
    pinMode(_dirCodoPin, OUTPUT);
    pinMode(_stepCodoPin, OUTPUT);
    digitalWrite(_enablePin, HIGH); // deshabilitado al inicio
}

void StepperLowLevel::enable()
{
    digitalWrite(_enablePin, LOW);
}

void StepperLowLevel::disable()
{
    digitalWrite(_enablePin, HIGH);
}

void StepperLowLevel::forwardStepHombro()
{
    enable();
    digitalWrite(_dirHombroPin, LOW);
    digitalWrite(_stepHombroPin, HIGH);
    delay(1);
    digitalWrite(_stepHombroPin, LOW);
}

void StepperLowLevel::backwardStepHombro()
{
    enable();
    digitalWrite(_dirHombroPin, HIGH);
    digitalWrite(_stepHombroPin, HIGH);
    delay(1);
    digitalWrite(_stepHombroPin, LOW);
}

void StepperLowLevel::forwardStepCodo()
{
    enable();
    digitalWrite(_dirCodoPin, LOW);
    digitalWrite(_stepCodoPin, HIGH);
    delay(1);
    digitalWrite(_stepCodoPin, LOW);
}

void StepperLowLevel::backwardStepCodo()
{
    enable();
    digitalWrite(_dirCodoPin, HIGH);
    digitalWrite(_stepCodoPin, HIGH);
    delay(1);
    digitalWrite(_stepCodoPin, LOW);
}

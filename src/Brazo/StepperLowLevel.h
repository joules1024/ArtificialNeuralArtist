#ifndef STEPPER_LOW_LEVEL_H
#define STEPPER_LOW_LEVEL_H

#include <Arduino.h>

class StepperLowLevel
{
public:
    StepperLowLevel(uint8_t enablePin, uint8_t dirHombroPin, uint8_t stepHombroPin, uint8_t dirCodoPin, uint8_t stepCodoPin);

    void begin();

    void enable();
    void disable();

    void forwardStepHombro();
    void backwardStepHombro();
    void forwardStepCodo();
    void backwardStepCodo();

private:
    uint8_t _enablePin;
    uint8_t _dirHombroPin;
    uint8_t _stepHombroPin;
    uint8_t _dirCodoPin;
    uint8_t _stepCodoPin;
};

#endif // STEPPER_LOW_LEVEL_H

#include "MotionController.h"
#include "../Config/Pinout.h"

MotionController::MotionController(AccelStepper &stepper1, AccelStepper &stepper2,
                                   MultiStepper &steppers, Limites &limite)
    : _stepper1(stepper1), _stepper2(stepper2), _steppers(steppers), _limite(limite)
{
}

void MotionController::setAccelVelocity(int stepper, long aceleracion, long velocidad)
{
  if (stepper == 1)
  {
    _stepper1.setMaxSpeed(velocidad);
    _stepper1.setAcceleration(aceleracion);
  }
  else if (stepper == 2)
  {
    _stepper2.setMaxSpeed(velocidad);
    _stepper2.setAcceleration(aceleracion);
  }
}

long MotionController::getCurrentPosition(int motor)
{
  if (motor == 1)
  {
    return _stepper1.currentPosition();
  }
  else if (motor == 2)
  {
    return _stepper2.currentPosition();
  }
  return 0;
}

void MotionController::setCurrentPosition(int motor, long pos)
{
  if (motor == 1)
  {
    _stepper1.setCurrentPosition(pos);
    Serial.println(_stepper1.currentPosition());
  }
  else if (motor == 2)
  {
    _stepper2.setCurrentPosition(pos);
    Serial.println(_stepper2.currentPosition());
  }
}

bool MotionController::checkMotor(int motor)
{
  if (motor == 1)
  {
    return (_stepper1.distanceToGo() != 0);
  }
  else if (motor == 2)
  {
    return (_stepper2.distanceToGo() != 0);
  }
  return false;
}

void MotionController::moveMotor(int motor, long inc)
{
  digitalWrite(ENABLE_MOT, LOW);

  if (motor == 1)
  {
    if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), inc, 0, 1, 2))
    {
      _stepper1.move(inc);

      do
      {
        if (_limite.sw3() == 1)
        {
          _stepper1.stop();
          Serial.print("M1: ");
          Serial.println(getCurrentPosition(1));
        }
      } while (_stepper1.run() && _limite.isInside(1, getCurrentPosition(1), getCurrentPosition(2)));
    }
  }
  else if (motor == 2)
  {
    if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), 0, inc, 2, 2))
    {
      _stepper2.move(inc);

      do
      {
        if (_limite.sw3() == 1)
        {
          _stepper2.stop();
          Serial.print("M2: ");
          Serial.println(getCurrentPosition(2));
        }
      } while (_stepper2.run() && _limite.isInside(2, getCurrentPosition(1), getCurrentPosition(2)));
    }
  }
}

int MotionController::getValidMode(long posX, long posY)
{
  long incX = posX - getCurrentPosition(1);
  long incY = posY - getCurrentPosition(2);
  int outVal = 0;

  if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), incX, 0, 1, 1) &&
      _limite.isValidMovement(posX, getCurrentPosition(2), 0, incY, 2, 1))
  {
    outVal = 3;
  }

  if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), 0, incY, 2, 1) &&
      _limite.isValidMovement(getCurrentPosition(1), posY, incX, 0, 1, 1))
  {
    outVal = outVal + 4;
  }

  if (outVal == 7)
  {
    outVal = 5;
  }

  return outVal;
}

void MotionController::moveAbsolute(long posX, long posY)
{
  long incX = posX - getCurrentPosition(1);
  long incY = posY - getCurrentPosition(2);
  int mode = getValidMode(posX, posY);

  if (mode == 3 || mode == 5)
  {
    moveMotor(1, incX);
    delay(100);
    moveMotor(2, incY);
  }
  else if (mode == 4)
  {
    moveMotor(2, incY);
    delay(100);
    moveMotor(1, incX);
  }
}

void MotionController::_stopOnLimit(int motor)
{
  if (_limite.sw3() == 1)
  {
    if (motor == 1)
    {
      _stepper1.stop();
      Serial.print("M1: ");
      Serial.println(getCurrentPosition(1));
    }
    else if (motor == 2)
    {
      _stepper2.stop();
      Serial.print("M2: ");
      Serial.println(getCurrentPosition(2));
    }
    else
    {
      _stepper1.stop();
      _stepper2.stop();
      Serial.print("M1: ");
      Serial.println(getCurrentPosition(1));
      Serial.print("M2: ");
      Serial.println(getCurrentPosition(2));
    }
  }
}

void MotionController::moveSimultaneous(long incX, long incY, long *positions)
{
  positions[0] = getCurrentPosition(1) + incX;
  positions[1] = getCurrentPosition(2) + incY;

  int valid = getValidMode(positions[0], positions[1]);

  if (valid == 5)
  {
    _steppers.moveTo(positions);

    do
    {
      _stopOnLimit(0);
    } while (_steppers.run() &&
             _limite.isInside(2, getCurrentPosition(1), getCurrentPosition(2)) &&
             _limite.isInside(1, getCurrentPosition(1), getCurrentPosition(2)));
  }
  else if (valid == 3)
  {
    moveMotor(1, incX);
    delay(100);
    moveMotor(2, incY);
  }
  else if (valid == 4)
  {
    moveMotor(2, incY);
    delay(100);
    moveMotor(1, incX);
  }
}

void MotionController::movePaint(long incX, long incY, bool horizontal)
{
  long pasos;
  long UincX, UincY;
  long pos1 = getCurrentPosition(1);
  long pos2 = getCurrentPosition(2);
  long finalX = pos1 + incX;
  long finalY = pos2 + incY;

  if (horizontal)
  {
    pasos = abs(incX) / (1L << random(3, 6));
    if (pasos == 0) pasos = 2;

    UincX = incX / pasos;
    UincY = incY / pasos;

    for (int a = 1; a <= pasos; a++)
    {
      if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), 0, incY, 2, 0))
      {
        _stepper2.move(incY);
        do
        {
          _stopOnLimit(2);
        } while (_stepper2.run() && _limite.isInside(2, getCurrentPosition(1), getCurrentPosition(2)));
        incY = -incY;
      }

      delay(100);

      if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), UincX, 0, 1, 0))
      {
        _stepper1.move(UincX);
        do
        {
          _stopOnLimit(1);
        } while (_stepper1.run() && _limite.isInside(1, getCurrentPosition(1), getCurrentPosition(2)));
      }

      delay(100);
    }
  }
  else
  {
    pasos = abs(incY) / (1L << random(3, 6));
    if (pasos == 0) pasos = 2;

    UincX = incX / pasos;
    UincY = incY / pasos;

    for (int a = 1; a <= pasos; a++)
    {
      if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), incX, 0, 1, 0))
      {
        _stepper1.move(incX);
        do
        {
          _stopOnLimit(1);
        } while (_stepper1.run() && _limite.isInside(1, getCurrentPosition(1), getCurrentPosition(2)));
        incX = -incX;
      }

      delay(100);

      if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), 0, UincY, 2, 0))
      {
        _stepper2.move(UincY);
        do
        {
          _stopOnLimit(2);
        } while (_stepper2.run() && _limite.isInside(2, getCurrentPosition(1), getCurrentPosition(2)));
      }

      delay(100);
    }
  }

  if (getCurrentPosition(1) != finalX || getCurrentPosition(2) != finalY)
  {
    moveAbsolute(finalX, finalY);
    Serial.print("*");
  }
}

void MotionController::moveOscillating(long incX, long incY, int iterations)
{
  if (_limite.isValidMovement(getCurrentPosition(1), getCurrentPosition(2), incX, incY, 5, 0))
  {
    _stepper1.move(incX);
    _stepper2.move(incY);

    int cont1 = 0;
    int cont2 = 0;

    do
    {
      if (cont2 < iterations)
      {
        _stepper2.run();
        _limite.tocoExtremos();

        if (_stepper2.distanceToGo() == 0 || _limite.antebrazoSW() > 0)
        {
          incY = -incY;
          _stepper2.move(incY);
          cont2++;
        }
      }

      if (cont1 < iterations)
      {
        _stepper1.run();
        _limite.tocoExtremos();

        if (_stepper1.distanceToGo() == 0 || _limite.antebrazoSW() > 0)
        {
          incX = -incX;
          _stepper1.move(incX);
          cont1++;
        }
      }
    } while ((cont1 + cont2) < iterations * 2);
  }
}

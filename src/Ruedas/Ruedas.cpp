#include "Ruedas.h"

Ruedas::Ruedas(void)
{
  pinMode(ENABLE_MOTORES, OUTPUT); // activo LOW
  pinMode(DIR3, OUTPUT);
  pinMode(STEP3, OUTPUT);
  pinMode(DIR4, OUTPUT);
  pinMode(STEP4, OUTPUT);
  digitalWrite(ENABLE_MOTORES, HIGH); // deshabilito motores

  // Acomodo la direccion al cableado
  stepper1.setPinsInverted(false, false, false);
  stepper2.setPinsInverted(true, false, false);
}

long Ruedas::mayor(long a, long b)
{
  if (a > b)
    return a;
  else
    return b;
}

// devuelve la posicion actual del motor n
long Ruedas::currentPos(int motor)
{
  long result;
  if (motor == 3)
    return stepper1.currentPosition();
  if (motor == 4)
    return stepper2.currentPosition();
  return 0;
}

// Setea la posicion del motor n
void Ruedas::_setCurrentPos(int motor, long pos)
{

  if (motor == 3)
    stepper1.setCurrentPosition(pos);
  if (motor == 4)
    stepper2.setCurrentPosition(pos);
}

void Ruedas::AVset(long a1, long v1, long a2, long v2) // Setea la acelercion y velocidad
{
  stepper1.setMaxSpeed(v1);
  stepper1.setAcceleration(a1);
  stepper2.setMaxSpeed(v2);
  stepper2.setAcceleration(a2);
}

void Ruedas::MoverRuedas(long modo, long inc, long vel, long acel)
{
  digitalWrite(ENABLE_MOTORES, LOW); // Habilito motores
  AVset(acel, vel, acel, vel);

  switch (modo)
  {
    case 1:
      stepper1.move(inc);
      stepper2.move(inc);
      break;
    case 2:
      stepper1.move(-inc);
      stepper2.move(-inc);
      break;
    case 3:
      stepper1.move(inc);
      stepper2.move(-inc);
      break;
    case 4:
      stepper1.move(-inc);
      stepper2.move(inc);
      break;
    case 5:
      stepper1.move(0);
      stepper2.move(-inc);
      break;
    case 6:
      stepper1.move(0);
      stepper2.move(inc);
      break;
    case 7:
      stepper1.move(inc);
      stepper2.move(0);
      break;
    case 8:
      stepper1.move(-inc);
      stepper2.move(0);
      break;
  }

  // BLOQUEANTE
  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0)
  {
    stepper1.run();
    stepper2.run();
  }

  // _mWheelOff = true;
  // _WheelsLastTime = millis(); // ultimo momento de uso
  digitalWrite(ENABLE_MOTORES, HIGH); // deshabilito motores
}

// tipo 1: giran las dos ruedas
// tipo 2: gira la derecha
// tipo 3: gira la izquierda
void Ruedas::Giro(int dist, int tipo, int vel, int acel)
{
  long inc1 = 0, inc2 = 0;

  switch (tipo)
  {
    case 1: // ambas ruedas
      inc1 = dist;
      inc2 = -dist;
      break;

    case 2: // derecha
      inc1 = dist;
      inc2 = 0;
      break;

    case 3: // izquierda
      inc1 = 0;
      inc2 = -dist;
      break;

    default:
      // si tipo inválido no mueve nada
      inc1 = 0;
      inc2 = 0;
      break;
  }

  AVset(acel, vel, acel, vel);
  stepper1.move(inc1);
  stepper2.move(inc2);

  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0)
  {
    stepper1.run();
    stepper2.run();
  }
}

void Ruedas::loop()
{
  if (abs(stepper1.distanceToGo()) > 0)
    stepper1.run();
  if (abs(stepper2.distanceToGo()) > 0)
    stepper2.run();
}

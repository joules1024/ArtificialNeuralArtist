#include "Brazo.h"

// =====================================================
// CONSTRUCTOR Y CONFIGURACIÓN
// =====================================================
Brazo::Brazo(void) : _driver(ENABLE_MOT, DIR1, STEP1, DIR2, STEP2)
{
  _driver.begin();
  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);
  stepper1.setPinsInverted(true, false, false);
  stepper2.setPinsInverted(true, false, false);
  _isMotorEnabled = false;
}

void Brazo::desactivarMotores()
{
  digitalWrite(ENABLE_MOT, HIGH);
  _isMotorEnabled = false;
}

// Setea la acelercion y velocidad  del motor n
void Brazo::AVset(int stepper, long aceleracion, long velocidad)
{
  if (stepper == 1)
  {
    stepper1.setMaxSpeed(velocidad);
    stepper1.setAcceleration(aceleracion);
  }
  if (stepper == 2)
  {
    stepper2.setMaxSpeed(velocidad);
    stepper2.setAcceleration(aceleracion);
  }
}
// Devuelve la posicion actual del motor n
void Brazo::printStatus()
{
  Serial.print("M1: ");
  Serial.println(currentPos(1));
  Serial.print("M2: ");
  Serial.println(currentPos(2));
}

// =====================================================
// LÓGICA PRINCIPAL DE MOVIMIENTO
// =====================================================

// Modos de dibujo 1,2,3,4,5,6,7,8,9 Resize 0 100% para escalar, Devuelve True si el motor se movio
bool Brazo::mover(long incX, long incY, long resize, int modo, bool info)
{
  bool result = false;
  digitalWrite(ENABLE_MOT, LOW); // Habilito motores

  // escalado
  incX = (incX * resize) / 100;
  incY = (incY * resize) / 100;

  // posiciones actuales
  const long pos1 = currentPos(1);
  const long pos2 = currentPos(2);

  // aceleracion y velocidad aleatoria
  long UincX, UincY;
  long A1 = random(AVMIN, AVMAX);
  long V1 = random(AVMIN, AVMAX);
  long A2 = random(AVMIN, AVMAX);
  long V2 = random(AVMIN, AVMAX);
  AVset(1, A1, V1);
  AVset(2, A2, V2);

  _pAnt1 = pos1;
  _pAnt2 = pos2;
  _pFinal1 = pos1 + incX;
  _pFinal2 = pos2 + incY;

  if (info)
  {
    Serial.println(F("INFO MOVER"));
    Serial.println("\t Modo: " + String(modo));
    Serial.println("\t AV1: " + String(A1) + ", " + String(V1));
    Serial.println("\t AV2: " + String(A2) + ", " + String(V2));
    Serial.print("\t Tamaño: " + String(resize));
    Serial.println("\t Incrementos: " + String(incX) + ", " + String(incY));
    Serial.println("\t Vieja Pos: " + String(_pAnt1) + ", " + String(_pAnt2));
    Serial.println("\t Nueva Pos: " + String(_pFinal1) + ", " + String(_pFinal2));
  }

  switch (modo)
  {
    /* ===================================================
     *   1) SOLO MOTOR 1
     * =================================================== */
    case MODO_M1_SOLO:
      _moverMotor(1, incX);
      break;

    /* ===================================================
     *   2) SOLO MOTOR 2
     * =================================================== */
    case MODO_M2_SOLO:
      _moverMotor(2, incY);
      break;

    /* ===================================================
     *   3) M1 → luego M2
     * =================================================== */
    case MODO_M1_LUEGO_M2:
      _moverMotor(1, incX);
      delay(100);
      _moverMotor(2, incY);
      break;

    /* ===================================================
     *   4) M2 → luego M1
     * =================================================== */
    case MODO_M2_LUEGO_M1:
      _moverMotor(2, incY);
      delay(100);
      _moverMotor(1, incX);
      break;

    /* ===================================================
     *   5) SIMULTÁNEO
     * =================================================== */
    case MODO_SIMULTANEO:
    {
      AVset(1, A1 / 4, V1 / 4);
      AVset(2, A2 / 4, V2 / 4);

      _positions[0] = pos1 + incX;
      _positions[1] = pos2 + incY;

      int valid = _getValidMode(_positions[0], _positions[1]);

      if (valid == 5)
      {
        steppers.moveTo(_positions);

        do
        {
          if (limite.sw3() == 1)
          {
            stepper1.stop();
            stepper2.stop();
            printStatus();
          }
        } while (steppers.run() && limite.isInside(2, currentPos(1), currentPos(2)) &&
                 limite.isInside(1, currentPos(1), currentPos(2)));
      }
      else if (valid == 3)
      {
        _moverMotor(1, incX);
        delay(100);
        _moverMotor(2, incY);
      }
      else if (valid == 4)
      {
        _moverMotor(2, incY);
        delay(100);
        _moverMotor(1, incX);
      }

      if (info)
      {
        Serial.print(F("\t VelSimultanea: "));
        Serial.println((V1 + V2) / 4);
        Serial.println();
      }
      break;
    }

    /* ===================================================
     *   6) SOMBREADO HORIZONTAL
     * =================================================== */
    case MODO_PINTAR_HORIZONTAL:
    {
      long pasos = abs(incX) / (1L << random(3, 6));
      if (pasos == 0)
        pasos = 2;

      long Xfinal = pos1 + incX;
      long Yfinal = pos2 + incY;

      UincX = incX / pasos;
      UincY = incY / pasos;

      for (int a = 1; a <= pasos; a++)
      {
        if (limite.isValidMovement(currentPos(1), currentPos(2), 0, incY, 2, 0))
        {
          stepper2.move(incY);
          do
          {
            if (limite.sw3())
            {
              stepper2.stop();
              printStatus();
            }
          } while (stepper2.run() && limite.isInside(2, currentPos(1), currentPos(2)));
          incY = -incY;
        }

        delay(100);

        if (limite.isValidMovement(currentPos(1), currentPos(2), UincX, 0, 1, 0))
        {
          stepper1.move(UincX);
          do
          {
            if (limite.sw3())
            {
              stepper1.stop();
              printStatus();
            }
          } while (stepper1.run() && limite.isInside(1, currentPos(1), currentPos(2)));
        }

        delay(100);
      }

      if (currentPos(1) != Xfinal || currentPos(2) != Yfinal)
      {
        _moverAbsoluto(Xfinal, Yfinal);
        Serial.print("*");
      }
      break;
    }

    /* ===================================================
     *   7) SOMBREADO VERTICAL
     * =================================================== */
    case MODO_PINTAR_VERTICAL:
    {
      long Xfinal = pos1 + incX;
      long Yfinal = pos2 + incY;
      long pasos = abs(incY) / (1L << random(3, 6));
      if (pasos == 0)
        pasos = 2;

      UincX = incX / pasos;
      UincY = incY / pasos;

      for (int a = 1; a <= pasos; a++)
      {
        if (limite.isValidMovement(currentPos(1), currentPos(2), incX, 0, 1, 0))
        {
          stepper1.move(incX);
          do
          {
            if (limite.sw3())
            {
              stepper1.stop();
              printStatus();
            }
          } while (stepper1.run() && limite.isInside(1, currentPos(1), currentPos(2)));
          incX = -incX;
        }

        delay(100);

        if (limite.isValidMovement(currentPos(1), currentPos(2), 0, UincY, 2, 0))
        {
          stepper2.move(UincY);
          do
          {
            if (limite.sw3())
            {
              stepper2.stop();
              printStatus();
            }
          } while (stepper2.run() && limite.isInside(2, currentPos(1), currentPos(2)));
        }

        delay(100);
      }

      if (currentPos(1) != Xfinal || currentPos(2) != Yfinal)
      {
        _moverAbsoluto(Xfinal, Yfinal);
        Serial.print("*");
      }
      break;
    }

    /* ===================================================
     *   8) CURVAS
     * =================================================== */
    case MODO_CURVAS:
    {
      int iteraciones = random(1, 10);
      int m = 10;

      if (random(2) == 0)
      {
        A1 = random(AVMIN, AVMAX);
        V1 = random(AVMIN, AVMAX);
        A2 = A1 * random(1, m);
        V2 = V1 * random(1, m);
      }
      else
      {
        A2 = random(AVMIN, AVMAX);
        V2 = random(AVMIN, AVMAX);
        A1 = A2 * random(1, m);
        V1 = V2 * random(1, m);
      }

      AVset(1, A1, V1);
      AVset(2, A2, V2);

      int cont1 = 0;
      int cont2 = 0;
      int cont = 0;
      bool tocoLimitesExtremos = false;

      if (limite.isValidMovement(currentPos(1), currentPos(2), incX, incY, 5, 0))
      {
        stepper1.move(incX);
        stepper2.move(incY);

        do
        {
          if (cont2 < iteraciones)
          {
            stepper2.run();
            limite.tocoExtremos(); // valido si toco Sw1 o Sw2 para activar _parkAfter

            if (stepper2.distanceToGo() == 0 || limite.antebrazoSW() > 0)
            {
              incY = -incY;
              stepper2.move(incY);
              cont2++;
            }
          }

          if (cont1 < iteraciones)
          {
            stepper1.run();
            limite.tocoExtremos(); // valido si toco Sw1 o Sw2 para activar _parkAfter
            if (stepper1.distanceToGo() == 0 || limite.antebrazoSW() > 0)
            {
              incX = -incX;
              stepper1.move(incX);
              cont1++;
            }
          }

        } while ((cont1 + cont2) < iteraciones * 2);
      }

      if (info)
      {
        Serial.print("\t ITERACION  ");
        Serial.println(cont);
      }
      break;
    }

    /* ===================================================
     *   9) CÍRCULOS
     * =================================================== */
    case MODO_CIRCULO:
    {
      A1 = random(AVMIN, AVMAX);
      V1 = random(AVMIN, AVMAX);
      A2 = A1 = 1000;
      V2 = V1 = 1000;
      AVset(1, A1, V1);
      AVset(2, A2, V2);

      if (limite.isValidMovement(currentPos(1), currentPos(2), incX, incY, 5, 0))
      {
        stepper1.move(50);
        stepper2.move(50);

        long circleIndex = 50;

        for (long i = 0; i < circleIndex * 2; i++)
        {
          Serial.println(F("PasoX"));
          stepper2.run();
          if (stepper2.distanceToGo() == 0 || limite.antebrazoSW() > 0)
          {
            incY = -incY;
            stepper2.move(incY);
          }

          Serial.println(F("PasoY"));
          stepper1.run();
          if (stepper1.distanceToGo() == 0 || limite.antebrazoSW() > 0)
          {
            incX = -incX;
            stepper1.move(incX);
          }
        }
      }
      break;
    }

    /* ===================================================
     *   DEFAULT (modo desconocido)
     * =================================================== */
    default:
      Serial.println(F("Modo no válido"));
      break;
  }

  result = true;
  _isMotorEnabled = true;
  _ArmLastTime = millis();
  delay(500);
  return result;
}

void Brazo::run() {}

// =====================================================
// IMPLEMENTACIÓN DE MODOS (PRIVADOS)
// =====================================================

// Desactivan los motores luego de un timepo inactividad
void Brazo::_DesactivarMotores(unsigned long tiempo)
{
  tiempo = tiempo * 1000;
  _tArmAux = millis() - _ArmLastTime;

  if ((_isMotorEnabled == true) and (digitalRead(ENABLE_MOT) == LOW))
  {
    if (_tArmAux > tiempo)
    {
      _isMotorEnabled = false;
      digitalWrite(ENABLE_MOT, HIGH); // desactivo los motores  luego de un tiempo en mseg
      Serial.println("Motores desactivados");
    }
  }
}

// =====================================================
// Mueve el motor si el isValidMovement se lo permite
void Brazo::_moverMotor(int motor, long inc)
{
  digitalWrite(ENABLE_MOT, LOW); // Habilito motores
  if (motor == 1)
  {
    if (limite.isValidMovement(currentPos(1), currentPos(2), inc, 0, 1, 2))
    {
      stepper1.move(inc);

      do
      {
        if (limite.sw3() == 1)
        {
          stepper1.stop();
          printStatus();
        }

      } while (stepper1.run() and limite.isInside(1, currentPos(1), currentPos(2)));
    }
  }
  if (motor == 2)
  {
    if (limite.isValidMovement(currentPos(1), currentPos(2), 0, inc, 2, 2))
    {

      stepper2.move(inc);
      do
      {
        if (limite.sw3() == 1)
        {
          printStatus();
          stepper2.stop();
        }

      } while (stepper2.run() and limite.isInside(2, currentPos(1), currentPos(2)));
    }
  }
}

// =====================================================
// Modos validos de movimiento desde pos actual hacia posXY (absoluto) devuelve 0,3,4,5
int Brazo::_getValidMode(long posX, long posY)
{
  long incX = posX - currentPos(1);
  long incY = posY - currentPos(2);
  int outVal = 0;

  // Primro X y luego Y
  if (limite.isValidMovement(currentPos(1), currentPos(2), incX, 0, 1, 1) and
      limite.isValidMovement(posX, currentPos(2), 0, incY, 2, 1))
  {

    outVal = 3; // modo 3 primero incX y luego incY
  }
  // Priemro Y y luego X
  if (limite.isValidMovement(currentPos(1), currentPos(2), 0, incY, 2, 1) and
      limite.isValidMovement(currentPos(1), posY, incX, 0, 1, 1))
  {

    outVal = outVal + 4;
  }

  if (outVal == 7)
  {
    outVal = 5;
  }
  return outVal;
}

// =====================================================
// Usado por ej en funcion para cerrar dibujos modos 3 y 4
void Brazo::_moverAbsoluto(long posX, long posY)

{
  long IncX = posX - currentPos(1);
  long IncY = posY - currentPos(2);
  int mode = _getValidMode(posX, posY);

  if (mode == 3 or mode == 5)
  {
    _moverMotor(1, IncX);
    delay(100);
    _moverMotor(2, IncY);
  }
  else if (mode == 4)
  {
    _moverMotor(2, IncY);
    delay(100);
    _moverMotor(1, IncX);
  }
}

// =====================================================
// Devuelve true si el motor aun tiene recorrido pendiente
bool Brazo::_checkMotor(int motor)

{
  bool result = false;
  if (motor == 1)
  {
    if (abs(stepper1.distanceToGo()) > 0)
      result = true;
  }
  if (motor == 2)
  {
    if (abs(stepper2.distanceToGo()) > 0)
      result = true;
  }
  return result;
}

// =====================================================
// Devuelve la posicion actual del motor n
long Brazo::currentPos(int motor)

{
  long result;
  if (motor == 1)
  {
    result = stepper1.currentPosition();
  }
  if (motor == 2)
  {
    result = stepper2.currentPosition();
  }

  return result;
}

//=====================================================
// Setea la posicion del motor seleccionado
void Brazo::_setCurrentPos(int motor, long pos)

{
  if (motor == 1)
  {
    stepper1.setCurrentPosition(pos);
    Serial.println(stepper1.currentPosition());
  }

  if (motor == 2)
  {
    stepper2.setCurrentPosition(pos);
    Serial.println(stepper2.currentPosition());
  }
}

bool Brazo::getTocoLimiteExterno()
{
  return limite.tocoLimiteExterno;
}

// =====================================================
// Estaciona el brazo suavemente
void Brazo::goHome()
{
  digitalWrite(ENABLE_MOT, LOW);
  delay(100);
  Serial.println("");
  Serial.print("Homing.... ");

  // Fase 1: Retroceder codo hasta liberar sw3 y sw2
  while (limite.sw3() == 0 && limite.sw2() == 0)
  {
    _driver.backwardStepCodo();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 2: Separar del switch sw3 (50 pasos hacia atrás en hombro)
  for (int cont = 0; cont < 50; cont++)
  {
    _driver.backwardStepHombro();
    delay(2);
  }

  // Fase 3: Retroceder hombro hasta liberar sw3 y sw1
  while (limite.sw3() == 0 && limite.sw1() == 0)
  {
    _driver.backwardStepHombro();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 4: Retroceder codo hasta liberar sw2
  while (limite.sw2() == 0)
  {
    _driver.backwardStepCodo();
    delay(2);
  }
  delay(100);
  Serial.print("..");

  // Fase 5: Retroceder hombro hasta liberar sw1
  while (limite.sw1() == 0)
  {
    _driver.backwardStepHombro();
    delay(2);
  }

  // Fase 6: Avanzar hombro hasta soltar sw1
  while (limite.sw1() > 0)
  {
    _driver.forwardStepHombro();
    delay(5);
  }
  Serial.print(".");

  // Fase 7: Avanzar codo hasta soltar sw2
  while (limite.sw2() > 0)
  {
    _driver.forwardStepCodo();
    delay(5);
  }
  Serial.print(".");

  // Fase 8: Avanzar codo hasta tocar sw3
  while (limite.sw3() == 0)
  {
    _driver.forwardStepCodo();
    delay(5);
  }
  Serial.print(".");

  // Fase 9: Avanzar hombro hasta soltar sw3
  while (limite.sw3() > 0)
  {
    _driver.forwardStepHombro();
    delay(5);
  }
  Serial.print(".");

  // Establecer posición home
  stepper1.setCurrentPosition(0);
  stepper2.setCurrentPosition(0);
  delay(500);
  desactivarMotores();
  Serial.println("Complete");
}
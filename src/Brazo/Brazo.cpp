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
  _motion = new MotionController(stepper1, stepper2, steppers, limite);
  _isMotorEnabled = false;
}

void Brazo::desactivarMotores()
{
  digitalWrite(ENABLE_MOT, HIGH);
  _isMotorEnabled = false;
}

void Brazo::AVset(int stepper, long aceleracion, long velocidad)
{
  _motion->setAccelVelocity(stepper, aceleracion, velocidad);
}
void Brazo::printStatus()
{
  Serial.print("M1: ");
  Serial.println(_motion->getCurrentPosition(1));
  Serial.print("M2: ");
  Serial.println(_motion->getCurrentPosition(2));
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
  const long pos1 = _motion->getCurrentPosition(1);
  const long pos2 = _motion->getCurrentPosition(2);

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
      _motion->moveMotor(1, incX);
      break;

    /* ===================================================
     *   2) SOLO MOTOR 2
     * =================================================== */
    case MODO_M2_SOLO:
      _motion->moveMotor(2, incY);
      break;

    /* ===================================================
     *   3) M1 → luego M2
     * =================================================== */
    case MODO_M1_LUEGO_M2:
      _motion->moveMotor(1, incX);
      delay(100);
      _motion->moveMotor(2, incY);
      break;

    /* ===================================================
     *   4) M2 → luego M1
     * =================================================== */
    case MODO_M2_LUEGO_M1:
      _motion->moveMotor(2, incY);
      delay(100);
      _motion->moveMotor(1, incX);
      break;

    /* ===================================================
     *   5) SIMULTÁNEO
     * =================================================== */
    case MODO_SIMULTANEO:
    {
      AVset(1, A1 / 4, V1 / 4);
      AVset(2, A2 / 4, V2 / 4);

      _motion->moveSimultaneous(incX, incY, _positions);

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
      _motion->movePaint(incX, incY, true);
      break;
    }

    /* ===================================================
     *   7) SOMBREADO VERTICAL
     * =================================================== */
    case MODO_PINTAR_VERTICAL:
    {
      _motion->movePaint(incX, incY, false);
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

      _motion->moveOscillating(incX, incY, iteraciones);

      if (info)
      {
        Serial.print("\t ITERACIONES: ");
        Serial.println(iteraciones);
      }
      break;
    }

    /* ===================================================
     *   9) CÍRCULOS
     * =================================================== */
    case MODO_CIRCULO:
    {
      A1 = 1000;
      V1 = 1000;
      A2 = 1000;
      V2 = 1000;
      AVset(1, A1, V1);
      AVset(2, A2, V2);

      const int circleIterations = 50;
      _motion->moveOscillating(50, 50, circleIterations);
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





long Brazo::currentPos(int motor)
{
  return _motion->getCurrentPosition(motor);
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
  _motion->setCurrentPosition(1, 0);
  _motion->setCurrentPosition(2, 0);
  delay(500);
  desactivarMotores();
  Serial.println("Complete");
}
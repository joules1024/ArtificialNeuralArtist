#include "Limites.h"

Limites::Limites(void)
{
  pinMode(HSW1, INPUT_PULLUP); // home M1
  pinMode(HSW2, INPUT_PULLUP); // home M2
  pinMode(HSW3, INPUT_PULLUP); // home M3 ANTEBRAZO

  _sw1Ant = 0;
  _sw2Ant = 0;
  _sw3Ant = 0;
  _HSW3anterior = false;
  _HSW3Ant = 0;
}

byte Limites::checkSwitch(byte switchPin, byte &switchAnt)
{
  bool switchState = digitalRead(switchPin);
  byte out = 0;

  if (switchState == HIGH)
  { // Libreado
    if (switchAnt > 0)
      delay(50); // debounce
    switchAnt = 0;
  }
  else
  { //  Presionado
    if (switchAnt == 0)
    {
      out = 1;
      switchAnt = 1;
      delay(50); // debounce
    }
    else if (switchAnt == 1)
    {
      out = 2;
    }
  }

  return out;
}

byte Limites::sw1()
{
  const int status = checkSwitch(HSW1, _sw1Ant);
  if (status == 1)
    Serial.println(F("sw1 - closed "));
  return status;
}

byte Limites::sw2()
{
  const int status = checkSwitch(HSW2, _sw2Ant);
  if (status == 1)
  {

    Serial.println(F("\nsw2 - closed "));
  }
  return status;
}

byte Limites::sw3()
{
  const int status = checkSwitch(HSW3, _sw3Ant);
  if (status == 1)
    Serial.println(F("sw3 - closed "));
  return status;
}

byte Limites::antebrazoSW()
{
  const int status = checkSwitch(HSW3, _sw3Ant);
  if (status == 1)
    Serial.println(F("sw3 - closed "));
  return status;
}

void Limites::tocoExtremos()
{
  tocoLimiteExterno = (sw1() > 0 || sw2() > 0);
}

// Limites Absolutos M1
long Limites::limiteInfM1(long currentM2)
{
  long M1 = currentM2 / 1.2;
  return M1;
}

// Limites Absolutos M1
long Limites::limiteSupM1(long currentM2)
{
  long M1;
  if (currentM2 <= MID_MOTORC)
    M1 = ((currentM2 / 1.2) + MID_MOTORH);
  if (currentM2 > MID_MOTORC)
    M1 = MAX_MOTORH;
  return M1;
}

// Limites Absolutos M2
long Limites::limiteInfM2(long currentM1)
{
  long M2;
  if (currentM1 <= MID_MOTORH)
    M2 = MIN_MOTORC;
  if (currentM1 > MID_MOTORH and currentM1 <= MAX_MOTORH)
    M2 = 1.2 * (currentM1 - MID_MOTORH);
  return M2;
}

// Limites Absolutos M2
long Limites::limiteSupM2(long currentM1)
{
  long M2 = 1.2 * currentM1;
  return M2;
}

// Validacion  de posiciones del brazo: M1 Hombro, M2 Codo  TRUE = OK
bool Limites::isValidMovement(long actualM1, long actualM2, long incM1, long incM2, int modo, int info)
{
  bool result = true;
  long newM1 = actualM1 + incM1; // nueva posicion X
  long newM2 = actualM2 + incM2; // nueva posicion Y

  if (modo == 1 || modo == 2)
  {
    if (newM1 < limiteInfM1(actualM2) || newM1 > limiteSupM1(actualM2))
      result = false;

    if (newM2 < limiteInfM2(actualM1) || newM2 > limiteSupM2(actualM1))
      result = false;
  }

  // Validación especial modo 5
  if (modo == 5)
  {
    long inf = limiteInfM2(actualM1);
    long sup = limiteSupM2(actualM1);

    if (inf >= sup)
      result = false;
  }

  if ((info > 1) && (_INFO == 1))
  {
    if (!result)
    {
      Serial.println(F("\n\t\tARM CONSTRAIN ERROR:"));
      Serial.println("\t\t Actual M1, M2: \t" + String(actualM1) + ", " + String(actualM2));
      Serial.print("\t\t Inc M1, M2:  \t\t" + String(incM1) + ", " + String(incM2) + "\n\t\t Limites M1(" +
                   String(actualM2) + "): \t" + String(limiteInfM1(actualM2)) + " - " + String(limiteSupM1(actualM2)) +
                   " [" + String(newM1) + "]");

      if (newM1 > limiteSupM1(actualM2) or newM1 < limiteInfM1(actualM2))
        Serial.print(F(" ** \n"));
      else
        Serial.print("\n");

      Serial.print("\t\t Limites M2(" + String(actualM1) + "): \t" + String(limiteInfM2(actualM1)) + " - " +
                   String(limiteSupM2(actualM1)) + " [" + String(newM2) + "]");

      if (newM2 > limiteSupM2(actualM1) or newM2 < limiteInfM2(actualM1))
        Serial.print(F(" ** \n"));
      else
        Serial.print("\n");
      Serial.println();
    }
  }

  return result;
}

bool Limites::isInside(long motor, long currentM1, long currentM2)

{ // TRUE si esta en cond de moverse,  FALSE llego al limite
  bool result = true;
  if (motor == 1)
  {
    if ((currentM1 < limiteInfM1(currentM2)) or (currentM1 > limiteSupM1(currentM2)))
    {
      result = false;
      Serial.println("\nMOTOR 1 LIMIT FALSE");
    }
  }
  if (motor == 2)
  {
    if ((currentM2 < limiteInfM2(currentM1)) or (currentM2 > limiteSupM2(currentM1)))
    {
      result = false;
      Serial.println("\nMOTOR 2 LIMIT FALSE");
    }
  }
  return result;
}

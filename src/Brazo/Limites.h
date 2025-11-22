#ifndef Limites_h
#define Limites_h

#include <Arduino.h>
#include "../Config/Pinout.h"
#include "../Config/Constants.h"

/*
   Clase Limites
   --------------
   Se encarga de leer los sensores de fin de carrera (HSW1, HSW2, HSW3)
   y calcular los límites mecánicos permitidos para los motores del brazo.

   Motores:
     - M1 = Hombro
     - M2 = Codo

   Los métodos permiten:
     * Saber si un switch está libre, recién presionado o sostenido.
     * Calcular los límites inferiores y superiores de cada motor.
     * Validar si un movimiento es seguro antes de realizarlo.
     * Verificar si la posición actual está dentro de los límites.
*/
class Limites
{
public:
  // Constructor: configura los pines de los switches como INPUT_PULLUP
  Limites(void);

  bool _HSW3anterior; // Estado previo del switch 3 (si se usaba en versiones viejas)
  int _HSW3Ant;       // Estado previo del switch 3 para transiciones

  /*
     Lectura de switches
     -------------------
     Devuelven:
       0 = libre
       1 = recién presionado
       2 = continúa presionado
  */
  byte sw1();         // Switch M1 (Hombro)
  byte sw2();         // Switch M2 (Codo)
  byte sw3();         // Switch M3 (Antebrazo)
  byte antebrazoSW(); // Alias de sw3()

  /*
     Límites absolutos de M1 según la posición actual de M2
  */
  long limiteInfM1(long currentM2);
  long limiteSupM1(long currentM2);

  /*
     Límites absolutos de M2 según la posición actual de M1
  */
  long limiteInfM2(long currentM1);
  long limiteSupM2(long currentM1);

  /*
     Verifica si un movimiento propuesto es válido.
     actualM1/actualM2 = posiciones actuales
     incM1/incM2       = incrementos propuestos
     modo              = tipo de validación (1,2,5 según tu lógica)
     info              = nivel de debug
     Devuelve false si el movimiento se sale de los límites.
  */
  bool isValidMovement(long ActualM1, long ActualM2, long incM1, long incM2, int modo, int info);

  /*
     Verifica si la posición actual del motor está dentro de los límites.
     motor = 1 (M1) o 2 (M2)
  */
  bool isInside(long motor, long currentM1, long currentM2);

  // solo valida sw1 y sw2
  void tocoExtremos();

  bool tocoLimiteExterno;

private:
  // Estados previos de los switches (para detectar cambio de estado)
  byte _sw1Ant;
  byte _sw2Ant;
  byte _sw3Ant;

  /*
     Función interna para manejar antibounce y transición del switch.
     Devuelve 0, 1 o 2 según el estado.
  */
  byte checkSwitch(byte switchPin, byte &switchAnt);
};

#endif

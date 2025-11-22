#ifndef CONSTANTS_H
#define CONSTANTS_H

#define _INFO 1 // 0 no 1 si

// MANO / PINCEL
#define NUMPIXELS 3

// Posiciones del servo de la mano/pincel
#define TOPE_CARGAR 150
#define ARRIBA 100
#define MEDIO 70
#define ABAJO 50
#define ATRAS 20
#define ANGULO 30

// SECUENCIAS
#define MOT_HOMBRO 1
#define MOT_CODO 2
#define MAX_SEQ 11
#define MAX_MODOS_TRAZOS
#define MAX_INTENSIDAD 3

// BRAZO
#define motorInterfaceType 1 // 1 define que se usa un driver
#define MOTOR_TIMETOOFF 5000 // Cuando  se desenergizan luego de usarlos
#define AVMAX 1500           // Maxima Velocidad
#define AVMIN 200            // Minima Vleocidad

// posiciones maximas y minimas de lo motores
#define MAX_MOTORH 1200
#define MID_MOTORH 650 // 670
#define MID_MOTORC 650 // 690
#define MIN_MOTORC 0

enum BrazoModoMovimiento
{
  MODO_RANDOM = 0,
  MODO_M1_SOLO = 1,     // Solo motor 1 (hombro)
  MODO_M2_SOLO = 2,     // Solo motor 2 (codo)
  MODO_M1_LUEGO_M2 = 3, // M1 luego M2
  MODO_M2_LUEGO_M1 = 4, // M2 luego M1
  MODO_SIMULTANEO = 5,
  MODO_PINTAR_HORIZONTAL = 6, // M1-->M2
  MODO_PINTAR_VERTICAL = 7,   // M2-->M1
  MODO_CURVAS = 8,            // Curvas M1 y M2 -->reset after use
  MODO_CIRCULO = 9
};

#endif

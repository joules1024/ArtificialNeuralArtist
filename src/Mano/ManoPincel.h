#ifndef ManoPincel_h
#define ManoPincel_h

#include <Arduino.h>
#include <Servo.h>
#include "Leds.h"
#include "../Config/Pinout.h"

// pines usados
// #define COLOR_PIN 62
// #define BUZZER_PIN 27
// #define GOTERO_PIN 27
// #define SERVO_PIN 23
// #define NEOPIXEL_MANO_PIN 45 // A8
#define NUMPIXELS 3 // Popular NeoPixel ring size

#define TOPE_CARGAR 150
#define ARRIBA 100
#define MEDIO 70
#define ABAJO 50
#define ATRAS 20

#define ANGULO 30

class ManoPincel
{
public:
  ManoPincel(void);
  int pStatus();
  int penStat;
  void lapizCargar(int cantidad);
  void lapizInit();
  void lapizUp();
  void lapizDown();
  void lapizPincelada(int vel); // 1 minima velociad
  void lapizPunto(int vel);
  void luzOn(int intensidad, int color);
  void luzOff();
  void buzzer(int tiempo);
  void gotero(int color);

private:
  Servo myservo;
  Servo servoGotero;
  LEDS leds;
};

#endif

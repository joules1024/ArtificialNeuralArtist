#include <Arduino.h>
#include "Sonido.h"

Sonido::Sonido(void) {}

void Sonido::Hablar(String palabra){

  int letra;
  int asciiVal;
    for (unsigned int a = 0; a < palabra.length(); a++) {
      letra = palabra.charAt(a);
      asciiVal = letra * 17;  // MAS AGUDO o GRAVE
      tone(45, asciiVal);                //  PIN SPEAKER 45
      delay (random (50,100));           //  TONADA 
      noTone(45);
      if (letra == ' ') delay(100);
      if (letra == ',') delay(500);
      if (letra == '.') delay(1000);
   }
}

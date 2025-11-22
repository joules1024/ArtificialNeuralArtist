#include <Arduino.h>
#include "Menu.h"
#include "../ArtModule/Dibujar.h"
#include "../Ruedas/Ruedas.h"
#include "../ArtModule/ArtPlan.h"

// Constantes
#define MAX_MODOS_DE_TRAZO 8
#define MAX_NUM_TRAZOS 5
#define MIN_RESIZE 50
#define MAX_RESIZE 100

// Objetos globales (extern declarados en otro archivo)
extern Dibujar dibujo;
extern Ruedas ruedas;
extern ArtPlan artPlan;

// ============================================================================
// UTILIDADES
// ============================================================================
void clearScreen()
{
    Serial.write(27);
    Serial.print(F("[2J"));
    Serial.write(27);
    Serial.print(F("[H"));
}

void menu()
{
    Serial.println(F("MENU"));
    Serial.println(F("----"));
    Serial.println();
    Serial.println(F("Menu/help             Menu"));
    Serial.println(F("Info                  Estado actual"));
    Serial.println(F("Pen Modo              0-up 1-down 2-pincelada 3-punto 4-cargar"));
    Serial.println(F("Home Modo             0-up, 1-down"));
    Serial.println(F("Color H,B             Color led MANO HUE,BRIGHT"));
    Serial.println(F("Text                  Dibuja una Secuencia de Texto"));
    Serial.println(F("Firma                 Firmar"));
    Serial.println(F("Seq N                 Secuencia N cantTrazos"));
    Serial.println(F("Ruedas Mode, D1, D2   Mover ruedas modos 1 2 3 4, dist"));
    Serial.println(F("Mover DX, DY          Mover brazo X Y"));
    Serial.println(F("Motor Mot, Dist       Mover motor de brazo por separado"));
    Serial.println(F("ArtPlan Mode          ArtPlan modos 1 2 3 4"));
    Serial.println(F("p - Lisajous X Y"));

    Serial.println(F("List                  Lista las secuencias en memoria"));
    Serial.println(F("Delete                Borra las secuencias de memoria"));
    Serial.println(F("Write N               Guarda secuencia en memoria slot N (0 a 9)"));
    Serial.println(F("Load N                Carga secuencia desde memoria slot N (0 a 9)"));

    Serial.println();
}

#ifndef ArtPlan_h
#define ArtPlan_h

#include <Arduino.h>
#include "../../src/Ruedas/Ruedas.h"
#include "../../src/Mano/ManoPincel.h"
#include "Dibujar.h"

class ArtPlan : public ManoPincel, public Dibujar
{
public:
    ArtPlan(void);
    void crearPlan(int modo, int circunferencia, int segmentos, int animo); // circunferencia 0 a 100% animo del 0 al 10
    void RunPlan();
    void randomUpDownBrush();
    void randomStroke();

private:
    Ruedas ruedas;
    ManoPincel manoPincel;
    Dibujar dibujo;

    int _recorrido;
    int _segmentos;
    int _silencio;
    int _animo;
    int _aceleracion;
    int _vel;
    int _modo;
};

#endif
#include "ArtPlan.h"

ArtPlan::ArtPlan(void) : ManoPincel(), Dibujar()
{
}

void ArtPlan::randomUpDownBrush()
{
    int rnd = random(0, 2);
    if (rnd == 1)
        dibujo.lapizDown();
    else
        dibujo.lapizUp();
}

void ArtPlan::randomStroke()
{
    int rnd = random(0, 2);
    if (rnd == 1)
        manoPincel.lapizPincelada(_animo);
}

void ArtPlan::crearPlan(int modo, int circunferencia, int segmentos, int animo)
{
    _segmentos = segmentos;
    _recorrido = (9000 * circunferencia / 360) / segmentos;
    _aceleracion = 4000 - ((2000 / 10) * (10 - animo));
    _vel = 1000 + (animo * 300);
    _modo = modo;
    _silencio = 500;
    manoPincel.lapizInit();
    _animo = animo;
};

void ArtPlan::RunPlan()
{
    dibujo.randomPos();                                // reposiciona el brazo
    dibujo.crearDibujo(random(0, 6), random(0, 7), 1); // trazos, modos, repeticion
    int size = random(60, 101);
    dibujo.lapizCargar(1);
    switch (_modo)
    {
    case 1:

        for (int cont = 0; cont < _segmentos; cont++)
        {
            int modoRuedas = random(3, 5); // 3 o 4
            randomUpDownBrush();
            ruedas.MoverRuedas(modoRuedas, _recorrido, _vel, _aceleracion);
            dibujo.ejecutarDibujo(size, 1, 1); // muevo el brazo  (tamano, intensidad,  lapizup)
            randomStroke();
            delay(_silencio);
            dibujo.lapizUp();
        }
        break;

    case 2:

        // muevo para un Lado
        for (int cont = 0; cont < _segmentos; cont++)
        {
            ruedas.MoverRuedas(3, _recorrido, _vel, _aceleracion);
            dibujo.ejecutarDibujo(size, 1, 1);                         // muevo el brazo  (tamano, intensidad,  lapizup)
            ruedas.MoverRuedas(1, random(0, 300), _vel, _aceleracion); // avanzo
            randomStroke();
            delay(_silencio);
            dibujo.lapizUp();
        }
        // muevo para el otro lado
        for (int cont = 0; cont < _segmentos; cont++)
        {
            ruedas.MoverRuedas(4, _recorrido, _vel, _aceleracion);
            randomUpDownBrush();
            dibujo.ejecutarDibujo(size, 1, 1);                         // muevo el brazo  (tamano, intensidad,  lapizup)
            ruedas.MoverRuedas(2, random(0, 300), _vel, _aceleracion); // avanzo
            delay(_silencio);
            dibujo.lapizUp();
        }

        break;

    case 3:
    {
        int modoGiro = random(3, 9); // 3 a 8
        for (int cont = 0; cont < _segmentos; cont++)
        {
            ruedas.MoverRuedas(modoGiro, _recorrido, _vel, _aceleracion);
            randomUpDownBrush();
            dibujo.ejecutarDibujo(size, 1, 1); // muevo el brazo  (tamano, intensidad,  lapizup)
            randomStroke();
            delay(_silencio);
            dibujo.lapizUp();
        }

        break;
    }
    case 4:
    {
        dibujo.lapizDown();
        int modoRuedas = random(3, 5); // 3 o 4
        for (int cont = 0; cont < _segmentos; cont++)
        {
            dibujo.lapizDown();
            ruedas.MoverRuedas(modoRuedas, _recorrido, _vel, _aceleracion);
            delay(_silencio / 2);                            // pausas más suaves
            dibujo.mover(0, random(20, 100), 100, 2, false); // pequeño ajuste de posicion
        }
        dibujo.lapizUp();
        break;
    }
    }
};
#ifndef Dibujar_h
#define Dibujar_h

#include <Arduino.h>
#include "Secuencia.h"
#include "Memories.h"
#include "../Brazo/Brazo.h"
#include "../../src/Mano/ManoPincel.h"
#include "../Config/Constants.h"

class Dibujar : public ManoPincel, public Brazo
{
public:
  Dibujar(void);                                              // constructor
  void loop();                                                // ejecutarlo una vez por ciclo
  void pintar(long x2, long y2, long iteraciones, bool info); // figuras tipo lissajous
  void crearDibujo(int nTrazos, int mode, int cerrada);       // Crea una secuencia random (dibujo)  cerrada 0,1,2
  void dibujarPalabra(String Texto);                          // transfromar y dibuja palabras en ideogramas
  void dibujaTexto(String Texto);                             // sequencia a partir  un texto

  void firmar();
  void ejecutarDibujo(long tamano, int intensidad, int dibuja); // ejecuta secuencia creada por una secuencia
  void randomPos();
  void posActual();
  void parking(int modo);
  void setStatusDibujar(bool status);

  long posM1, posM2;
  bool nuevaSeq = false;
  bool estoyDibujando = false;
  Secuencia secuencia;

private:
  ManoPincel manoPincel;
  bool _parkAfter = false;
};
#endif

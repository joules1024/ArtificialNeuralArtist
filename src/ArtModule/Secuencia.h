#ifndef Secuencia_h
#define Secuencia_h

#include "../Brazo/Limites.h"
#include "../../src/Config/Constants.h"
class Secuencia
{
public:
  Secuencia(void);
  int seqOrder = -1;
  int Modo[MAX_SEQ]; // Modo Dibujo
  int modoMano[MAX_SEQ];
  long taskStatus[MAX_SEQ];    // para usar en ejecutarDibujo
  long M1[MAX_SEQ];            //_incMotHombro
  long M2[MAX_SEQ];            //_incMotCodo
  long valoracionInt[MAX_SEQ]; // Basada en simetria y numero aureo
  long animo[MAX_SEQ];         // estado endorfinico actual, dopamina
  long valoracionExt[MAX_SEQ]; // valoracion del tutor
  long animoExt[MAX_SEQ];      // estado de animo del turor
  long SeqCerrada;             // 1 si, 0 NO
  long largoSeq;               // largo de la sequencia

  void setOrigen(long pIni1, long pIni2);                              // Posicion inicial de la secuencia
  void ArmarSecuencia(long nTrazos, int mode, int cerrada, bool info); // Armo la secuencia  modo 0 = rnd, o fuerza modo de mov 1 2 3 4 5
  bool ArmarSecuenciaTexto(String texto, bool info);
  bool ArmarFirma();
  String proximaPalabra(const String &Texto);

  long _posOrigenHombro;
  long _posOrigenCodo;

private:
  byte _info = 1; // 1 loguea en pantalla, 0 desactivado
  int _modo, _taskIndex;
  long _incMotHombro, _incMotCodo; // incremento Motor1, Motor2
  long _incTotalX, _incTotalY;
  long _randomGen(int motor, long xInicial, long yInicial);
  long _maxHombroEnDireccion(long incM1, long actualM1, long actualM2); // busca el maximo en la direccion de M1
  long _maxCodoEnDireccion(long incM2, long actualM1, long actualM2);   // busca el maximo en la direccion de M2
  void _procesarLetrasXY(String Texto, int orden);
  bool _validarSecuencia(bool info);
  bool _repararSecuencia(int modo);
  int _generarCirculo(long origenHombro, long origenCodo, int indice);

  int _posPalabra1;
  int _posPalabra2;

  Limites limite;
};
#endif

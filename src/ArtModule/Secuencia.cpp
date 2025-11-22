#include <Arduino.h>
#include "Secuencia.h"
#include "../Config/Constants.h"

Secuencia::Secuencia(void) {}

// --------------------------
// Set origen
// --------------------------
void Secuencia::setOrigen(long posHombro, long posCodo)
{
  _posOrigenHombro = posHombro;
  _posOrigenCodo = posCodo;
}

// --------------------------------------------------
// Genera número random dentro de límites permitidos
// --------------------------------------------------
long Secuencia::_randomGen(int motor, long actualM1, long actualM2)
{
  long sup, inf;

  if (motor == MOT_HOMBRO)
  {
    sup = limite.limiteSupM1(actualM2) - actualM1;
    inf = limite.limiteInfM1(actualM2) - actualM1;
  }
  else if (motor == MOT_CODO)
  {
    sup = limite.limiteSupM2(actualM1) - actualM2;
    inf = limite.limiteInfM2(actualM1) - actualM2;
  }
  else
    return 0;

  return random(inf, sup);
}

// --------------------------------------------------
// Distancia máxima en dirección de movimiento M1 y M2
// --------------------------------------------------
long Secuencia::_maxHombroEnDireccion(long inc, long actH, long actC)
{
  long sup = limite.limiteSupM1(actC);
  long inf = limite.limiteInfM1(actC);
  return (inc >= 0) ? (sup - actH) : (actH - inf);
}

long Secuencia::_maxCodoEnDireccion(long inc, long actH, long actC)
{
  long sup = limite.limiteSupM2(actH);
  long inf = limite.limiteInfM2(actH);
  return (inc >= 0) ? (sup - actC) : (actC - inf);
}

// =====================================================
//  Armar Secuencia de Trazos
// =====================================================
// Armo la secuencia  _modo 0 = rnd, o fuerza _modo de mov 1 2 3 4 5
void Secuencia::ArmarSecuencia(long nTrazos, int mode, int cerrada, bool info)
{
  long posH = _posOrigenHombro;
  long posC = _posOrigenCodo;

  Serial.println();
  Serial.println(F("NUEVA FORMA (SEQ)"));
  Serial.println(F("-----------------"));
  Serial.println("\t Trazos:\t" + String(nTrazos));
  Serial.println("\t PosActual:\t" + String(posH) + ", " + String(posC));
  Serial.print(F("\t Seq Cerrada: "));

  if (cerrada == 0)
    Serial.println(F("ABIERTA"));
  else if (cerrada == 1)
    Serial.println(F("CERRADA"));

  for (int i = 0; i < nTrazos; i++)
  {
    long origH = posH;
    long origC = posC;

    // Mano (pincelada)
    modoMano[i] = random(1, 3);

    // Secuencia cerrada?
    SeqCerrada = (cerrada == 1) ? 1 : (cerrada == 2) ? random(0, 2) : 0;

    // Modo
    int modo = (mode == MODO_RANDOM) ? random(1, MAX_MODOS_TRAZOS + 1) : mode;

    Modo[i] = modo;

    // Movimientos según modo
    switch (modo)
    {
      case MODO_M1_SOLO:
        _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC);
        _incMotCodo = 0;
        break;

      case MODO_M2_SOLO:
        _incMotCodo = _randomGen(MOT_CODO, origH, origC);
        _incMotHombro = 0;
        break;

      case MODO_M1_LUEGO_M2:
        _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC);
        _incMotCodo = _randomGen(MOT_CODO, origH + _incMotHombro, origC);
        break;

      case MODO_M2_LUEGO_M1:
        _incMotCodo = _randomGen(MOT_CODO, origH, origC);
        _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC + _incMotCodo);
        break;

      case MODO_SIMULTANEO:

      case MODO_CURVAS:
      {
        bool mueveM1Primero = (limite.limiteSupM1(origC) - limite.limiteInfM1(origC)) >=
                              (limite.limiteSupM2(origH) - limite.limiteInfM2(origH));

        if (mueveM1Primero)
        {
          _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC);
          _incMotCodo = _randomGen(MOT_CODO, origH + _incMotHombro, origC);
        }
        else
        {
          _incMotCodo = _randomGen(MOT_CODO, origH, origC);
          _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC + _incMotCodo);
        }
      }
      break;

      case MODO_PINTAR_HORIZONTAL:
        _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC);
        _incMotCodo = _randomGen(MOT_CODO, origH + _incMotHombro, origC);

        if (abs(_incMotCodo) > _maxCodoEnDireccion(_incMotCodo, origH, origC))
          _incMotCodo = _maxCodoEnDireccion(_incMotCodo, origH, origC);
        break;

      case MODO_PINTAR_VERTICAL:
        _incMotCodo = _randomGen(MOT_CODO, origH, origC);
        _incMotHombro = _randomGen(MOT_HOMBRO, origH, origC + _incMotCodo);

        if (abs(_incMotHombro) > _maxHombroEnDireccion(_incMotHombro, origH, origC))
          _incMotHombro = _maxHombroEnDireccion(_incMotHombro, origH, origC);
        break;

      case MODO_CIRCULO:
      {
        int pasos = _generarCirculo(origH, origC, i);
        i += pasos - 1;
        continue;
      }
    }

    // Guardar
    M1[i] = _incMotHombro;
    M2[i] = _incMotCodo;

    // Actualizar posición
    posH = origH + _incMotHombro;
    posC = origC + _incMotCodo;
  }

  largoSeq = nTrazos;

  // Info
  if (info)
  {
    Serial.println(F("\n\t MATRIZ SECUENCIA"));
    Serial.println(F("\t [IncH\tIncC\tModo\tMano]"));
    for (int i = 0; i < nTrazos; i++)
    {
      Serial.print("\t [");
      Serial.print(M1[i]);
      Serial.print("\t");
      Serial.print(M2[i]);
      Serial.print("\t");
      Serial.print(Modo[i]);
      Serial.print("\t");
      Serial.print(modoMano[i]);
      Serial.println("]");
    }

    Serial.println(SeqCerrada ? F("\t SEQ CERRADA") : F("\t SEQ ABIERTA"));
  }
}

// Arma una secuencia en base a un texto o palabra
bool Secuencia::ArmarSecuenciaTexto(String texto, bool info)
{

  int largo = texto.length();
  int cant = largo / 2; // cantidad de pares de letras
  int a = 0;
  bool aux;
  String parXY;

  if ((largo % 2) == 0)
    SeqCerrada = 1; // Cambia a _cerrada si es par
  else
    SeqCerrada = 0;

  for (int i = 0; i < cant; i++)
  {
    parXY = texto.substring(i * 2, +(i * 2) + 2);
    _procesarLetrasXY(parXY, i);
  }
  if (info)
  {
    Serial.println();
    Serial.print(F("GENERANDO SECUENCIA: "));
    Serial.println(texto);
    Serial.println(F("\t [_incMotHombro  _incMotCodo MODO]"));

    for (int cont = 0; cont < cant; cont++)
    {
      Serial.print("\t [" + String(M1[cont]) + "\t" + String(M2[cont]) + "\t" + String(Modo[cont]) + "]\n");
    }
    if (SeqCerrada == 1)
      Serial.println(F("\t SEQ CERRADA"));
    else
      Serial.println(F("\t SEQ ABIERTA"));
  }
  largoSeq = cant;

  do
  { // reparo la secuencia rotandola si es necesario
    aux = _repararSecuencia(a);
    a++;
  } while (aux == false and a <= 3);
  if (aux == false)
    Serial.println("\t Secuencia invalida");
  largoSeq = cant;
  return aux;
}

bool Secuencia::ArmarFirma()
{
  int _cerrada = 1; // Abierta
  long posOrigenHombro = _posOrigenHombro;
  long posOrigenCodo = _posOrigenCodo;
  long posActualHombro = _posOrigenHombro;
  long posActualCodo = _posOrigenCodo;

  M1[0] = 60;  // inc M1
  M2[0] = 120; // inc M2
  Modo[0] = 5; // _modo
  modoMano[0] = 1;
  M1[1] = 60;   // inc M1
  M2[1] = -120; // inc M2
  Modo[1] = 5;  // _modo
  modoMano[1] = 1;
  largoSeq = 2;

  SeqCerrada = _cerrada; // SeqCerrada o abierta
}

bool Secuencia::_validarSecuencia(bool info) // TRUE si esta OK
{
  long val1 = _posOrigenHombro;
  long val2 = _posOrigenCodo;
  bool result = true;

  for (int i = 0; i < largoSeq; i++)
  {
    val1 = val1 + M1[i];
    val2 = val2 + M2[i];

    if (info)
    {
      Serial.println("\t [" + String(val1) + "\t" + String(val2) + "\t" + String(i) + "]");
    }
    if (limite.isValidMovement(val1, val2, M1[i], M2[i], Modo[i], 0) == false)
    {
      if (info)
        Serial.print(" *");
      result = false;
    }
  }
  if (info)
    Serial.println();
  return result;
}

bool Secuencia::_repararSecuencia(int modo)
{
  if (modo < 0 || modo > 3)
    return false; // modo inválido

  // Copias auxiliares
  long M1aux[MAX_SEQ];
  long M2aux[MAX_SEQ];

  for (int i = 0; i < largoSeq; i++)
  {
    M1aux[i] = M1[i];
    M2aux[i] = M2[i];
  }

  // ----- Aplicar transformación según modo -----
  switch (modo)
  {
    case 1: // Invertir eje HOMBRO
      for (int i = 0; i < largoSeq; i++)
        M1[i] = -M1[i];
      break;

    case 2: // Invertir eje CODO
      for (int i = 0; i < largoSeq; i++)
        M2[i] = -M2[i];
      break;

    case 3: // Rotar 90° intercambiando M1 <-> M2
      for (int i = 0; i < largoSeq; i++)
      {
        long tmp = M1[i];
        M1[i] = M2[i];
        M2[i] = tmp;
      }
      break;

    default: // modo == 0
      return _validarSecuencia(false);
  }

  // ----- Validar la secuencia reparada -----
  bool ok = _validarSecuencia(false);

  // Si falla → restaurar valores
  if (!ok)
  {
    for (int i = 0; i < largoSeq; i++)
    {
      M1[i] = M1aux[i];
      M2[i] = M2aux[i];
    }
  }

  return ok;
}

void Secuencia::_procesarLetrasXY(String Texto, int orden)
{
  int largo = Texto.length();
  if (largo < 2)
    return; // Palabra demasiado corta, no hace nada

  // ---- Cálculo del incremento Hombro (primer carácter) ----
  char c1 = Texto.charAt(0);
  long incH = (long)c1;
  if ((c1 % 2) == 0)
    incH = -incH;

  // ---- Cálculo del incremento Codo (segundo carácter) ----
  char c2 = Texto.charAt(1);
  long incC = (long)c2;
  if ((c2 % 2) == 0)
    incC = -incC;

  // ---- Asignar a vectores de secuencia ----
  M1[orden] = incH;
  M2[orden] = incC;

  // ---- Modo según vocal/consonante del primer carácter ----
  switch (c1)
  {
    case 'a':
    case 'A':
      Modo[orden] = 1;
      break;
    case 'e':
    case 'E':
      Modo[orden] = 2;
      break;
    case 'i':
    case 'I':
      Modo[orden] = 3;
      break;
    case 'o':
    case 'O':
      Modo[orden] = 4;
      break;
    default:
      Modo[orden] = 5;
      break; // cualquier consonante
  }
}

// Parsea un texto y devuelve de a una las palabras,  "" cuadno termina
String Secuencia::proximaPalabra(const String &Texto)
{
  // Si el texto está vacío → no hay nada que devolver
  if (Texto.length() == 0)
    return "";

  // Saltar espacios consecutivos
  while (_posPalabra1 < Texto.length() && Texto[_posPalabra1] == ' ')
    _posPalabra1++;

  // Si nos pasamos del final → terminar
  if (_posPalabra1 >= Texto.length())
    return "";

  // Buscar el próximo espacio
  _posPalabra2 = Texto.indexOf(' ', _posPalabra1);

  String palabra;

  if (_posPalabra2 == -1)
  {
    // Última palabra
    palabra = Texto.substring(_posPalabra1);
    _posPalabra1 = Texto.length(); // para forzar final
  }
  else
  {
    // Intermedia
    palabra = Texto.substring(_posPalabra1, _posPalabra2);
    _posPalabra1 = _posPalabra2 + 1; // avanzar para la próxima
  }

  palabra.trim(); // seguridad extra: sacar espacios laterales

  return palabra;
}

// =====================================================
// 🔵 Genera una secuencia circular
// =====================================================
int Secuencia::_generarCirculo(long origenHombro, long origenCodo, int indice)
{
  float radio = random(30, 80); // Tamaño del círculo
  int pasos = random(6, 12);    // Cantidad de segmentos del círculo
  float anguloInicio = random(0, 360);
  int sentido = random(0, 2) ? 1 : -1; // 1 = horario, -1 = antihorario
  float anguloStep = (360.0 / pasos) * sentido;

  for (int i = 0; i < pasos; i++)
  {
    float angulo = (anguloInicio + i * anguloStep) * DEG_TO_RAD;
    float anguloNext = (anguloInicio + (i + 1) * anguloStep) * DEG_TO_RAD;

    float x1 = radio * cos(angulo);
    float y1 = radio * sin(angulo);
    float x2 = radio * cos(anguloNext);
    float y2 = radio * sin(anguloNext);

    long incHombro = (long)(x2 - x1);
    long incCodo = (long)(y2 - y1);

    // Validación de límites
    if (!limite.isValidMovement(origenHombro + incHombro, origenCodo + incCodo, incHombro, incCodo, MODO_CIRCULO, 0))
    {
      // Si está fuera de rango, reducir amplitud
      incHombro /= 2;
      incCodo /= 2;
    }

    // Guardar los incrementos
    M1[indice + i] = incHombro;
    M2[indice + i] = incCodo;
    Modo[indice + i] = MODO_CIRCULO;
    modoMano[indice + i] = 1;
  }

  SeqCerrada = 1; // Un círculo siempre se considera cerrado
  return pasos;   // Devuelve la cantidad de trazos generados
}

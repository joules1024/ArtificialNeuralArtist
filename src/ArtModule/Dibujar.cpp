#include "Dibujar.h"

//==============================================
// Constructor hereda ManoPincel y Brazo
//==============================================
Dibujar::Dibujar(void) : ManoPincel(), Brazo() {}

//==============================================
// FUNCIONES PUBLICAS
//==============================================
void Dibujar::setStatusDibujar(bool status)
{
  estoyDibujando = status;
}

// Arma una secuencia = forma = dibujo
void Dibujar::crearDibujo(int nTrazos, int mode, int cerrada)
{
  int _cerrada;
  long posOrigen1 = currentPos(1);
  long posOrigen2 = currentPos(2);

  secuencia.setOrigen(posOrigen1, posOrigen2);
  secuencia.ArmarSecuencia(nTrazos, mode, cerrada, true);
  nuevaSeq = true;
}

void Dibujar::pintar(long incX, long incY, long iteraciones, bool info) {}

void Dibujar::dibujaTexto(String Texto)
{
  // Validación básica
  if (Texto.length() == 0)
  {
    Serial.println(F("dibujaTexto: Texto vacío, no se dibuja."));
    return;
  }

  String palabra;
  long posOrigen1;
  long posOrigen2;

  const uint8_t MAX_ITER = 100; // evita loops infinitos
  uint8_t contador = 0;

  // TIMEOUT ADICIONAL: tiempo máximo de ejecución (30 segundos)
  unsigned long tiempoInicio = millis();
  const unsigned long TIMEOUT_MS = 30000;

  do
  {
    // PROTECCIÓN: Contador de iteraciones
    if (++contador > MAX_ITER)
    {
      Serial.println(F("dibujaTexto: Máximo de iteraciones alcanzado. Abortando."));
      break;
    }

    // PROTECCIÓN: Timeout por tiempo
    if (millis() - tiempoInicio > TIMEOUT_MS)
    {
      Serial.println(F("dibujaTexto: Timeout alcanzado. Abortando."));
      break;
    }

    posOrigen1 = currentPos(1);
    posOrigen2 = currentPos(2);

    palabra = secuencia.proximaPalabra(Texto);

    // Limpiar y validar caracteres raros
    palabra.trim();
    if (palabra.length() == 0)
    {
      continue; // si la palabra es solo espacios, saltar
    }

    secuencia.setOrigen(posOrigen1, posOrigen2);

    dibujarPalabra(palabra);
    randomPos();
    delay(200);

  } while (palabra != "");

  Serial.print(F("✅ dibujaTexto completado. Iteraciones: "));
  Serial.println(contador);
}

void Dibujar::firmar()
{
  mover(300, 100, 100, 5, false);
  secuencia.ArmarFirma();
  ejecutarDibujo(100, 1, 1);
  mover(30, 30, 100, 5, false);
  manoPincel.lapizPunto(1);
}

void Dibujar::dibujarPalabra(String Palabra)
{
  if (secuencia.ArmarSecuenciaTexto(Palabra, true))
    ejecutarDibujo(100, 1, 1);
  else
    Serial.println("\t No se ejecuto la secuencia");
}

void Dibujar::ejecutarDibujo(long tamano, int intensidad, int dibuja)
{
  int contInt = 0;
  long mAux;
  long posOrigen1 = currentPos(1);
  long posOrigen2 = currentPos(2);

  Serial.println("\n\t EJECUTANDO SECUENCIA [qty trazos: " + String(intensidad) + ", tamano: " + String(tamano) + "]");

  if (dibuja == 0)
    manoPincel.lapizUp();
  if (dibuja == 1)
    manoPincel.lapizDown();

  Serial.print("\t ");

  // PROTECCIÓN: Detección de loop infinito (sin progreso)
  unsigned long ultimoProgreso = millis();
  const unsigned long TIMEOUT_SIN_PROGRESO = 10000; // 10s sin avanzar = loop infinito
  int contIntAnterior = -1;

  // repeticion x intensidad
  do
  {

    // Solo verificar si NO está progresando
    if (contInt == contIntAnterior)
    {
      // No hay progreso, verificar timeout
      if (millis() - ultimoProgreso > TIMEOUT_SIN_PROGRESO)
      {
        Serial.println(F("\nLoop infinito detectado (sin progreso). Abortando."));
        manoPincel.lapizUp();
        desactivarMotores();
        return;
      }
    }
    else
    {
      // Hay progreso, resetear timeout
      ultimoProgreso = millis();
      contIntAnterior = contInt;
    }

    for (int cont = 0; cont < secuencia.largoSeq; cont++)
    {

      if (secuencia.Modo[cont] == MODO_CURVAS)
        _parkAfter = true;

      if (secuencia.Modo[cont] < 10)
      {
        mover(secuencia.M1[cont], secuencia.M2[cont], tamano, secuencia.Modo[cont], false);
        if (secuencia.modoMano[cont] == 2 && dibuja == 1)
        {
          manoPincel.lapizPincelada(10);
          manoPincel.lapizDown();
        }
      }
      Serial.print(">");
    }

    contInt = contInt + 1;

    // Retorno a origen secuencia cerrada
    if (secuencia.SeqCerrada == 0)
    {
      if (contInt < intensidad)
      {
        for (int cont = secuencia.largoSeq - 1; cont >= 0; cont--)
        {
          // PROTECCIÓN: Verificar índice válido en reversa
          if (cont < 0 || cont >= secuencia.largoSeq)
          {
            Serial.println(F("\n Índice fuera de rango (reversa)"));
            break;
          }

          int mAux = secuencia.Modo[cont];
          if (mAux == 3)
            mAux = 4;
          else if (mAux == 4)
            mAux = 3;

          mover(-secuencia.M1[cont], -secuencia.M2[cont], tamano, mAux, false);
          Serial.print("<");
        }
      }
    }
    else
    {
      long deltaH = posOrigen1 - currentPos(1);
      long deltaC = posOrigen2 - currentPos(2);

      // PROTECCIÓN 8: Verificar deltas razonables
      const long MAX_DELTA = 10000;
      if (abs(deltaH) > MAX_DELTA || abs(deltaC) > MAX_DELTA)
      {
        Serial.println(F("\nDelta de retorno excesivo, posible error de posición"));
        Serial.print(F("DeltaH: "));
        Serial.print(deltaH);
        Serial.print(F(" DeltaC: "));
        Serial.println(deltaC);
      }
      else
      {
        mover(deltaH, deltaC, 100, 5, false);
      }
    }

    Serial.print(contInt);
    Serial.print(" ");

  } while (contInt < intensidad);

  manoPincel.lapizUp();

  if (_parkAfter == true and getTocoLimiteExterno() == true)
  {
    goHome();
    manoPincel.lapizCargar(1);
    _parkAfter = false;
  }

  manoPincel.lapizUp();
  desactivarMotores();
  Serial.println();
  Serial.println();
  Serial.println("\t CURRENT POS: " + String(currentPos(1)) + " " + String(currentPos(2)));
  Serial.println(F("\t FIN SECUENCIA"));
  Serial.println("");
  Serial.println(F("--------------------------------------------------"));
  Serial.println("");
}

void Dibujar::randomPos()
{
  crearDibujo(1, 3, 0);
  ejecutarDibujo(100, 1, 0);
}

void Dibujar::posActual()
{
  posM1 = currentPos(1);
  posM2 = currentPos(2);
}

void Dibujar::parking(int modo)
{
  if (modo == ARRIBA)
    manoPincel.lapizUp();
  else if (modo == ABAJO)
  {
    manoPincel.lapizCargar(1);
    manoPincel.lapizDown();
  }

  goHome();
  manoPincel.lapizCargar(1);
}

void Dibujar::loop()
{
  run();
}
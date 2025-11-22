#include <Arduino.h>
#include <TimerOne.h>
#include <Wire.h>
#include "src/Sonido/Sonido.h"
#include "src/ArtModule/Dibujar.h"
#include "src/ArtModule/ArtPlan.h"
#include "src/Ruedas/Ruedas.h"
#include "src/Config/Constants.h"
#include "src/Menu/Menu.h"
#include "src/Utils/UtilsCommands.h"

#define MAX_MODOS_DE_TRAZO 8
#define MAX_NUM_TRAZOS 5
#define MIN_RESIZE 50
#define MAX_RESIZE 100
#define MAX_I2C_MSG 3
#define SERIAL_TIMEOUT 250

bool I2CdataReady;
volatile unsigned long ISRCount = 1; // use volatile for shared variables
unsigned long KeepAliveCount = 0;

String posM12Temp, posM12; // string con la info de pos M1 y M2
String distSensorTemp, distSensor;
int id = 1; // para la mensajeria con ESP

String dataI2C; // Para los mensajes MEGA a ESP
String I2Cdata; // enviado por ESP recibido por MEGA

// Objetos
Ruedas ruedas;
Dibujar dibujo;
Sonido sonido;
ArtPlan artPlan;

// ============================================================================
// COMUNICACIÓN I2C
// ============================================================================

void I2CreceiveData(int howMany) // ISR MEGA recibe data del ESP (que viene por MQTT)
{
  I2Cdata = "";
  while (Wire.available() > 0)
  {
    char c = Wire.read();
    I2Cdata = I2Cdata + c; // armo el comadno byte a byte
  }
  // para ser procesado
  if (I2Cdata.length() > 0)
  {
    I2CdataReady = true;
  }
}

void MEGAsendDataI2C()
{
  // Se envian una serie de parametros de estado delimitados por n@ PAYLOAD @
  String statusDibujo = dibujo.estoyDibujando ? "YES" : "NO";

  switch (id)
  {
    case 1:
      KeepAliveCount++;
      dataI2C = String(id) + "@" + String(KeepAliveCount) + "@0"; // 4@ informacion de modo    @0 no hay mas
      break;

    case 2:
      dataI2C = String(id) + "@" + statusDibujo + "@1"; // 1@ informacion de posiciones M1 y M2   @1 hay mas
      break;

    case 3:
      dataI2C = String(id) + "@" + distSensor + "@1"; // 2@ informacion de distancia a la pared @1 hay mas
      break;
  }
  char buffer[32];
  dataI2C.toCharArray(buffer, 32);
  Wire.write(buffer);

  id++;
  if (id > (MAX_I2C_MSG))
    id = 1;
}

// ============================================================================
// INTERRUPCIÓN TIMER
// ============================================================================

void getDataISR(void) // Capturo datos de los motores, secuencia, etc..
{
  ISRCount++;
  dibujo.posActual();
  posM12Temp += String(dibujo.posM1) + "," + String(dibujo.posM2) + "|";

  if (ISRCount > 3)
  {
    noInterrupts();
    posM12 = posM12Temp;
    interrupts();
    posM12Temp = "";
    ISRCount = 1;
  }
}

// ============================================================================
// LECTURA DE COMANDOS  SERIAL & I2C -  NO BLOCK
// ============================================================================

String leerComandoSERI2C()
{
  String comandoSERI2C = "";

  if (Serial.available())
  {
    comandoSERI2C = Serial.readString();
    comandoSERI2C.trim();
  }
  if (I2CdataReady)
  {
    noInterrupts();
    comandoSERI2C = I2Cdata;
    I2CdataReady = false;
    interrupts();
  }
  return comandoSERI2C;
}

void procesarComando(String CommandStr)
{
  ParsedCommand cmd = UtilsCommands::parse(CommandStr);
  String err;

  if (!UtilsCommands::validate(cmd, err))
  {
    Serial.print("ERR: ");
    Serial.println(err);
    return;
  }

  // Alias local
  String C = cmd.command;

  // Helpers para leer parámetros numéricos
  auto P = [&](int i, int def = 0) -> long
  {
    if (i >= cmd.paramCount)
      return (long)def;
    return cmd.params[i].toInt();
  };

  // ======================================================
  // PROCESAR COMANDOS
  // ======================================================

  // ======================================================
  // Parking
  if (C == "H" or C == "HOME")
  {
    if (P(0) == 0)
      dibujo.parking(ARRIBA);
    else
      dibujo.parking(ABAJO);
    return;
  }

  // ======================================================
  // Info status
  if (C == "I" or C == "INFO")
  {
    dibujo.posActual();
    clearScreen();
    Serial.println("General Status");
    Serial.println("--------------");
    Serial.print("Pen Status: ");
    Serial.println(dibujo.pStatus());
    Serial.print("Posicion: ");
    Serial.print(dibujo.posM1);
    Serial.print(", ");
    Serial.print(dibujo.posM2);
    Serial.println();
    Serial.println("Presione Enter para continuar");

    while (!Serial.available())
    {
    }
    menu();
    return;
  }

  // ======================================================
  // Menu
  if (C == "MENU" or C == "HELP")
  {
    menu();
    return;
  }

  // ======================================================
  // Lapiz
  if (C == "PEN")
  {
    int m = P(0);
    if (m == 0)
      dibujo.lapizUp();
    if (m == 1)
      dibujo.lapizDown();
    if (m == 2)
      dibujo.lapizPincelada(3);
    if (m == 3)
      dibujo.lapizPunto(5);
    if (m == 4)
      dibujo.lapizCargar(1);
    return;
  }

  // ======================================================
  // color RGB LED Mano
  if (C == "COLOR" or C == "C")
  {
    int h = P(0);
    int b = P(1);
    int s = P(2);
    dibujo.luzOn(b, h);
    return;
  }

  // ======================================================
  // mover brazo
  if (C == "MOVER" or C == "MB")
  {
    dibujo.mover(P(0), P(1), 100, 5, true);
    return;
  }

  // ======================================================
  // mover motores del brazo por separado
  if (C == "MOTOR")
  {
    int motor = P(0);
    int val = P(1);

    if (motor == 1)
      dibujo.mover(val, 0, 100, 1, true);
    if (motor == 2)
      dibujo.mover(0, val, 100, 2, true);
    dibujo.printStatus();
    return;
  }

  // ======================================================
  // secuencia N
  if (C == "S" or C == "SEQ")
  {
    int Tiempo = P(0);
    int t = 0;
    while (t < Tiempo)
    {
      dibujo.setStatusDibujar(true);
      Serial.println("NUEVA SECUENCIA");

      dibujo.luzOff();
      dibujo.randomPos();
      delay(500);
      dibujo.luzOn(150, random(0, 361));
      dibujo.crearDibujo(random(1, MAX_NUM_TRAZOS + 1), random(1, MAX_MODOS_DE_TRAZO + 1), 2);
      dibujo.ejecutarDibujo(random(MIN_RESIZE, MAX_RESIZE), 1, 1);
      delay(500);
      t++;
    }
    dibujo.luzOff();
    dibujo.setStatusDibujar(false);
    return;
  }

  // ======================================================
  // Dibuja secuencia de Texto
  if (C == "TEXT")
  {
    if (cmd.paramCount == 0)
    {
      Serial.println("ERR: T requiere texto → T PALABRA");
      return;
    }

    String texto = cmd.params[0];
    texto.trim();

    dibujo.luzOff();
    dibujo.randomPos();
    delay(500);

    Serial.print("DIBUJANDO TEXTO: ");
    Serial.println(texto);

    dibujo.luzOn(100, random(0, 361));
    dibujo.dibujaTexto(texto);
    delay(500);
    return;
  }

  // ======================================================
  // Firma
  if (C == "F" or C == "FIRMA")
  {
    dibujo.firmar();
    return;
  }

  // ======================================================
  // Art Plan
  if (C == "AP" or C == "ARTPLAN")
  {
    int _modo = P(0);
    int _circ = P(1);
    int _seg = P(2);
    int _animo = P(3);

    dibujo.setStatusDibujar(true);
    Serial.print(F("ART PLAN MODO: "));
    Serial.println(_modo);
    artPlan.crearPlan(_modo, _circ, _seg, _animo);
    artPlan.RunPlan();
    dibujo.setStatusDibujar(false);
    return;
  }

  // ======================================================
  // mover ruedas
  if (C == "R" or C == "RUEDAS" or C == "MR")
  {
    ruedas.MoverRuedas(P(0), P(1), P(2), P(2));
    return;
  }

  // ======================================================
  // guardar memoria
  if (C == "WRITE")
  {
    int slot = P(0);
    if (memories_saveSeq(slot, dibujo))
      Serial.println("OK guardado");
    else
      Serial.println("ERR guardando");
    return;
  }

  // ======================================================
  // cargar memoria
  if (C == "LOAD")
  {
    int slot = P(0);
    if (memories_loadSeq(slot, dibujo))
    {
      Serial.println("OK cargado");
      dibujo.ejecutarDibujo(100, 1, 1);
    }
    else
      Serial.println("ERR slot");
    return;
  }

  // ======================================================
  // listar mmeoria
  if (C == "LIST")
  {
    memories_list();
    return;
  }

  // ======================================================
  // borrar memorias
  if (C == "DELETE")
  {
    memories_clearAll();
    return;
  }

  //---------------------------------------------------------
  Serial.print("Comando no reconocido: ");
  Serial.println(C);
}

void leerProcesarCmd() // Loop para esperar comandos y procesarlos - NO BLOCK
{
  String aux;
  aux = leerComandoSERI2C();
  if (aux != "")
  {
    Serial.println(aux);
    procesarComando(aux);
    Serial.print("cmd: ");
  }
}

// ============================================================================
// SETUP
// ============================================================================

void setup()
{
  randomSeed(analogRead(0)); // Altero la semilla del Random

  Timer1.initialize(333333);          // inicializo interrupcion 1 seg = 1000000  (1/3 de seg)
  Timer1.attachInterrupt(getDataISR); // rutina para obtener info de estado periodicamente

  Serial.setTimeout(SERIAL_TIMEOUT);
  Serial.begin(115200);

  Wire.begin(8);                   // I2c Address 8 (MEGA SLAVE)
  Wire.onRequest(MEGAsendDataI2C); // callback i2c para enviar Info al Controller cuando lo solicite
  Wire.onReceive(I2CreceiveData);  // callback i2C para recepcion de info desde el Head Controller

  delay(100);
  clearScreen();
  Serial.println("*** A.N.A ***");

  delay(200);
  dibujo.parking(ARRIBA); // Estacion abrazo lapizup
  clearScreen();

  menu();

  digitalWrite(ENABLE_MOT, HIGH); // Desactivo motores
}

// ============================================================================
// LOOP
// ============================================================================

void loop()
{
  leerProcesarCmd();
  ruedas.loop();
  dibujo.loop();
  delay(1);
}

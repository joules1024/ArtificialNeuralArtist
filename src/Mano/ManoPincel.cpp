#include "ManoPincel.h"

ManoPincel::ManoPincel(void) : leds(NEOPIXEL_MANO_PIN, NUMPIXELS)
{
  penStat = 1;
}

void ManoPincel::luzOn(int intensidad, int color)
{
  uint16_t auxColor = (uint16_t)(color * 182.04f);
  Serial.println("");
  Serial.print("Hand Light ON: ");
  Serial.println(auxColor);
  leds.clear();
  for (uint16_t cont = 0; cont <= auxColor; cont += 30)
  {
    leds.setPixelColor(1, leds.pixels.ColorHSV(cont, 255, intensidad));
    leds.show();
    delay(1);
  }
}

void ManoPincel::luzOff()
{
  Serial.println("Hand Lights OFF");
  leds.clear();
  leds.setPixelColor(1, leds.pixels.ColorHSV(0, 0, 0));
  leds.show();
}

void ManoPincel::lapizInit()
{
  Serial.println("");
  Serial.print("Initializing hand...");

  myservo.attach(SERVO_PIN);
  myservo.write(ARRIBA);
  delay(500);
  myservo.detach();
  Serial.println("OK");
  penStat = 0;
  int color = random(0, 361);
  luzOn(100, color);
}

void ManoPincel::lapizPincelada(int vel) // 1 minima velociad
{
  if (penStat == 0) // esto garantiza que esta ARRIBA
  {
    myservo.attach(SERVO_PIN);

    int pasos = abs(ARRIBA - ATRAS) / vel;

    // Mueve el servo hacia ATRAS
    for (int i = 0; i < pasos; i++)
    {
      myservo.write(ARRIBA - (i * vel));
      delay(30); // Tiempo de espera entre cada paso
    }
    delay(500); // Espera adicional después de llegar a ATRAS

    // Mueve el servo hacia ARRIBA
    for (int i = 0; i < pasos; i++)
    {
      myservo.write(ATRAS + (i * vel));
      delay(30); // Tiempo de espera entre cada paso
    }
    delay(500); // Espera adicional después de llegar a ARRIBA

    myservo.write(ARRIBA);
    myservo.detach();
    penStat = 0;
  }
  delay(500); // Espera adicional al final de la función
}

void ManoPincel::lapizPunto(int vel)
{
  if (penStat == 0) // esto garantiza que esta ARRIBA
  {
    myservo.attach(SERVO_PIN);
    int pasos = abs(ARRIBA - ABAJO) / vel;
    // Mueve el servo hacia ATRAS
    for (int i = 0; i < pasos; i++)
    {
      myservo.write(ARRIBA - (i * vel));
      delay(30); // Tiempo de espera entre cada paso
    }
    delay(1000); // Espera adicional después de llegar a ATRAS

    // Mueve el servo hacia ARRIBA
    for (int i = 0; i < pasos; i++)
    {
      myservo.write(ABAJO + (i * vel));
      delay(30); // Tiempo de espera entre cada paso
    }
    delay(500); // Espera adicional después de llegar a ARRIBA

    myservo.write(ARRIBA);
    myservo.detach();
    penStat = 0;
  }
  delay(500); // Espera adicional al final de la función
}

void ManoPincel::lapizDown()
{

  myservo.attach(SERVO_PIN);
  myservo.write(ABAJO);
  delay(500);
  myservo.detach();
  penStat = 1;
}

void ManoPincel::lapizUp()
{
  myservo.attach(SERVO_PIN);
  myservo.write(ARRIBA);
  delay(500);
  myservo.detach();
  penStat = 0;
}

void ManoPincel::lapizCargar(int cantidad)
{
  myservo.attach(SERVO_PIN);
  for (int i = 0; i < cantidad; i++)
  {                             // Repite tres veces
    myservo.write(TOPE_CARGAR); // Mueve el servo a la posición TOPE_CARGAR
    delay(2500);                // Espera 2.5 segundos
    myservo.write(ARRIBA);      // Mueve el servo a la posición ABAJO
    delay(1000);                // Espera 0.5 segundos
  }

  myservo.detach();
  penStat = 0;
}

int ManoPincel::pStatus()
{
  return penStat;
}

#include "led.h"
#include "global.h"
void blink_led(int times)
{
  pinMode(pVext, OUTPUT);
  for (int i = 0; i < times; i++)
  {
    digitalWrite(pLed, HIGH);
    vTaskDelay(125);
    digitalWrite(pLed, LOW);
    vTaskDelay(50);
  }
}
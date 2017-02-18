#include "Blinker.h"


Blinker::Blinker(uint8_t pin, uint8_t ms)
{
  _pin = pin;
  _ms = ms;
  pinMode(_pin, OUTPUT);
}

void Blinker::beep(void)
{
  digitalWrite(_pin, HIGH);
  delay(_ms);
  digitalWrite(_pin, LOW);
}

void Blinker::flash(int flashes)
{
  for (int i = 0; i < flashes; i++) {
    Blinker::beep();
    delay(_ms);
  }
}

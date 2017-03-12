#ifndef Blinker_h
#define Blinker_h
#include <Arduino.h>

class Blinker
{
  public:
    Blinker(uint8_t pin, uint8_t ms);
    void beep(void);
    void flash(int flashes);
    void longFlash(int flashes);

  private:
    int _pin;
    int _ms;
};
#endif

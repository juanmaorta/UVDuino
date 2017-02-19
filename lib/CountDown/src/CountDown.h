#ifndef CountDown_h
#define CountDown_h
#include <Arduino.h>

class CountDown
{
  public:
    CountDown();
    int countTo(int seconds);
    bool isTimeOn(volatile int seconds, volatile int &limit);

  private:
    int _limit;
};
#endif

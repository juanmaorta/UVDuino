#include "CountDown.h"

CountDown::CountDown() {}

int CountDown::countTo(int limit) {
  if (limit > 0) {
    delay(1000);
    return limit - 1;
  }

  return 0;
}

bool CountDown::isTimeOn(volatile int seconds, volatile int &limit)
{
  return false;
}

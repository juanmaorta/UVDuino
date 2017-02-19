#include "State.h"

State::State()
{
  _state = IDLE;
}

int State::get(void)
{
  return _state;
}

void State::update(int state)
{
  _state = state;
}

void State::togglePrint()
{
  _state = (_state == IDLE) ? PRINTING : IDLE;
}

bool State::isPrinting()
{
  return _state == PRINTING;
}

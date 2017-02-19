#define IDLE 0
#define PRINTING 1

#ifndef State_h
#define State_h
class State
{
  public:
    State();
    int get();
    void update(int state);
    void togglePrint();
    bool isPrinting();
  private:
    int _state;
};
#endif

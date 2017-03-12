#define IDLE 0
#define PRINTING 1
#define SETUP_DIMMER 3

#ifndef State_h
#define State_h
class State
{
  public:
    State();
    int get();
    void update(int state);
    void togglePrint();
    void toggleDimmer();
    bool isPrinting();
    bool isDimmerSetup();
  private:
    int _state;
};
#endif

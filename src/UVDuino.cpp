#include <ClickEncoder.h>
#include <Button.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>
#include <Blinker.h>
#include <CountUpDownTimer.h>
#include <EEPROM.h>
// states
#include <State.h>

#define PIN_DISPLAY_DIO   3
#define PIN_DISPLAY_CLK   4
#define ENCODER_PINA      5
#define ENCODER_PINB      11
#define PRINT_BTN         8
#define DIMMER_PIN        10 // PWM
#define ENCODER_BTN       6
#define BUZZER_PIN        12
#define RELAY_PIN         9
#define TIME_INCREASE     0.5 // mins

#define ENCODER_STEPS_PER_NOTCH    1   // Change this depending on which encoder is used

// encoder buffer
#define TIME_BUFFER 150 // milliseconds
// print button params
#define PULLUP true
#define INVERT true
#define DEBOUNCE_MS 50
#define BEEPER_FLASHES 2
#define HOLD_RELEASE_DELAY 1000
#define LEVEL_MEM_POSITION 0
#define TIME_MEM_POSITION 1

// initialize TM1637 Display objects
SevenSegmentFun display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
CountUpDownTimer T(DOWN);

Button PrintBtn(PRINT_BTN, PULLUP, INVERT, DEBOUNCE_MS);
Button EncoderBtn(ENCODER_BTN, PULLUP, INVERT, DEBOUNCE_MS);
ClickEncoder encoder = ClickEncoder(ENCODER_PINA,ENCODER_PINB,ENCODER_BTN,ENCODER_STEPS_PER_NOTCH);
Blinker beeper = Blinker(BUZZER_PIN, 100);
State state;

int lightLevel = 4;
volatile float lastTime = 0;
volatile int lastRead = 0;
boolean useMinutes = true;

int ExtractSeconds(float Value) {
  int IntegerPart = (int)(Value);
  int Seconds = (Value - IntegerPart) * 60;

  return Seconds;
}

void print() {
  if (state.isDimmerSetup()) return;
  if (lastTime == 0) return;

  /** @TODO Keep this here until real proof is done */
  // int dimmerLevel = 240 / (5 - lightLevel);
  digitalWrite(RELAY_PIN, LOW);

  if (state.isPrinting()) {
    /** @TODO Keep this here until real proof is done */
    // analogWrite(DIMMER_PIN, dimmerLevel);
    T.Timer();
    if (!T.TimeCheck()) {
      if (T.TimeHasChanged()) {
        if (useMinutes) {
          if (T.ShowMinutes() == 0 && T.ShowSeconds() < 10 ) {
            beeper.bleep();
          }
          display.printTime(T.ShowMinutes(), T.ShowSeconds(), false);
        } else {
          display.printTime(T.ShowSeconds(), 0, false);
        }
      }

      return;
    } else {
      display.printTime(0, 0, false);
      state.togglePrint();
      beeper.longFlash(1);
      // delay(500);
      int secs = (useMinutes) ? abs(ExtractSeconds(lastTime)) : 0;
      display.printTime(abs(lastTime), secs, false);
    }
  }

  digitalWrite(RELAY_PIN, HIGH);
}

void displayTime(int time, int lastValue) {
  if (state.isPrinting()) return;

  int read = millis();
  if (read - lastRead > TIME_BUFFER) {
    if (time > lastValue) {
      lastTime += (useMinutes) ? TIME_INCREASE : 1;
    } else {
      lastTime -= (useMinutes) ? TIME_INCREASE : 1;
    }

    lastRead = read;
  }
  EEPROM.put(TIME_MEM_POSITION, lastTime);
  display.printTime(abs(lastTime), abs(ExtractSeconds(lastTime)), false);
}

void resetTime() {
  lastTime = 0;
  lastRead = 0;
  beeper.beep();
  display.printTime(0, 0, true);
  display.blink();
}

void setup()
{
  // read values from EEPROM
  lightLevel = EEPROM.read(LEVEL_MEM_POSITION);
  if (lightLevel == 255) {
    lightLevel = 4;
  }

  EEPROM.get(TIME_MEM_POSITION, lastTime);

  // initialize RELAY_PIN digital pin as an output.
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);

  display.begin();            // initializes the display
  display.setBacklight(50);  // set the brightness to 100 %
  display.print("HOLA");      // display INIT on the display
  delay(1000);                // wait 1000 ms
  beeper.flash(BEEPER_FLASHES);
  display.clear();

  display.printTime(abs(lastTime), abs(ExtractSeconds(lastTime)), true);



  // encoder.setButtonHeldEnabled(true);
  // encoder.setDoubleClickEnabled(false);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

// updates LED level through PWM
void updateLevel() {
  if (state.isPrinting()) return;

  if (!state.isDimmerSetup()) {
    state.toggleDimmer();
  }

  if (lightLevel < 4) {
    lightLevel++;
  } else {
    lightLevel = 1;
  }

  EEPROM.write(LEVEL_MEM_POSITION, lightLevel);

  display.printLevelVertical(lightLevel * 25);

  int dimmerLevel = 240 / (5 - lightLevel);
  analogWrite(DIMMER_PIN, dimmerLevel);
}

void showCurrentTime() {
  int secs = (useMinutes) ? abs(ExtractSeconds(lastTime)) : 0;
  display.printTime(abs(lastTime), secs, false);
  // delay(1000);
  beeper.beep();
}

void loop() {

  //Call Service in loop becasue using timer interrupts may affect ESP8266 WIFI
  //however call no more than 1 time per millisecond to reduce encoder bounce
  static uint32_t lastService = 0;
  if (lastService + 1000 < micros()) {
    lastService = micros();
    encoder.service();
  }


  static int16_t last, value;
  value += encoder.getValue();

  if (value != last) {
    displayTime(value, last);
    last = value;
  }

  // ClickEncoder::Button b = encoder.getButton();
  // if (b == ClickEncoder::Released) {
  //   resetTime();
  // }

  EncoderBtn.read();
  if (EncoderBtn.wasPressed()) {
    // set status level
    updateLevel();
  }

  if (EncoderBtn.releasedFor(HOLD_RELEASE_DELAY) && state.isDimmerSetup()) {
    showCurrentTime();
    state.toggleDimmer();
  }

  if (EncoderBtn.pressedFor(HOLD_RELEASE_DELAY)) {
    resetTime();
  }

  PrintBtn.read();

  if (PrintBtn.wasPressed()) {
    if (lastTime == 0) {
      beeper.beep();
      display.blink();
      return;
    }
    T.SetTimer(useMinutes ? lastTime * 60 : lastTime); // seconds
    T.StartTimer();
    state.togglePrint();
  }

  print();
  /*
  if (b != ClickEncoder::Open) {
    Serial.print("Button: ");
    #define VERBOSECASE(label) case label: Serial.println(#label); break;
    switch (b) {
      VERBOSECASE(ClickEncoder::Pressed);
      VERBOSECASE(ClickEncoder::Held)
      VERBOSECASE(ClickEncoder::Released)
      VERBOSECASE(ClickEncoder::Clicked)
      VERBOSECASE(ClickEncoder::DoubleClicked)
    }
  }
  */
}

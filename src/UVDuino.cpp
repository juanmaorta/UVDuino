#include <ClickEncoder.h>
#include <Button.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>
#include <SevenSegmentFun.h>
#include <Blinker.h>
// states
#include <State.h>

#define PIN_DISPLAY_DIO   3
#define PIN_DISPLAY_CLK   4
#define ENCODER_PINA      5
#define ENCODER_PINB      6
#define PRINT_BTN         8
#define DIMMER_PIN        10 // PWM
#define ENCODER_BTN       11
#define BUZZER_PIN        12
#define RELAY_PIN         13

#define ENCODER_STEPS_PER_NOTCH    1   // Change this depending on which encoder is used

// encoder buffer
#define TIME_BUFFER 150 // milliseconds
// print button params
#define PULLUP true
#define INVERT true
#define DEBOUNCE_MS 50
#define BEEPER_FLASHES 2

// initialize TM1637 Display objects
SevenSegmentFun display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);

Button PrintBtn(PRINT_BTN, PULLUP, INVERT, DEBOUNCE_MS);
Button EncoderBtn(ENCODER_BTN, PULLUP, INVERT, DEBOUNCE_MS);
ClickEncoder encoder = ClickEncoder(ENCODER_PINA,ENCODER_PINB,ENCODER_BTN,ENCODER_STEPS_PER_NOTCH);
Blinker beeper = Blinker(BUZZER_PIN, 100);
State state;

int lightLevel = 4;
volatile int lastTime = 0;
volatile int lastRead = 0;

void print() {
  int dimmerLevel = 240 / (5 - lightLevel);

  if (state.isPrinting()) {
    analogWrite(DIMMER_PIN, dimmerLevel);
    digitalWrite(RELAY_PIN, LOW);
    return;
  }

  digitalWrite(RELAY_PIN, HIGH);
}

void displayTime(int time, int lastValue) {
  int read = millis();
  if (read - lastRead > TIME_BUFFER) {
    if (time > lastValue) {
      lastTime++;
    } else {
      lastTime--;
    }

    lastRead = read;
  }
  display.printTime(abs(lastTime), 0, false);
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
  // initialize LED digital pin as an output.
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  // pinMode(BUZZER_PIN, OUTPUT);

  display.begin();            // initializes the display
  display.setBacklight(50);  // set the brightness to 100 %
  display.print("HOLA");      // display INIT on the display
  delay(1000);                // wait 1000 ms
  beeper.flash(BEEPER_FLASHES);
  display.clear();

  // encoder.setButtonHeldEnabled(true);
  // encoder.setDoubleClickEnabled(false);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

void updateLevel () {
  if (lightLevel < 4) {
    lightLevel++;
  } else {
    lightLevel = 1;
  }

  display.printLevelVertical(lightLevel * 25);
  delay(500);

  // set status idle
  Serial.println("lightLevel is now");
  Serial.println(lightLevel);
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
  if (EncoderBtn.wasReleased()) {
    // set status level
    updateLevel();
  }

  if (EncoderBtn.pressedFor(1000)) {
    resetTime();
  }

  PrintBtn.read();

  if (PrintBtn.wasPressed()) {
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

/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <ClickEncoder.h>
#include <Button.h>
#include <SevenSegmentTM1637.h>
#include <SevenSegmentExtended.h>

#define LED_PIN 13
#define BUZZER_PIN 12

#define ENCODER_PINA     5
#define ENCODER_PINB     6
#define ENCODER_BTN      11
#define ENCODER_STEPS_PER_NOTCH    1   // Change this depending on which encoder is used

// define clock and digital input pins for display
#define PIN_DISPLAY_CLK   4
#define PIN_DISPLAY_DIO   3

#define TIME_BUFFER 150 // milliseconds
// print button params
#define PRINT_BTN 8
#define PULLUP true
#define INVERT true
#define DEBOUNCE_MS 50

// initialize TM1637 Display objects
SevenSegmentExtended display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);
Button PrintBtn(PRINT_BTN, PULLUP, INVERT, DEBOUNCE_MS);
ClickEncoder encoder = ClickEncoder(ENCODER_PINA,ENCODER_PINB,ENCODER_BTN,ENCODER_STEPS_PER_NOTCH);

// @TODO State
bool printing = false;

void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  display.begin();            // initializes the display
  display.setBacklight(50);  // set the brightness to 100 %
  display.print("HOLA");      // display INIT on the display
  delay(1000);                // wait 1000 ms
  display.clear();

  encoder.setButtonHeldEnabled(true);
  encoder.setDoubleClickEnabled(false);

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(10);
  digitalWrite(BUZZER_PIN, LOW);
  display.blink();
}

void togglePrint() {
  printing = !printing;
}

void print() {
  if (printing) {
    digitalWrite(LED_PIN, HIGH);
    return;
  }

  digitalWrite(LED_PIN, LOW);
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(10);
  digitalWrite(LED_PIN, LOW);
}

volatile int lastTime = 0;
volatile int lastRead = 0;

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
  display.printTime(0, 0, true);
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

    Serial.print("Encoder Value: ");
    Serial.println(value);
    displayTime(value, last);
    last = value;

  }

  ClickEncoder::Button b = encoder.getButton();
  if (b == ClickEncoder::Released) {
    beep();
    resetTime();
  }

  PrintBtn.read();

  if (PrintBtn.wasPressed()) {
    // change printing state
    beep();
  }

  // print();
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

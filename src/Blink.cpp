/*
 * Blink
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include <Encoder.h>
#include <Button.h>
#include <SevenSegmentTM1637.h>
#include "SevenSegmentExtended.h"

#define LED_PIN 13
#define BUZZER_PIN 12
#define BUTTON_MODE_PIN 11
#define ENCODER_CLK 5
#define ENCODER_DT 6
// define clock and digital input pins
#define PIN_DISPLAY_CLK   4
#define PIN_DISPLAY_DIO   3

// initialize TM1637 Display objects
SevenSegmentExtended display(PIN_DISPLAY_CLK, PIN_DISPLAY_DIO);

Button button1(BUTTON_MODE_PIN); // Connect your button between pin 2 and GND
Encoder myEnc(ENCODER_CLK, ENCODER_DT);

bool buzz = false;
bool printing = false;
long oldPosition  = -999;


void setup()
{
  // initialize LED digital pin as an output.
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  button1.begin();

  display.begin();            // initializes the display
  display.setBacklight(50);  // set the brightness to 100 %
  display.print("helo");      // display INIT on the display
  delay(1000);                // wait 1000 ms
  display.clear();

  Serial.begin(9600);
  Serial.println("Basic Encoder Test:");
}

void toggleBuzzer() {
  buzz = !buzz;
}

void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(10);
  digitalWrite(BUZZER_PIN, LOW);
  display.blink();
}

void togglePrint() {
  beep();
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

void loop()
{
  // digitalWrite(LED_PIN, HIGH);
  // // wait for a second
  // delay(50);
  // // turn the LED off by making the voltage LOW
  // // digitalWrite(6, LOW);
  // digitalWrite(LED_PIN, LOW);
  //  // wait for a second
  // delay(500);

  long newPosition = abs(myEnc.read());
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
    // display.print(newPosition);
    display.printTime(newPosition, 0, true);
    blink();
  }

  if (button1.pressed()) {
    togglePrint();
	}

  print();
}

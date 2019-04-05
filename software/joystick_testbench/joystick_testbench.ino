#include "stdio.h"

const int roll_pin = A9;
const int pitch_pin = A8;
const int yaw_pin = A7;
const int button_pin = A6;
const int LED_pin = 13;

char str[64];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLDOWN);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_pin, HIGH);
  sprintf(str, "%d,%d,%d,%d\r\n", analogRead(roll_pin), analogRead(pitch_pin), analogRead(yaw_pin), 1000*digitalRead(button_pin));
  Serial.write(str);
  digitalWrite(LED_pin, LOW);
  delay(20);
}

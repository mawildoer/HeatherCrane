asm(".global _printf_float"); // Allow printf floats

#include "motor.h"
#include "joystick.h"

Motor test(0,1,3);
const int LED_pin = 13;
const int roll_pin = A7;
const int pitch_pin = A8;
const int yaw_pin = A9;
const int button_pin = A6;

JoystickAxis *roll, *pitch, *yaw;

char str[128];

void setup() {
  pinMode(LED_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLDOWN);
  Serial.begin(115200);

  digitalWrite(LED_pin, HIGH);
  delay(1000);
  digitalWrite(LED_pin, LOW);

  roll = new JoystickAxisLinear(roll_pin,58,1017,534);
  pitch = new JoystickAxisLinear(pitch_pin,67,1015,510);
  yaw = new JoystickAxisLog(yaw_pin,46,741,56);
  
  test.enable();
}

void loop() {
  digitalWrite(LED_pin, HIGH);
  test.setSpeed(roll->pos());
  sprintf(str, "%f,%f,%f,%i\r\n", roll->pos(), pitch->pos(), yaw->pos(), digitalRead(button_pin));
  Serial.write(str);
  digitalWrite(LED_pin, LOW);
  delay(100);

  if (Serial.available()) {
    Serial.read();

    roll->printTuning();
    pitch->printTuning();
    yaw->printTuning();
  }
}

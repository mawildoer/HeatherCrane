#include "motor.h"
#include "joystick.h"

//Motor test(0,1,3);
const int LED_pin = 13;
const int roll_pin = A9;
const int pitch_pin = A8;
const int yaw_pin = A7;
const int button_pin = A6;

JoystickAxisLinear roll(roll_pin);
JoystickAxisLinear pitch(pitch_pin);
JoystickAxisLog yaw(yaw_pin);

char str[64];

class Axis {
private:
  Motor mot;
  double rf,pf,yf;

public:
  void enable() {
    mot.enable();
  }

  void disable() {
    mot.disable();
  }

  Axis (Motor m, double rf, double pf, double yf) : mot(m), rf(rf), pf(pf), yf(yf) {}

  double update (double r, double p, double y) {
    double speed = r*rf + p*pf  + y*yf;
    mot.setSpeed(speed);
    return speed;
  }
};

//Axis axes[] = {
//  Motor(
//}

void setup() {
  pinMode(LED_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLDOWN);
  Serial.begin(115200);

  digitalWrite(LED_pin, HIGH);
  delay(1000);
  digitalWrite(LED_pin, LOW);
  
  //test.enable();
}

void loop() {
  digitalWrite(LED_pin, HIGH);
  sprintf(str, "%f,%f,%f,%i\r\n", roll.read(), pitch.read(), yaw.read(), digitalRead(button_pin));
  Serial.write(str);
  digitalWrite(LED_pin, LOW);
  delay(20);
}

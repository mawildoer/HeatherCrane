#include "motor.h"

const double MOTOR_POWER_LIMIT = 0.5L;
Motor flMot(0, 1, 3, MOTOR_POWER_LIMIT); // DRV 1
Motor frMot(19, 18, 17, MOTOR_POWER_LIMIT); // DRV 2
Motor blMot(13, 14, 16, MOTOR_POWER_LIMIT); // DRV 3
Motor brMot(6, 7, 4, MOTOR_POWER_LIMIT); // DRV 4

Motor armMot(8, 5, 9, MOTOR_POWER_LIMIT); // DRV 5. For controlling arm height. Full power allowed as the actuator was originally run off this battery
Motor legMot(11, 12, 10, MOTOR_POWER_LIMIT); // DRV 6. For controlling leg width
Motor motors[] = {flMot, frMot, blMot, brMot, armMot, legMot};

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  Serial.begin(115200);
  Serial.println("Starting");

  for (int i=0; i<6; i++) {
    motors[i].enable();
    motors[i].setSpeed(0.3);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  
}

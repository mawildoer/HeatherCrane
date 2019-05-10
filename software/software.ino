#include "motor.h"
#include "joystick.h"

// Joystick
const int LED_pin = 13;
const int roll_pin = A7;
const int pitch_pin = A8;
const int yaw_pin = A9;
const int mode_button_pin = A6;
JoystickAxis *rollAx, *pitchAx, *yawAx;
double joystickInput[3];
int joystickAveragingCount=1;

// Motors. Magic numbers are pin numbers
const double MAX_SYSTEM_VOLTAGE = 27.0;
const double MOTOR_POWER_LIMIT = 1.0; //18.0 / MAX_SYSTEM_VOLTAGE; // Drill motor voltage / Battery voltage . Could be higher as the drill's max voltage is 21V
double motorSpeeds[4] = {0.0};
double motorSpeedMatrix[4][3] = {
  {1,1,-1}, 
  {-1,1,-1}, 
  {-1,1,1}, 
  {-1,-1,-1}
};

Motor DRV1(0, 1, 3, MOTOR_POWER_LIMIT); // DRV 1
Motor DRV2(19, 18, 17, MOTOR_POWER_LIMIT); // DRV 2
Motor DRV3(13, 14, 16, MOTOR_POWER_LIMIT); // DRV 3
Motor DRV4(6, 7, 4, MOTOR_POWER_LIMIT); // DRV 4
Motor DRV5(8, 5, 9, 1.0); // DRV 5. For controlling arm height. Full power allowed as the actuator was originally run off this battery
Motor DRV6(11, 12, 10, 12.0/MAX_SYSTEM_VOLTAGE); // DRV 6. For controlling leg width

Motor flMot = DRV2;
Motor frMot = DRV3;
Motor blMot = DRV5;
Motor brMot = DRV6;

Motor armMot = DRV3;
Motor legMot = DRV1; // Not currently functional

Motor driveMotors[] = {flMot, frMot, blMot, brMot};

void enableDriveMotors () {
  for (int i=0;i<4;i++) {
    driveMotors[i].enable();
  }
}

void disableDriveMotors () {
  for (int i=0;i<4;i++) {
    driveMotors[i].disable();
  }
}

void disableAuxMotors () {
  armMot.disable();
  legMot.disable();
}

// Debug
asm(".global _printf_float"); // Allow printf floats
char str[128];

// State
enum class Mode {drive, auxIdle, auxArm, auxLegs};
volatile Mode mode = Mode::auxIdle;
volatile bool modeChanged = true; // true for init on boot
double joystickMag = 0.0;
const double modeChangeJoystickMax = 0.2; // Limit for how far the joystick can be from center to change mode
long lastCycle = 0;
const long minCycleTime = 20;

void modeButtonPressed () {
  if (joystickMag < modeChangeJoystickMax) {
    if (mode == Mode::drive) mode = Mode::auxIdle;
    else if (mode == Mode::auxIdle) mode = Mode::drive;
    modeChanged = true;
  }
}

void setup() {
  pinMode(LED_pin, OUTPUT);
  pinMode(mode_button_pin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(mode_button_pin), modeButtonPressed, RISING);
  Serial.begin(115200);

  //  Blink LED so we know it's on
  digitalWrite(LED_pin, HIGH);
  delay(100);
  digitalWrite(LED_pin, LOW);

  // Init Joystick. Magic numbers are from tuning limits and function "printTuning"
  rollAx = new JoystickAxisLinear(roll_pin,58,1017,534);
  pitchAx = new JoystickAxisLinear(pitch_pin,67,1015,510);
  yawAx = new JoystickAxisLog(yaw_pin,46,741,56);

  delay(1000);
}

void loop() {
  // Cycle rate monitor
  digitalWrite(LED_pin, LOW);
  while (millis() - lastCycle < minCycleTime) delay(1);
  lastCycle = millis();
  digitalWrite(LED_pin, HIGH);

  // Debug
  sprintf(str, "%f,%f,%f", joystickInput[0], joystickInput[1], joystickInput[2]); Serial.write(str); // Print Joystick
  sprintf(str, ",%f,%f,%f,%f", motorSpeeds[0], motorSpeeds[1], motorSpeeds[2], motorSpeeds[3]); Serial.write(str); // Print motor output
  sprintf(str, ",%i", (int)mode); Serial.write(str); // Print mode
  Serial.println();

  // User
  joystickInput[0] = ((joystickInput[0] * (double)(joystickAveragingCount-1)) + rollAx->pos()) / (double)joystickAveragingCount;
  joystickInput[1] = ((joystickInput[1] * (double)(joystickAveragingCount-1)) + pitchAx->pos()) / (double)joystickAveragingCount;
  joystickInput[2] = ((joystickInput[2] * (double)(joystickAveragingCount-1)) + yawAx->pos()) / (double)joystickAveragingCount;
  joystickMag = sqrt(sq(joystickInput[0]) + sq(joystickInput[1]) + sq(joystickInput[2]));

  if (mode == Mode::drive) {
    if (modeChanged) {
      modeChanged = false;
      Serial.println("MODE: drive");
      
      enableDriveMotors();
      disableAuxMotors();
      joystickAveragingCount = 50;
    }

    double maxSpeed = 0.0;
    for (int oax=0; oax<4; oax++) {
      motorSpeeds[oax] = 0.0;
      for (int iax=0; iax<3; iax++) {
        motorSpeeds[oax] += (double)motorSpeedMatrix[oax][iax] * joystickInput[iax];
      }

      if (abs(motorSpeeds[oax]) > maxSpeed) maxSpeed = abs(motorSpeeds[oax]);
    }

    if (maxSpeed > 1.0) {
      for (int oax=0; oax<4; oax++) {
        motorSpeeds[oax] /= maxSpeed;
      }
    }

    for (int oax=0; oax<4; oax++) {
      driveMotors[oax].setSpeed(motorSpeeds[oax]);
    }
    
  } else if (mode == Mode::auxIdle) {
    if (modeChanged) {
      modeChanged = false;
      Serial.println("MODE: auxIdle");
    
      joystickAveragingCount = 1;
      disableDriveMotors();
    }
    
    if (abs(joystickInput[0]) - abs(joystickInput[1]) > 0.001) { // Roll > Pitch
      mode = Mode::auxLegs;
      modeChanged = true;
      
      legMot.enable();
    } else if (abs(joystickInput[1]) - abs(joystickInput[0]) > 0.001) { // Pitch > Roll
      mode = Mode::auxArm;
      modeChanged = true;
      
      armMot.enable();
    }
  } else if (mode == Mode::auxLegs) {
    if (modeChanged) {
      modeChanged = false;
      Serial.println("MODE: auxLegs");
    }
    
    if (joystickMag < 0.001) {
      mode = Mode::auxIdle;
      modeChanged = true;
    }
    
    armMot.setSpeed(joystickInput[0]);
    
  } else if (mode == Mode::auxArm) {
    if (modeChanged) {
      modeChanged = false;
      Serial.println("MODE: auxArm");
    }
    
    if (joystickMag < 0.001) {
      mode = Mode::auxIdle;
      modeChanged = true;
    }
    
    legMot.setSpeed(joystickInput[1]);
  }
}

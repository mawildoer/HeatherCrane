#include <math.h>

class JoystickAxis {
public:
  int pin;
  int raw_min, raw_max, raw_center;
  double center;
  double deadband = 0.1;
  bool update = false;
  
  double read() {
    int raw = analogRead(pin);
    if (raw < raw_min) {
      raw_min=raw;
      triggerUpdate();
    } else if (raw > raw_max) {
      raw_max=raw;
      triggerUpdate();
    }
    
    return map((double)raw, (double)raw_min, (double)raw_max, 0.0, 1.0);
  }

  void triggerUpdate () {
    update = true;
    center = map((double)raw_center, (double)raw_min, (double)raw_max, 0.0, 1.0);
  }

  virtual double pos() {Serial.println("NOT IMPLIMENTED");} // Return a value between -1 and 1, with 0 being center of the joystick

  virtual void printTuning() {
    Serial.print(pin);
    Serial.print(",");
    Serial.print(raw_min);
    Serial.print(",");
    Serial.print(raw_max);
    Serial.print(",");
    Serial.print(raw_center);
  }

  JoystickAxis (int p) : pin(p) {
    raw_center = analogRead(pin);
    raw_min = raw_center;
    raw_max = raw_center;    
    triggerUpdate();
  }

  JoystickAxis (int p, int mn, int mx, int c) : pin(p), raw_min(mn), raw_max(mx), raw_center(c) {
    triggerUpdate();
  }
};

class JoystickAxisLinear : public JoystickAxis {
public:
  double pos() {
    if (JoystickAxis::update) JoystickAxis::update = false;
    
    double raw_pos = read();
    double p;
    if (raw_pos < center) {
      p = map(raw_pos, 0.0, center, -1.0, 0.0);
    } else if (raw_pos > center) {
      p = map(raw_pos, center, 1.0, 0.0, 1.0);
    } else p = 0.0;

    if (abs(p) < deadband) {
      return 0.0;
    } else return p;
  }

  JoystickAxisLinear(int p) : JoystickAxis(p) {}

  JoystickAxisLinear(int p, int mn, int mx, int c) : JoystickAxis(p, mn, mx, c) {}

  void printTuning() {
    Serial.print("JoystickAxisLinear(");
    JoystickAxis::printTuning();
    Serial.print(")");
  }
};

class JoystickAxisLog : public JoystickAxis {
public:
  double a,b;
  
  void update () {
    b = sq((1.0/center)-1.0);
    a = 1.0/(b-1.0);
    JoystickAxis::update = false;
  }

  double pos() {
    if (JoystickAxis::update) update();
    double raw_pos = log((JoystickAxis::read() + a)/a)/log(b); 
    //double raw_pos = a*pow(b, JoystickAxis::read())-a
    double p = map(raw_pos, 0.0, 1.0, -1.0, 1.0);
    if (abs(p) < deadband) {
      return 0.0;
    } else return p;
  }

  JoystickAxisLog(int p) : JoystickAxis(p) {}

  JoystickAxisLog(int p, int mn, int mx, int c) : JoystickAxis(p, mn, mx, c) {
    JoystickAxis::triggerUpdate();
  }
  
  void printTuning() {
    Serial.print("JoystickAxisLog(");
    JoystickAxis::printTuning();
    Serial.print(")");
  }
};

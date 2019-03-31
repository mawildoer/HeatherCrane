const int ANALOG_READ_RESOLUTION = 10;

class JoystickAxis {
protected:
  int pin;
  int raw_min, raw_max, raw_center;
  
  double read() {
    analogReadResolution(ANALOG_READ_RESOLUTION);
    int raw = analogRead(pin);
    if (raw < raw_min) {
      raw_min=raw;
      triggerUpdate();
    } else if (raw > raw_max) {
      raw_max=raw;
      triggerUpdate();
    }
    
    return map(raw, raw_min, raw_max, 0.0, 1.0);
  }

  void triggerUpdate () {
    update();
    center = map(raw_center, raw_min, raw_max, 0.0, 1.0);
  }

public:
  virtual double pos()=0; // Return a value between -1 and 1, with 0 being center of the joystick
  virtual void update()=0;
  virtual void printTuning() {
    Serial.print(pin);
    Serial.print(",");
    Serial.print(raw_min);
    Serial.print(",");
    Serial.print(raw_max);
    Serial.print(",");
    Serial.print(raw_center);
  }

  double center;

  JoystickAxis (int p) : pin(p) {
    raw_center = analogRead(pin);
    triggerUpdate();
  }

  JoystickAxis (int p, int mn, int mx, int c) : pin(p), raw_min(mn), raw_max(mx), raw_center(c) {
    triggerUpdate();
  }
};

class JoystickAxisLinear : protected JoystickAxis {
protected:
  void update () {
    
  }
  
public:
  double pos() {
    double raw_pos = read();
    
    if (raw_pos < center) {
      return map(raw_pos, 0, center, -1.0, 0);
    } else if (raw_pos > center) {
      return map(raw_pos, center, 0, 0, 1.0);
    } else return 0.0;
  }

  JoystickAxisLinear(int p) : JoystickAxis(p) {}

  JoystickAxisLinear(int p, int mn, int mx, int c) : JoystickAxis(p, mn, mx, c) {}

  void printTuning() {
    Serial.print("JoystickAxisLinear(");
    JoystickAxis::printTuning();
    Serial.print(")");
  }
};

class JoystickAxisLog : protected JoystickAxis {
protected:
  double a,b;
  void update () {
    b = pow((1/center)-1,2);
    a = 1/(b-1);
  }

public:
  double pos() {
      return a*pow(b,read())-a;
    }

  JoystickAxisLog(int p) : JoystickAxis(p) {}

  JoystickAxisLog(int p, int mn, int mx, int c) : JoystickAxis(p, mn, mx, c) {}
  
  void printTuning() {
    Serial.print("JoystickAxisLog(");
    JoystickAxis::printTuning();
    Serial.print(")");
  }
};

class Motor {
private:
  int a, b, p, limit;
  int speed=0;
  bool enabled = false;

  void setDir () {
    if (speed > 0) {
      digitalWrite(a, HIGH);
      digitalWrite(b, LOW);
    } else if (speed < 0) {
      digitalWrite(a, LOW);
      digitalWrite(b, HIGH);
    } else {
      digitalWrite(a, LOW);
      digitalWrite(b, LOW);
    }
  }
  
public:
  Motor (int a, int b, int p) : a(a), b(b), p(p) {
    pinMode(p, OUTPUT);
    analogWrite(p, 0); // Make sure the motor's off in the beginning

    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
    digitalWrite(a, LOW);
    digitalWrite(b, LOW);
  }

  Motor (int a, int b, int p, double l) {
    Motor(a,b,p);
    setLimit(l);
  }
 
  void setSpeed (float s) {
    speed = (int)map(s, -1.0, 1.0, -(double)limit, (double)limit);
    
    if (speed < -limit) speed = -limit;
    else if (speed > limit) speed = limit;
    if (!enabled) speed=0;
    
    setDir();

    analogWrite(p, abs(speed));
  }

  void setLimit (double l) {
    disable();
    limit = (int)map(l, 0.0, 1.0, 0.0, 255.0);
    if (limit < -255) limit = -255;
    else if (limit > 255) limit = 255;
  }

  void enable() {
    enabled=true;
  }

  void disable() {
    enabled=false;
    setSpeed(0);
  }
};

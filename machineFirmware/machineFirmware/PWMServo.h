/*  Class to control dervos using pwm
*/

#ifndef PWMServo_h
#define PWMServo_h
#include <Arduino.h>

static bool firstServo = true;

class PWMServo
{
  public:
    PWMServo();
    void attach(int servoPin);
    void attach (int servoPin, int microSeconds);
    void writeMicroseconds(int microSeconds);

  private:
    int pin;
};

#endif

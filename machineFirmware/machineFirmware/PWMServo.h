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
    void attach(const int& servoPin);
    void attach (const int& servoPin, const int& microSeconds);
    void writeMicroseconds(const int& microSeconds);

  private:
    int pin;
};

#endif

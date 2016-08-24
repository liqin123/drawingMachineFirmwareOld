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
    void attach(int);
    void writeMicroseconds(int);

  private:
    int pin;
};

#endif

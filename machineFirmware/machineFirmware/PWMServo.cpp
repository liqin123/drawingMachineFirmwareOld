/*  Class to control a servo using pwm
*/

#include <Arduino.h>
#include "PWMServo.h"

PWMServo::PWMServo()
{
  if(firstServo)
  {
    Serial.println("First Servo");
    analogWriteRange(3000);
    analogWriteFreq(333);
    firstServo = false;
  }
}

void PWMServo::attach(const int& servoPin)
{
  pin = servoPin;
  pinMode(pin, OUTPUT);
  analogWrite(pin, 1500);
}

void PWMServo::attach(const int& servoPin, const int& microSeconds)
{
  pin = servoPin;
  pinMode(pin, OUTPUT);
  analogWrite(pin, microSeconds);
}

void PWMServo::writeMicroseconds(const int& microSeconds)
{
  analogWrite(pin, microSeconds);
}

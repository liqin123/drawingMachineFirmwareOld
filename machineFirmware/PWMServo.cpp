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

void PWMServo::attach(int servoPin)
{
  pin = servoPin;
  pinMode(pin, OUTPUT);
  analogWrite(pin, 1500);
}

void PWMServo::attach(int servoPin, int microSeconds)
{
  pin = servoPin;
  pinMode(pin, OUTPUT);
  analogWrite(pin, microSeconds);
}

void PWMServo::writeMicroseconds(int microSeconds)
{
  analogWrite(pin, microSeconds);
}

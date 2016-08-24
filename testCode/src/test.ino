#include <Arduino.h>

const int shoulderServoPin = 4;
const int elbowServoPin = 5;

void setup(void)
{
  pinMode(shoulderServoPin, OUTPUT);
  pinMode(elbowServoPin, OUTPUT);
  analogWriteFreq(333);
  analogWriteRange(3000);
}

void loop()
{
  for(int i = 10; i < 20; i++)
  {
    analogWrite(shoulderServoPin, i);
    delay(1000);
  }
  analogWrite(elbowServoPin, 1500);
}

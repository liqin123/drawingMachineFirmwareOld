/*  Class to control a drawing arm
*/

#include <Arduino.h>
#include "DrawingArm.h"

DrawingArm::DrawingArm()
{
  //Empty constructor - arduino style
}

void DrawingArm::attach(int shoulderServoPin, int elbowServoPin, int penServoPin)
{
  _shoulderServo.attach(shoulderServoPin);
  _elbowServo.attach(elbowServoPin);
  _penServo.attach(penServoPin);
}

int DrawingArm::fastMove(float x, float y, float z)
{
  Serial.printf("DrawingArm: %d, %d, %d\n", x, y, z);
}

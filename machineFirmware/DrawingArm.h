/*  Class to control a drawing arm
*/

#ifndef DrawingArm_h
#define DrawingArm_h
#include <Arduino.h>
#include <Servo.h>

class DrawingArm
{
  public:
    DrawingArm();
    void attach(int shoulderServoPin, int elbowServoPin, int penServoPin);
    int fastMove(float x, float y, float z);
  private:
    Servo _shoulderServo;
    Servo _elbowServo;
    Servo _penServo;
    struct coordinate
    {
      float x; float y; float z;
    };
    coordinate _lastPosition;
    struct servoAngles
    {
      float shoulder; float elbow; float pen;
    };
    servoAngles lastServoAngle;
    struct servoMicroseconds
    {
      int shoulder; int elbow; int pen;
    };
    servoMicroseconds _lastServoMicroseconds;
};

#endif

/*  Class to control a drawing arm
*/

#ifndef DrawingArm_h
#define DrawingArm_h
#include <Arduino.h>
#include <Servo.h>

typedef struct
{
  float x;
  float y;
  float z;
} coordinate_t;

typedef struct
{
  float shoulder;
  float elbow;
  float pen;
} servoAngles_t;

typedef struct
{
  uint16_t shoulder;
  uint16_t elbow;
  uint16_t pen;
} servoMicroseconds_t;

class DrawingArm
{
  public:
    DrawingArm();
    void attach(uint8_t shoulderServoPin, uint8_t elbowServoPin, uint8_t penServoPin);
    uint8_t fastMove(float x, float y, float z);

  private:
    int armLength;
    coordinate_t homePositionCoordinates;
    servoMicroseconds_t servoMinMicroseconds;
    servoMicroseconds_t servoMaxMicroseconds;
    servoAngles_t servoMinAngles;
    servoAngles_t servoMaxAngles;
    servoMicroseconds_t servoDeadbandMicroseconds;
    float armHomePositionAngle;
    uint16_t minReach;
    uint16_t maxReach;
    uint16_t minShoulderAngle;
    uint16_t maxShoulderAngle;
    uint16_t minElbowAngle;
    uint16_t maxElbowAngle;

    Servo shoulderServo;
    Servo elbowServo;
    Servo penServo;

    coordinate_t lastPositionCoordinates;
    servoAngles_t lastServoAngle;
    servoMicroseconds_t lastServoMicroseconds;

    servoAngles_t positionToAngles(coordinate_t position);
    servoMicroseconds_t anglesToMicroseconds(servoAngles_t angles);
    coordinate_t checkReach(coordinate_t position, uint8_t &limitReached);
    servoAngles_t checkAngles(servoAngles_t servoAngles, uint8_t &limitReached);
};

#endif

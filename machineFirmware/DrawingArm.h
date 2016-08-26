/*  Class to control a drawing arm
*/

#ifndef DrawingArm_h
#define DrawingArm_h
#include <Arduino.h>
#include <Servo.h>
#include "PWMServo.h"

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

typedef struct
{
  float shoulder;
  float elbow;
  float pen;
} servoMicrosecondsFloat_t;

class DrawingArm
{
  public:
    DrawingArm();
    void attach(uint8_t shoulderServoPin, uint8_t elbowServoPin, uint8_t penServoPin);
    void fastMove(float x, float y, float z);
    void move(float x, float y, float z, int stepSize);
    void draw(float x, float y, float z);
    unsigned long isMoveDone(bool block);
    void home();
    void pen(float z);

  private:
    servoMicroseconds_t servoMinMicroseconds;
    servoMicroseconds_t servoMaxMicroseconds;
    servoAngles_t servoMinAngles;
    servoAngles_t servoMaxAngles;
    servoMicrosecondsFloat_t servoMicrosecondsPerDegree;
    servoMicroseconds_t servoDeadbandMicroseconds;
    servoAngles_t servoMoveRates;
    servoMicroseconds_t servoMicrosecondsMoveRate;
    uint8_t minimumMoveTime;
    uint8_t penUpSpeed;
    uint8_t defaultSpeed;

    int armLength;
    coordinate_t homePosition;
    float armHomePositionAngle;
    uint16_t minReach;
    uint16_t maxReach;

    PWMServo shoulderServo;
    PWMServo elbowServo;
    Servo penServo;

    coordinate_t lastPosition;
    servoAngles_t lastServoAngle;
    servoMicroseconds_t lastServoMicroseconds;
    unsigned long lastMoveTime;
    unsigned long moveCompleteTime;

    servoAngles_t positionToAngles(coordinate_t position);
    servoMicroseconds_t anglesToMicroseconds(servoAngles_t angles);
    coordinate_t checkReach(coordinate_t position, uint8_t &limitReached);
    servoAngles_t checkAngles(servoAngles_t servoAngles, uint8_t &limitReached);
    void writeToServos(servoMicroseconds_t servoMicroseconds);
    unsigned long calculateMoveTime(servoMicroseconds_t servoMicroseconds);
    float distance(coordinate_t point1, coordinate_t point2);

};

#endif

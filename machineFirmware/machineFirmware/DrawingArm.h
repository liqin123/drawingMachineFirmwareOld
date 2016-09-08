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
    void attach(const uint8_t& shoulderServoPin, const uint8_t& elbowServoPin, const uint8_t& penServoPin);
    void fastMove(const float& x, const float& y, const float& z);
    void move(const float& x, const float& y, const float& z, const int& stepSize);
    void draw(const float& x, const float& y, const float& z);
    unsigned long isMoveDone(const bool& block);
    void home();
    void pen(const float& z);

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
    float penUpSpeed;
    float defaultSpeed;

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

    servoAngles_t positionToAngles(const coordinate_t& position);
    servoMicroseconds_t anglesToMicroseconds(const servoAngles_t& angles);
    coordinate_t checkReach(const coordinate_t& position, uint8_t &limitReached);
    servoAngles_t checkAngles(const servoAngles_t& servoAngles, uint8_t &limitReached);
    void writeToServos(const servoMicroseconds_t& servoMicroseconds);
    unsigned long calculateMoveTime(const servoMicroseconds_t& servoMicroseconds);
    float distance(const coordinate_t& point1, const coordinate_t& point2);

};

#endif

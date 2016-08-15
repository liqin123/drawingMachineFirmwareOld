/*  Class to control a drawing arm
*/

#include <Arduino.h>
#include "DrawingArm.h"

DrawingArm::DrawingArm()
{
  armLength = 1000;
  homePosition = {1000, 1000, 1000};
  servoMinMicroseconds = {710, 710, 700};     //MG90D for arms, MG90S for pen
  servoMaxMicroseconds = {2290, 2290, 2500};
  servoMinAngles = {13, 13, 4};
  servoMaxAngles = {167, 167, 176};
  servoDeadbandMicroseconds = {2, 2, 2};
  minimumMoveTime = 30;                          // milliseconds
  servoMoveRates = {500, 500, 500};              // degrees per second. Datasheet says 600, but seems a bit fast.

  armHomePositionAngle = 45.0;
  minReach = 120;
  maxReach = 1950;
  minShoulderAngle = 15;
  maxShoulderAngle = 150;
  minElbowAngle = 15;
  maxElbowAngle = 150;
  lastMoveTime = 0;
  moveCompleteTime = 0;

  // precomupte move reates in microseconds of servo command per second
  servoMicrosecondsMoveRate.shoulder = (servoMaxMicroseconds.shoulder - servoMinMicroseconds.shoulder) / (servoMaxAngles.shoulder - servoMinAngles.shoulder) * servoMoveRates.shoulder;
  servoMicrosecondsMoveRate.elbow = (servoMaxMicroseconds.elbow - servoMinMicroseconds.elbow) / (servoMaxAngles.elbow - servoMinAngles.elbow) * servoMoveRates.elbow;
  servoMicrosecondsMoveRate.pen = (servoMaxMicroseconds.pen - servoMinMicroseconds.pen) / (servoMaxAngles.pen - servoMinAngles.pen) * servoMoveRates.pen;
}

void DrawingArm::attach(uint8_t shoulderServoPin, uint8_t elbowServoPin, uint8_t penServoPin)
{
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);
  penServo.attach(penServoPin);
  Serial.print("Move rate: ");
  Serial.println(servoMicrosecondsMoveRate.shoulder);
}

uint8_t DrawingArm::fastMove(float x, float y, float z)
{
  coordinate_t pos = {x, y, z};
  Serial.print("DrawingArm: ");
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);

  uint8_t limitReached;
  // check that the reach is OK
  pos = checkReach(pos, limitReached);
  if(limitReached)
  {
    Serial.print("Reach limit: ");
    Serial.println(limitReached);
  }

  // compute arm angles and check
  servoAngles_t angles = positionToAngles(pos);
  angles = checkAngles(angles, limitReached);
  if(limitReached)
  {
    Serial.print("Angles limit: ");
    Serial.println(limitReached);
  }

  servoMicroseconds_t micros = anglesToMicroseconds(angles);
  writeToServos(micros);

  lastPosition = pos;
  lastServoAngle = angles;
  lastServoMicroseconds = micros;
}

servoAngles_t DrawingArm::positionToAngles(coordinate_t position)
{
  float a1;       //radians
  float a2;       //radians
  servoAngles_t servoAngles;

  if (position.x == 0)
  {
    a1 = PI / 2;
  } else {
    a1 = atan2(position.y, position.x);
  }
  a2 = acos(sqrt(sq(position.x) + sq(position.y)) / (2 * armLength));

  servoAngles.shoulder = (a1 + a2);
  servoAngles.shoulder *= (180 / PI);             // radians to degrees
  servoAngles.shoulder += armHomePositionAngle;   // add home position
  servoAngles.elbow = (PI / 2 + a2 - a1);
  servoAngles.elbow *= (180 / PI);                // radians to degrees
  servoAngles.elbow -= armHomePositionAngle;      // subtract home position

  if ( servoAngles.elbow < 0)                     // pick the right angle
  {
    servoAngles.elbow += 180;
  }

  //servoAngles.shoulder = 180 - servoAngles.shoulder;                    // Shoulder servo is inverted, so adjust angle

  servoAngles.pen = position.z /1000 * (servoMaxAngles.pen - servoMinAngles.pen) + servoMinAngles.pen;
    Serial.print("servoAngles: ");
    Serial.print(servoAngles.shoulder);
    Serial.print(",");
    Serial.print(servoAngles.elbow);
    Serial.print(",");
    Serial.println(servoAngles.pen);
  return servoAngles;
}

servoMicroseconds_t DrawingArm::anglesToMicroseconds(servoAngles_t angles)
{
  servoMicroseconds_t micros;

  uint16_t shoulderMicrosecondsRange = servoMaxMicroseconds.shoulder - servoMinMicroseconds.shoulder;
  micros.shoulder = (servoMaxAngles.shoulder - angles.shoulder) / servoMaxAngles.shoulder * shoulderMicrosecondsRange + servoMinMicroseconds.shoulder;

  uint16_t elbowMicrosecondsRange = servoMaxMicroseconds.elbow - servoMinMicroseconds.elbow;
  micros.elbow = (servoMaxAngles.elbow - angles.elbow) / servoMaxAngles.elbow * elbowMicrosecondsRange + servoMinMicroseconds.elbow;

  uint16_t penMicrosecondsRange = servoMaxMicroseconds.pen - servoMinMicroseconds.pen;
  micros.pen = angles.pen / servoMaxAngles.pen * penMicrosecondsRange + servoMinMicroseconds.pen;

  Serial.printf("anglesToMicroseconds: %d, %d, %d\n", micros.shoulder, micros.elbow, micros.pen);
  return micros;
}

coordinate_t DrawingArm::checkReach(coordinate_t position, uint8_t &limitReached)
{
  coordinate_t modifiedPosition = position;

  limitReached = 0;

  uint16_t reach = sqrt(sq(position.x) + sq(position.y));
  if(reach > maxReach)
  {
    modifiedPosition.x = position.x * maxReach / reach;
    modifiedPosition.y = position.y * maxReach / reach;
    limitReached = 1;
  }
  if(reach < minReach)
  {
    modifiedPosition.x = position.x * minReach / reach;
    modifiedPosition.y = position.y * minReach / reach;
    limitReached = 2;
  }
  return modifiedPosition;
}

servoAngles_t DrawingArm::checkAngles(servoAngles_t servoAngles, uint8_t &limitReached)
{
  servoAngles_t modifiedAngles = servoAngles;

  limitReached = 0;

  if (servoAngles.shoulder < minShoulderAngle)
  {
    servoAngles.shoulder = minShoulderAngle;
    limitReached = 1;
  }
  if (servoAngles.shoulder > maxShoulderAngle)
  {
    servoAngles.shoulder = maxShoulderAngle;
    limitReached = 2;
  }
  if (servoAngles.elbow < minElbowAngle)
  {
    servoAngles.elbow = minElbowAngle;
    limitReached = 3;
  }
  if (servoAngles.elbow > maxElbowAngle)
  {
    servoAngles.elbow = maxElbowAngle;
    limitReached = 4;
  }
  return modifiedAngles;
}

void DrawingArm::writeToServos(servoMicroseconds_t micros)
{
  isMoveDone(true);                                   // wait for previous move
  shoulderServo.writeMicroseconds(micros.shoulder);
  elbowServo.writeMicroseconds(micros.elbow);
  penServo.writeMicroseconds(micros.pen);
  calculateMoveTime(micros);
  lastMoveTime = millis();
}

void DrawingArm::home()
{
  // we need to lift pen first, but we may not know where the arm is
  // so need to access servo directly
  penServo.writeMicroseconds(anglesToMicroseconds(positionToAngles(homePosition)).pen);
  delay(700); //TODO put in proper delay
  fastMove(homePosition.x, homePosition.y, homePosition.z);
}

unsigned long DrawingArm::isMoveDone(bool block)
{
  unsigned long now = millis();
  unsigned long timeRemaining;
  if(moveCompleteTime > now)
  {
    timeRemaining = moveCompleteTime - now;
  } else {
    return 0;
  }

  if(block)
  {
    Serial.print("isMoveDone blocking: ");
    Serial.println(timeRemaining);
    delay(timeRemaining);
    return 0;
  } else {
    Serial.print("isMoveDone nonBlocking: ");
    Serial.println(timeRemaining);
    return timeRemaining;
  }
}

unsigned long DrawingArm::calculateMoveTime(servoMicroseconds_t servoMicroseconds)
{
  unsigned long shoulderTime = 1000 * abs((lastServoMicroseconds.shoulder - servoMicroseconds.shoulder)) / servoMicrosecondsMoveRate.shoulder;
  unsigned long elbowTime = 1000 * abs((lastServoMicroseconds.elbow - servoMicroseconds.elbow)) / servoMicrosecondsMoveRate.elbow;
  unsigned long penTime = 1000 * abs((lastServoMicroseconds.pen - servoMicroseconds.pen)) / servoMicrosecondsMoveRate.pen;
  unsigned long moveTime = max(max(max(shoulderTime, elbowTime), penTime), minimumMoveTime);
  moveCompleteTime = millis() + moveTime;
  return moveTime;
}

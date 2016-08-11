/*  Class to control a drawing arm
*/

#include <Arduino.h>
#include "DrawingArm.h"

DrawingArm::DrawingArm()
{
  armLength = 1000;
  homePositionCoordinates = {1000, 1000, 1000};
  servoMinMicroseconds = {710, 710, 710};
  servoMaxMicroseconds = {2290, 2290, 2290};
  servoMinAngles = {15, 15, 15};                   // assumes home position is 90 degree servo position
  servoMaxAngles = {165, 165, 165};
  servoDeadbandMicroseconds = {2, 2, 2};
  armHomePositionAngle = 45.0;
  minReach = 120;
  maxReach = 1975;
  minShoulderAngle = 15;
  maxShoulderAngle = 150;
  minElbowAngle = 15;
  maxElbowAngle = 150;
}

void DrawingArm::attach(uint8_t shoulderServoPin, uint8_t elbowServoPin, uint8_t penServoPin)
{
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);
  penServo.attach(penServoPin);
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

  servoMicroseconds_t micros = anglesToMicroseconds(positionToAngles(pos));
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
  servoAngles.shoulder *= 180 / PI;               // radians to degrees
  servoAngles.shoulder += armHomePositionAngle;   // add home position
  servoAngles.elbow = (PI / 2 + a2 - a1);
  servoAngles.elbow *= 180 / PI;                  // radians to degrees
  servoAngles.elbow -= armHomePositionAngle;      // subtract home position

  if ( servoAngles.elbow < 0)                     // pick the right angle
  {
    servoAngles.elbow += 180;
  }

  servoAngles.pen = position.z /1000 * (servoMaxAngles.pen - servoMinAngles.pen);
  return servoAngles;
}

servoMicroseconds_t DrawingArm::anglesToMicroseconds(servoAngles_t angles)
{
  servoMicroseconds_t servoMicroseconds;

  float shoulderAngleRange = servoMaxAngles.shoulder - servoMinAngles.shoulder;
  uint16_t shoulderMicrosecondsRange = servoMaxMicroseconds.shoulder - servoMinMicroseconds.shoulder;
  servoMicroseconds.shoulder = servoMinMicroseconds.shoulder + int(shoulderMicrosecondsRange/shoulderAngleRange * (angles.shoulder - servoMinAngles.shoulder) + 0.5);

  float elbowAngleRange = servoMaxAngles.elbow - servoMinAngles.elbow;
  uint16_t elbowMicrosecondsRange = servoMaxMicroseconds.elbow - servoMinMicroseconds.elbow;
  servoMicroseconds.elbow = servoMinMicroseconds.elbow + int(elbowMicrosecondsRange/elbowAngleRange * (angles.elbow - servoMinAngles.elbow) + 0.5);

  float penAngleRange = servoMaxAngles.pen - servoMinAngles.pen;
  uint16_t penMicrosecondsRange = servoMaxMicroseconds.pen - servoMinMicroseconds.pen;
  servoMicroseconds.pen = servoMinMicroseconds.pen + int(penMicrosecondsRange/penAngleRange * (angles.pen - servoMinAngles.pen) + 0.5);

  Serial.printf("ArmMicroseconds: %d, %d, %d\n", servoMicroseconds.shoulder, servoMicroseconds.elbow, servoMicroseconds.pen);
  return servoMicroseconds;
}

coordinate_t DrawingArm::checkReach(coordinate_t position, uint8_t &limitReached)
{
  coordinate_t modifiedPosition;

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
  servoAngles_t modifiedAngles;

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
}

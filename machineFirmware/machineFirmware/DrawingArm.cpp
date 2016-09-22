/*  Class to control a drawing arm
*/

#include <Arduino.h>
#include "DrawingArm.h"

DrawingArm::DrawingArm()
{
  armLength = 1000;
  homePosition = {1000, 1000, 800};
  servoMinMicroseconds = {710, 710, 700};     //MG90D for arms, MG90S for pen
  servoMaxMicroseconds = {2290, 2290, 2400};
  servoMinAngles = {13, 13, 4};
  servoMaxAngles = {167, 167, 176};
  servoDeadbandMicroseconds = {2, 2, 2};
  minimumMoveTime = 4;                          // milliseconds
  servoMoveRates = {500, 500, 500};              // degrees per second. Datasheet says 600, but seems a bit fast.
  penUpSpeed = 15;
  defaultSpeed = 3;

  armHomePositionAngle = 45.0;
  minReach = 350;
  maxReach = 1950;
  lastMoveTime = 0;
  moveCompleteTime = 0;

  // precomupte move rates in microseconds of servo command per second
  servoMicrosecondsMoveRate.shoulder = (servoMaxMicroseconds.shoulder - servoMinMicroseconds.shoulder) / (servoMaxAngles.shoulder - servoMinAngles.shoulder) * servoMoveRates.shoulder;
  servoMicrosecondsMoveRate.elbow = (servoMaxMicroseconds.elbow - servoMinMicroseconds.elbow) / (servoMaxAngles.elbow - servoMinAngles.elbow) * servoMoveRates.elbow;
  servoMicrosecondsMoveRate.pen = (servoMaxMicroseconds.pen - servoMinMicroseconds.pen) / (servoMaxAngles.pen - servoMinAngles.pen) * servoMoveRates.pen;

  //precompute number of microseconds per degree of servo moveCompleteTime
  servoMicrosecondsPerDegree.shoulder = (servoMaxMicroseconds.shoulder - servoMinMicroseconds.shoulder) / (servoMaxAngles.shoulder - servoMinAngles.shoulder);
  servoMicrosecondsPerDegree.elbow = (servoMaxMicroseconds.elbow - servoMinMicroseconds.elbow) / (servoMaxAngles.elbow - servoMinAngles.elbow);
  servoMicrosecondsPerDegree.pen = (servoMaxMicroseconds.pen - servoMinMicroseconds.pen) / (servoMaxAngles.pen - servoMinAngles.pen);
}

void DrawingArm::attach(const uint8_t& shoulderServoPin, const uint8_t& elbowServoPin, const uint8_t& penServoPin)
{
  penServo.attach(penServoPin);
  penServo.writeMicroseconds(servoMaxMicroseconds.pen);
  delay(700); // TODO make the pen lift optional..?
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);
  lastPosition = homePosition;
}

void DrawingArm::fastMove(const float& x, const float& y, const float& z)
{
  coordinate_t pos = {x, y, z};

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
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.print(angles.shoulder);
  Serial.print(",");
  Serial.print(angles.elbow);
  Serial.print(",");
  //Serial.print(pos.x);
  //Serial.print(",");
  //Serial.print(pos.y);
  //Serial.print(",");

  angles = checkAngles(angles, limitReached);
  if(limitReached)
  {
    Serial.print("Angles limit: ");
    Serial.println(limitReached);
  }
  //Serial.print(angles.shoulder);
  //Serial.print(",");
  //Serial.println(angles.elbow);

  servoMicroseconds_t micros = anglesToMicroseconds(angles);
  Serial.print(micros.shoulder);
  Serial.print(",");
  Serial.println(micros.elbow);
  //Serial.print(",");
  writeToServos(micros);

  lastPosition = pos;
  lastServoAngle = angles;
  lastServoMicroseconds = micros;
}

void DrawingArm::move(const float& x, const float& y, const float& z, const int& stepSize)
{
   coordinate_t pos = {x, y, z};

  float distanceMoved = distance(lastPosition, pos);
  long steps = int(distanceMoved/stepSize + 0.5);
  //Serial.println(steps);
  if(steps <= 1)
  {
    fastMove(x, y, z);
  } else {
    coordinate_t originalPosition = lastPosition;
    float deltaX = (pos.x - lastPosition.x) / steps;
    float deltaY = (pos.y - lastPosition.y) / steps;

    for(int i = 1; i <= steps; i++)
    {
      fastMove(originalPosition.x + deltaX * i, originalPosition.y + deltaY * i, z);
    }
  }
}

void DrawingArm::draw(const float& x, const float& y, const float& z)
{
  float smallDistance = 1;

  coordinate_t pos = {x, y, z};

  if(lastPosition.z > 500 && pos.z < 500 && distance(lastPosition, pos) > smallDistance)
  {
    //pen down and move, so move, then pen down
    //Serial.print(" Pen down and move ");
    move(pos.x, pos.y, lastPosition.z, penUpSpeed);
    move(pos.x, pos.y, pos.z, penUpSpeed);
    return;
  }

  if(lastPosition.z < 500 && pos.z > 500 && distance(lastPosition, pos) > smallDistance)
  {
    //pen up and move, so pen up, then move
    //Serial.print(" Pen up and move ");
    move(lastPosition.x, lastPosition.y, pos.z, penUpSpeed);
    move(pos.x, pos.y, pos.z, penUpSpeed);
    return;
  }

  if(lastPosition.z > 500 && pos.z > 500)
  {
    // pen up move
    //Serial.print(" Pen up move ");
    move(pos.x, pos.y, pos.z, penUpSpeed);
    return;
  }
  move(pos.x, pos.y, pos.z, defaultSpeed);
}

servoAngles_t DrawingArm::positionToAngles(const coordinate_t& position)
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
  a2 = acos(distance(coordinate_t {0, 0, 0}, position) / (2 * armLength));
  //a2 = acos(sqrt(sq(position.x) + sq(position.y)) / (2 * armLength));

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
  return servoAngles;
}

servoMicroseconds_t DrawingArm::anglesToMicroseconds(const servoAngles_t& angles)
{
  servoMicroseconds_t micros;

  micros.shoulder = (180 - angles.shoulder - servoMinAngles.shoulder) * servoMicrosecondsPerDegree.shoulder + servoMinMicroseconds.shoulder;
  micros.elbow = (180 - angles.elbow - servoMinAngles.elbow) * servoMicrosecondsPerDegree.elbow + servoMinMicroseconds.elbow;
  micros.pen = (angles.pen - servoMinAngles.pen) * servoMicrosecondsPerDegree.pen + servoMinMicroseconds.pen;

  return micros;
}

coordinate_t DrawingArm::checkReach(const coordinate_t& position, uint8_t &limitReached)
{
  coordinate_t modifiedPosition = position;

  limitReached = 0;

  uint16_t reach = distance(coordinate_t {0, 0, 0}, position);
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

servoAngles_t DrawingArm::checkAngles(const servoAngles_t& servoAngles, uint8_t &limitReached)
{
  servoAngles_t modifiedAngles = servoAngles;

  limitReached = 0;

  if (servoAngles.shoulder < servoMinAngles.shoulder)
  {
    modifiedAngles.shoulder = servoMinAngles.shoulder;
    limitReached = 1;
  }
  if (servoAngles.shoulder > servoMaxAngles.shoulder)
  {
    modifiedAngles.shoulder = servoMaxAngles.shoulder;
    limitReached = 2;
  }
  if (servoAngles.elbow < servoMinAngles.elbow)
  {
    modifiedAngles.elbow = servoMinAngles.elbow;
    limitReached = 3;
  }
  if (servoAngles.elbow > servoMaxAngles.elbow)
  {
    modifiedAngles.elbow = servoMaxAngles.elbow;
    limitReached = 4;
  }
  return modifiedAngles;
}

void DrawingArm::writeToServos(const servoMicroseconds_t& micros)
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
  delay(700); // TODO put in proper delay
  move(homePosition.x, homePosition.y, homePosition.z, penUpSpeed);
}

void DrawingArm::pen(const float& z)
{
  fastMove(lastPosition.x, lastPosition.y, z);
}

unsigned long DrawingArm::isMoveDone(const bool& block)
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
    delay(timeRemaining);
    return 0;
  } else {
    return timeRemaining;
  }
}

unsigned long DrawingArm::calculateMoveTime(const servoMicroseconds_t& servoMicroseconds)
{
  unsigned long shoulderTime = 1000 * abs((lastServoMicroseconds.shoulder - servoMicroseconds.shoulder)) / servoMicrosecondsMoveRate.shoulder;
  unsigned long elbowTime = 1000 * abs((lastServoMicroseconds.elbow - servoMicroseconds.elbow)) / servoMicrosecondsMoveRate.elbow;
  unsigned long penTime = 1000 * abs((lastServoMicroseconds.pen - servoMicroseconds.pen)) / servoMicrosecondsMoveRate.pen;
  unsigned long moveTime = max(max(max(shoulderTime, elbowTime), penTime), minimumMoveTime);
  moveCompleteTime = millis() + moveTime;
  return moveTime;
}

float DrawingArm::distance(const coordinate_t& point1, const coordinate_t& point2)
{
  return sqrt(sq(point1.x - point2.x) + sq(point1.y - point2.y));
}

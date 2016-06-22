#include <Servo.h>

Servo servoUnderTest;
const int servoPin = 5;
const int minMicros = 690;
const int maxMicros = 2520;

int numberOfSteps = 19;

void setup() {

  Serial.begin(115200);
  servoUnderTest.attach(servoPin);

}

void loop() {

  Serial.println("Starting test.");
  servoUnderTest.writeMicroseconds(minMicros);
  delay(600);
  servoUnderTest.writeMicroseconds(maxMicros);
  delay(5000);
  for(int i = 0; i <= numberOfSteps; i++)
  {
    int thisMicros = maxMicros - i * (maxMicros - minMicros) / numberOfSteps;
    Serial.println(String("Step: ") += String(i) += String(" Micros: ") += String(thisMicros));
    servoUnderTest.writeMicroseconds(thisMicros);
    delay(6000);
  }
  Serial.println("All Done.");
  while(1)
  {
    delay(1000);
  }
}

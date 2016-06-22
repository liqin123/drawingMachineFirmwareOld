#include <Servo.h>

Servo servoUnderTest;
const int servoPin = 5;
const int minMicros = 690;
const int maxMicros = 2520;

int numberOfSteps = 18;

void setup() {

  Serial.begin(115200);
  servoUnderTest.attach(servoPin);

}

void loop() {

  Serial.println("Starting test.");
  Serial.print(String("How many steps? <") += String(numberOfSteps) += String(">: "));

  int x;
  while (Serial.available() == 0)
  {
    x = Serial.parseInt();
  }

  Serial.println(String("Number was: ") += String(x));

  for (int i = 0; i < numberOfSteps; i++)
  {
    Serial.println(i);
  }

}

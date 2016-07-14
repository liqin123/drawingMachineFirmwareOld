#include <Servo.h>

Servo servoUnderTest;
const int servoPin = 5;
const int buttonPin = 12;
const int minMinMicros = 500;
const int maxMinMicros = 800;
const int minMaxMicros = 2000;
const int maxMaxMicros = 2700;

int numberOfSteps = 18;
unsigned long buttonDebounceTimer;

void setup() 
{

  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  servoUnderTest.attach(servoPin);

}

void loop() 
{

  Serial.println("Starting test.");
  Serial.println("Move servo to far right");
  int minMicros;
  while (digitalRead(buttonPin) == 1)
  {
    minMicros = map(analogRead(A0), 1024, 0, minMinMicros, maxMinMicros);
    servoUnderTest.writeMicroseconds(minMicros);
    delay(100);
  }
  Serial.println(String("minMicros: ") += minMicros);
  delay(500); //lazy switch debounce

  Serial.println("Move servo to far left");
  int maxMicros;
  while (digitalRead(buttonPin) == 1)
  {
    maxMicros = map(analogRead(A0), 1024, 0, minMaxMicros, maxMaxMicros);
    servoUnderTest.writeMicroseconds(maxMicros);
    delay(100);
  }

  Serial.println(String("maxMicros: ") += maxMicros);
  delay(500); //lazy switch debounce
  waitForButton();
  Serial.println("Starting calibration sequence");

  servoUnderTest.writeMicroseconds(minMicros);
  
  for (int i = 0; i <= numberOfSteps; i++)
  {
    int thisMicros =  minMicros + i * (maxMicros - minMicros) / numberOfSteps;
    Serial.println(String("Step: ") += String(i) += String(" Micros: ") += String(thisMicros));
    servoUnderTest.writeMicroseconds(thisMicros);
     waitForButton();
  }
  Serial.println("All Done.");
  while (1)
  {
    delay(1000);
  }
}

void waitForButton()
{
  while(digitalRead(buttonPin) == 1 || millis() < buttonDebounceTimer)
  {
    delay(20);
  }
  buttonDebounceTimer = millis() + 500;
}


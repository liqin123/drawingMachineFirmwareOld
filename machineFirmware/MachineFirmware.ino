//https://evothings.com/how-to-connect-your-phone-to-your-esp8266-module/
//http://192.168.0.29:1337/'H'
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
//#include <ServoRob.h>

const int LED_PIN = 2;

const float shoulderServoMin = 5;
const float shoulderServoMax = 175;
const float elbowServoMin = 5;
const float elbowServoMax = 175;
const float penServoMin = 0;
const float penServoMax = 180;
const int servoMaxMicroseconds = 2520;
const int servoMinMicroseconds = 690;
const int servoRangeMicroseconds = servoMaxMicroseconds - servoMinMicroseconds;

float lastShoulderServoAngle;
float lastElbowServoAngle;
float lastPenServoAngle;

unsigned long shoulderMoveDoneTime;
unsigned long elbowMoveDoneTime;
unsigned long penMoveDoneTime;

// Servo motion rates in us/degree
const int shoulderServoMoveRate = 3000;
const int elbowServoMoveRate = 3000;
const int penServoMoveRate = 3000;

const int maxReach = 1875; //relative to arm lenghth, where arm = 1000
const int minReach = 750;

// Variable to hold time in millis when the move will be completed
unsigned long timeWhenMoveDone = 0;

// Shoulder
Servo shoulderServo;
const int shoulderServoPin = 15;
// Elbow
Servo elbowServo;
const int elbowServoPin = 13;
// Up/down
Servo penServo;
const int penServoPin = 5;

///wifi
//const char* ssid = "durrellphone";
//const char* ssid = "durrell";
const char* ssid = "orb";
//const char* password = "0101010101";
const char* password = "Blenet2238";
WiFiServer server(1337);
void printWiFiStatus();

float xValue;
float yValue;
float zValue;

float shoulderServoAngle;
float elbowServoAngle;

float lastShoulderAngle;
float lastElbowAngle;

int armLength = 1000; //not actual length, just use 1000 to make user side hardware independent

void setup(void) {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  initPins();

  //Lift pen
  penServo.writeMicroseconds(2300);

  // Start TCP server.
  server.begin();
  Serial.println("Start");
}

void loop(void) {

  // Check if module is still connected to WiFi.
  keepConnected();

  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");

    while (client.connected())
    {
      if (client.available() )
      {
        digitalWrite(LED_PIN, HIGH);
        String req = client.readStringUntil(';');

        if (req[0] == 0)
        {
          // edit out  a null charecter
          req = req.substring(1, req.length());
        }

        int Num = 0;

        Num = req.indexOf('x');// has to be an x
        //Serial.println(Num);
        //Serial.println(req);
        //Serial.println(req.length());
        //Serial.println();
        if (Num > 0)
        {

          parseString(req, Num);
          //delay(10);
        }

        checkBounds(&xValue, &yValue, maxReach, minReach);
        
        computeArmAngles(xValue, yValue);

        waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
        Serial.print(xValue);
        Serial.print(",");
        Serial.print(yValue);
        Serial.print(",");
        Serial.print(zValue);
        Serial.print(" :: ");
        Serial.print(shoulderServoAngle);
        Serial.print(",");
        Serial.println(elbowServoAngle);
        servoWrite(shoulderServoAngle, elbowServoAngle, zValue / 1000 * 180);
        digitalWrite(LED_PIN, LOW);
      }
    }
    // Client has disconnected
    Serial.println("Client disconnected.");
    client.stop();

    //Lift and home pen
    waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
    //raise pen
    computeArmAngles(xValue, yValue);
    servoWrite(shoulderServoAngle, elbowServoAngle, 1000 / 1000 * 180);
    waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
    //home
    computeArmAngles(1000, 1000);
    servoWrite(shoulderServoAngle, elbowServoAngle, 1000 / 1000 * 180);
    waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  }
}

int checkBounds(float *xValue, float *yValue, int maxReach, int minReach)
{
  int reach = sqrt(sq(* xValue) + sq(* yValue));
  if(reach > maxReach)
  {
    Serial.print("Max Reach Limit=");
    Serial.print(reach);
    Serial.print("/");
    Serial.print(maxReach);
    Serial.print(" xy=");
    Serial.print(* xValue);
    Serial.print(",");
    Serial.println(* yValue);
    (* xValue) = (*xValue) * maxReach/reach;
    (* yValue) = (*yValue) * maxReach/reach;
    return (reach - maxReach);
  }
  
  if(reach < minReach)
  {
    Serial.print("Min Reach Limit=");
    Serial.print(reach);
    Serial.print("/");
    Serial.print(minReach);
    Serial.print(" xy=");
    Serial.print(* xValue);
    Serial.print(",");
    Serial.println(* yValue);
    (* xValue) = (*xValue) * minReach/reach;
    (* yValue) = (*yValue) * minReach/reach;
    return (minReach - reach);
  }
  return 0;
}
  
void waitForServos(int shoulderMoveDoneTime, int elbowMoveDoneTime, int penMoveDoneTime)
{
  unsigned long waitUntilTime = 0;
  if (shoulderMoveDoneTime > waitUntilTime)
  {
    waitUntilTime = shoulderMoveDoneTime;
  }
  if (elbowMoveDoneTime > waitUntilTime)
  {
    waitUntilTime = elbowMoveDoneTime;
  }
  if (penMoveDoneTime > waitUntilTime)
  {
    waitUntilTime = penMoveDoneTime;
  }
  unsigned long nowMillis = millis();
  long timeToWaitFor = waitUntilTime - nowMillis;
  if (timeToWaitFor > 0)
  {
    Serial.print("Waiting for ");
    Serial.println(timeToWaitFor);
    if (timeToWaitFor < 1000)
    {
      delay(timeToWaitFor);
    } else {
      Serial.println("Abnormally long wait!");
    }
  }
}

void servoWrite(float shoulderServoAngle, float elbowServoAngle, float penServoAngle)
{

  if (shoulderServoAngle < shoulderServoMin)
  {
    shoulderServoAngle = shoulderServoMin;
    Serial.println("@@@SSSSSSSS@@@");
  }
  if (shoulderServoAngle > shoulderServoMax)
  {
    shoulderServoAngle = shoulderServoMax;
    Serial.println("@@@SSSSSSSS@@@");
  }
  // Write Value to servo
  shoulderServo.writeMicroseconds((180 - shoulderServoAngle) / 180 * servoRangeMicroseconds + servoMinMicroseconds);

  //Calculate when move will complete
  shoulderMoveDoneTime = millis() + abs(lastShoulderServoAngle - shoulderServoAngle) * shoulderServoMoveRate / 1000;
  lastShoulderServoAngle = shoulderServoAngle;

  //limits checking
  if (elbowServoAngle < elbowServoMin)
  {
    elbowServoAngle = elbowServoMin;
    Serial.println("@@@EEEEEEEE@@@");
  }
  if (elbowServoAngle > elbowServoMax)
  {
    elbowServoAngle = elbowServoMax;
    Serial.println("@@@EEEEEEEE@@@");
  }

  // Write Value to servo
  elbowServo.writeMicroseconds((180 - elbowServoAngle) / 180 * servoRangeMicroseconds + servoMinMicroseconds);

  //Calculate when move will complete
  elbowMoveDoneTime = millis() + abs(lastElbowServoAngle - elbowServoAngle) * elbowServoMoveRate / 1000;
  lastElbowServoAngle = elbowServoAngle;

  // Pen servo
  if (penServoAngle < penServoMin)
  {
    penServoAngle = penServoMin;
    Serial.println("@@@PPPPPPPP@@@");
  }
  if (penServoAngle > penServoMax)
  {
    penServoAngle = penServoMax;
    Serial.println("@@@PPPPPPPP@@@");
  }
  // Write Value to servo
  penServo.writeMicroseconds(penServoAngle / 180 * servoRangeMicroseconds + servoMinMicroseconds);

  //Calculate time when move will complete 
  penMoveDoneTime = millis() + abs(lastPenServoAngle - penServoAngle) * penServoMoveRate / 1000;
  lastPenServoAngle = penServoAngle;
}

void computeArmAngles(float x, float y)
{
  float a1;
  float a2;

  if (x == 0)
  {
    a1 = PI / 2;
  } else {
    a1 = atan2(y, x);
  }

  a2 = acos(sqrt(sq(x) + sq(y)) / (2 * armLength));

  shoulderServoAngle = (a1 + a2) * 180 / PI;
  elbowServoAngle = (PI / 2 + a2 - a1) * 180 / PI;
  if ( elbowServoAngle < 0)
  {
    elbowServoAngle += 180;
  };
  //  Serial.print(x);
  //  Serial.print(",");
  //  Serial.print(y);
  //  Serial.print(",");
  //  Serial.print(shoulderServoAngle);
  //  Serial.print(",");
  //  Serial.println(elbowServoAngle);
}

void parseString(String req, int letterOffset) {
  //"A  B  C"
  int fromNum = 0;

  //Serial.println("parse");
  //Serial.println(req.substring(fromNum, letterOffset));
  xValue = req.substring(fromNum, letterOffset).toFloat();
  fromNum = letterOffset;
  //Serial.println(xValue);

  letterOffset = req.indexOf("y");
  if (letterOffset > 0)
  {
    yValue = req.substring(fromNum + 1, letterOffset).toFloat();
    //Serial.println(yValue);
    fromNum = letterOffset;
  }

  letterOffset = req.indexOf("z");
  if (letterOffset > 0)
  {
    zValue = req.substring(fromNum + 1, letterOffset).toFloat();
    //Serial.println(zValue);
    fromNum = letterOffset;
  }
}

void printWiFiStatus() {
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void initPins() {

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);
  penServo.attach(penServoPin);
}

void keepConnected() {
  if (WiFi.status() != WL_CONNECTED) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print("waiting - Wifi.status=");
      Serial.println(WiFi.status());
    }
    // Print the new IP to Serial.
    Serial.println("I have connected to a wifi and will print my IP");
    printWiFiStatus();
    if (!MDNS.begin("esp8266")) {
      Serial.println("Error setting up MDNS responder!");
    }
    Serial.println("mDNS responder started");
  }
}


//https://evothings.com/how-to-connect-your-phone-to-your-esp8266-module/
//http://192.168.0.29:1337/'H'
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
//For Autoconfig
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Servo.h>
#include <ESP8266HTTPClient.h>
//#include <ServoRob.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
const char *APssid = "drawingMachine";
const char *APpassword = "drawingPassword";

const int RED_LED_PIN = 13;
const int GREEN_LED_PIN = 14;
const int BLUE_LED_PIN = 16;

int led_colour = 0;

const int SWITCH_PIN = 0;

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
const int shoulderServoMoveRate = 4000;
const int elbowServoMoveRate = 4000;
const int penServoMoveRate = 4000;

const int maxReach = 1970; //relative to arm lenghth, where arm = 1000
const int minReach = 200;

// Variable to hold time in millis when the move will be completed
unsigned long timeWhenMoveDone = 0;

// Shoulder
Servo shoulderServo;
const int shoulderServoPin = 4; //15
// Elbow
Servo elbowServo;
const int elbowServoPin = 5;//13
// Up/down
Servo penServo;
const int penServoPin = 12;//5

///wifi
//const char* ssid = "durrellphone";
//const char* ssid = "durrell";
const char* ssid = "orb";
//const char* password = "0101010101";
const char* password = "Blenet2238";
WiFiManager wifiManager;
WiFiServer server(1337);

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

  initPins();

  liftAndHome();

  Serial.println(ESP.getChipId());
  //  WiFi.begin(ssid, password);

  ///Wifi AutoAP setup
  wifiManager.setAPCallback(configModeCallback);
  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //Start a SoftAP...?
  WiFi.softAP(APssid, APpassword);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  //Start a proper DNS server as mDNS doesn't work in AP mode.
  dnsServer.setTTL(300);
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  dnsServer.start(DNS_PORT, "esp8266", apIP);
  //Start mDNS
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
  // Start TCP server.
  server.begin();
  Serial.println("Start");
}

void loop(void) {

  // Check if module is still connected to WiFi.
  //keepConnected();
  dnsServer.processNextRequest();
  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");
    dnsServer.processNextRequest();
    while (client.connected())
    {
      dnsServer.processNextRequest();

      if (digitalRead(SWITCH_PIN) == LOW)
      {
        Serial.println("Resetting WiFi Settings");
        wifiManager.resetSettings();
      }
      String req = client.readStringUntil(';');

      if (client.available() )
      {
        //digitalWrite(RED_LED_PIN, HIGH);
        if (req[0] == 0)
        {
          // edit out  a null charecter
          req = req.substring(1, req.length());
        }

        int Num = 0;

        Num = req.indexOf('x');// has to be an x
        if (Num > 0)
        {

          parseString(req, Num);
          //delay(10);
          fastMove(xValue, yValue, zValue);
        }

        //Gesture tests
        int gCmd = req.indexOf('g');
        if (gCmd > 0)
        {
          int gestureValue = req.substring(0, gCmd).toInt();
          //Serial.println(String("gesture") += gestureValue);
          doGesture(gestureValue);
        }
      }
    }
    // Client has disconnected
    Serial.println("Client disconnected.");
    client.stop();
    liftAndHome();
  }
}



int downloadAndDraw(String website, String path)
{
  HTTPClient http;
  String fullPath = "http://" + website + "/" + path;
  Serial.println(fullPath);
  http.begin(fullPath);
  int httpCode = http.GET();
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {

      // get length of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      Serial.printf("Document Size: %d\n", len);

      // get tcp stream
      WiFiClient stream = http.getStream();

      // read all data from server
      while (http.connected() && (len > 0 || len == -1)) {
        // get available data size
        size_t size = stream.available();
        Serial.printf("Stream avail: %d\n", size);
        yield();
        if (size) {
          //String thisLine = stream.readStringUntil('\n');
          //Serial.println("reading char");
          char c = stream.read();
          Serial.print(c);
//          String thisLine = "";
//          while (c != '\n')
//          {
//            thisLine += c;
//            c = stream.read();
//            yield();
//          }
//          Serial.println(thisLine);
//          parseFileLine(thisLine);
//          fastMove(xValue, yValue, zValue);
          delay(5);
        }
      }
    }
  }
  Serial.println("File Done..");
}

void doGesture(int gesture)
{
  switch (gesture)
  {
    case 1 :
      drawCircle(1000, 700, 300, 100, 2, 1000);
      //drawAntiCircle(1000, 700, 300, 100, 2, 1000);
      drawCircle(1000, 700, 300, 100, 2, 1000);
      drawCircle(1000, 700, 300, 100, 2, 1000);
      break;

    case 2 :
      drawSpiral(1000, 700, 600, 0, 2, 300, 4, 1000);
      break;

    case 3 :
      drawTriangle(1000, 1000, 800, 500, 2.5, 1000);
      break;

    case 4 :
      drawSquare(1000, 1000, 700, 400, 3, 1000);
      break;

    case 5 :
      gestureWave(2);
      break;

    case 6 :
      gestureYes(1);
      break;

    case 7 :
      gestureInfinity(1);
      break;

    case 8 :
      gestureCross(1);
      break;

    case 9 :
      gestureTick(10);
      break;

    case 10:
      downloadAndDraw("drawingmachine.s3-website-us-west-2.amazonaws.com", "Durrell/pic_20.txt");
      break;

    case 11:
      downloadAndDraw("www.robertpoll.com", "client/files/pic_20.txt");
      break;
  }
}

int gestureTick(int howMany)
{
  //drawLine(500, 500, 800, 200, 100, 0.4, 1000);
  //drawLine(800, 200, 1200, 1400, 300, 0.2, 1000);
  for (int i = 0; i < howMany; i++)
  {
    drawLine(1000, 1000, 1100, 1100, 10, 0.1, 1000);
    delay(500);
    drawLine(1100, 1100, 1000, 1000, 10, 0.1, 1000);
    delay(500);
  }
}

int gestureCross(int howMany)
{
  for (int i = 0; i < howMany; i++)
  {
    drawLine(200, 200, 1000, 1000, 100, 1, 1000);
    drawLine(1000, 200, 200, 1000, 100, 1, 1000);
  }
}

int gestureInfinity(int howMany)
{
  for (int i = 0; i < howMany; i++)
  {
    drawArc(700, 1000, 300, 90, 450, 100, 6, 1000);
    drawArc(1300, 1000, 300, 270, -90, 100, 6, 1000);
  }
}

int gestureYes(int howMany)
{
  for (int i = 0; i < howMany; i++)
  {
    drawLine(500, 500, 500, 1500, 100, .5, 1000);
    drawLine(500, 1500, 500, 500, 100, .5, 1000);
  }
}

int gestureWave(int howMany)
{
  for (int i = 0; i < howMany; i++)
  {
    drawArc(0, 0, 1300, -30, 30, 100, .4, 1000);
    drawArc(0, 0, 1300, 30, -30, 100, .4, 1000);
  }
}

int drawSquare(float centreX, float centreY, float sideLength, int steps, double drawTime, int zValue)
{
  drawLine(centreX + sideLength / 2, centreY + sideLength / 2, centreX + sideLength / 2, centreY - sideLength / 2, steps / 4, drawTime / 4, zValue);
  drawLine(centreX + sideLength / 2, centreY - sideLength / 2, centreX - sideLength / 2, centreY - sideLength / 2, steps / 4, drawTime / 4, zValue);
  drawLine(centreX - sideLength / 2, centreY - sideLength / 2, centreX - sideLength / 2, centreY + sideLength / 2, steps / 4, drawTime / 4, zValue);
  drawLine(centreX - sideLength / 2, centreY + sideLength / 2, centreX + sideLength / 2, centreY + sideLength / 2, steps / 4, drawTime / 4, zValue);
}

int drawTriangle(float centreX, float centreY, float sideLength, int steps, double drawTime, int zValue)
{
  int height = sideLength * sin(PI / 3);
  int topX = centreX;
  int topY = centreY + height / 2;
  int rightX = centreX + sideLength / 2;
  int rightY = centreY - height / 2;
  int leftX = centreX - sideLength / 2;
  int leftY = rightY;
  drawLine(topX, topY, rightX, rightY, steps / 3, drawTime / 3, zValue);
  drawLine(rightX, rightY, leftX, leftY, steps / 3, drawTime / 3, zValue);
  drawLine(leftX, leftY, topX, topY, steps / 3, drawTime / 3, zValue);
}

int drawLine(float startX, float startY, float endX, float endY, int steps, double drawTime, int zValue)
{
  for (int i = 0; i < steps; i++)
  {
    fastMove(startX + (endX - startX) * i / steps, startY + (endY - startY) * i / steps, zValue);
    delay(drawTime / steps * 1000);
  }
}

int drawSpiral(float centreX, float centreY, float startRadius, float endRadius, float revolutions, int steps, double drawTime, int zValue)
{
  for (int i = 0; i < steps; i++)
  {
    float angle = revolutions * i / steps * 2 * PI;
    float thisRadius = (endRadius - startRadius) * i / steps + startRadius;
    fastMove(centreX + thisRadius * sin(angle), centreY + thisRadius * cos(angle), zValue);
    delay(drawTime / steps * 1000);
  }
}

int drawArc(float centreX, float centreY, float radius, float startAngle, float endAngle, int steps, double drawTime, int zValue)
{
  for (int i = 0; i < steps; i++)
  {
    float angle = (endAngle / 180 * PI - startAngle / 180 * PI) / steps * i + startAngle / 180 * PI;
    fastMove(centreX + radius * sin(angle), centreY + radius * cos(angle), zValue);
    delay(drawTime / steps * 1000);
  }
}

int drawCircle(float centreX, float centreY, float radius, int steps, double drawTime, int zValue)
{
  drawArc(centreX, centreY, radius, 0, 360, steps, drawTime, zValue);
}

int drawAntiCircle(float centreX, float centreY, float radius, int steps, double drawTime, int zValue)
{
  drawArc(centreX, centreY, radius, 360, 0, steps, drawTime, zValue);
}

int fastMove(float xValue, float yValue, float zValue)
{
  checkBounds(&xValue, &yValue, maxReach, minReach);
  computeArmAngles(xValue, yValue);
  waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  //  Serial.print(xValue);
  //  Serial.print(",");
  //  Serial.print(yValue);
  //  Serial.print(",");
  //  Serial.print(zValue);
  //  Serial.print(" :: ");
  //  Serial.print(shoulderServoAngle);
  //  Serial.print(",");
  //  Serial.println(elbowServoAngle);
  servoWrite(shoulderServoAngle, elbowServoAngle, zValue / 1000 * 180);
}

//void ledColour(int colour)
//{
//  switch (led_colour) {
//    case 0  :
//      analogWrite(RED_LED_PIN, 1023);
//      analogWrite(GREEN_LED_PIN, 1023);
//      analogWrite(BLUE_LED_PIN, 1023);
//      break;
//    case 1  :
//      analogWrite(RED_LED_PIN, 1023);
//      analogWrite(GREEN_LED_PIN, 0);
//      analogWrite(BLUE_LED_PIN, 1023);
//      break;
//    case 2  :
//      analogWrite(RED_LED_PIN, 1023);
//      analogWrite(GREEN_LED_PIN, 1023);
//      analogWrite(BLUE_LED_PIN, 0);
//      break;
//    case 3  :
//      analogWrite(RED_LED_PIN, 0);
//      analogWrite(GREEN_LED_PIN, 1023);
//      analogWrite(BLUE_LED_PIN, 1023);
//      break;
//    case 4  :
//      analogWrite(RED_LED_PIN, 0);
//      analogWrite(GREEN_LED_PIN, 0);
//      analogWrite(BLUE_LED_PIN, 0);
//      break;
//  }
//}

void configModeCallback (WiFiManager * myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void liftAndHome()
{
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

int checkBounds(float * xValue, float * yValue, int maxReach, int minReach)
{
  int reach = sqrt(sq(* xValue) + sq(* yValue));
  if (reach > maxReach)
  {
    Serial.print("Max Reach Limit=");
    Serial.print(reach);
    Serial.print("/");
    Serial.print(maxReach);
    Serial.print(" xy=");
    Serial.print(* xValue);
    Serial.print(",");
    Serial.println(* yValue);
    (* xValue) = (*xValue) * maxReach / reach;
    (* yValue) = (*yValue) * maxReach / reach;
    return (reach - maxReach);
  }

  if (reach < minReach)
  {
    Serial.print("Min Reach Limit=");
    Serial.print(reach);
    Serial.print("/");
    Serial.print(minReach);
    Serial.print(" xy=");
    Serial.print(* xValue);
    Serial.print(",");
    Serial.println(* yValue);
    (* xValue) = (*xValue) * minReach / reach;
    (* yValue) = (*yValue) * minReach / reach;
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
      if (led_colour++ == 4)
      {
        led_colour = 0;
      }
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

void parseFileLine(String req) {
  int commaOffset = req.indexOf(',');
  xValue = req.substring(0, commaOffset).toFloat();
  Serial.print("xValue: ");
  Serial.println(xValue);
  req = req.substring(commaOffset + 1, req.length());

  commaOffset = req.indexOf(',');
  yValue = req.substring(0, commaOffset).toFloat();
  Serial.print("yValue: ");
  Serial.println(yValue);
  req = req.substring(commaOffset + 1, req.length());

  commaOffset = req.indexOf(',');
  zValue = req.substring(0, commaOffset).toFloat();
  Serial.print("zValue: ");
  Serial.println(zValue);
  yield();
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

//  pinMode(RED_LED_PIN, OUTPUT);
//  pinMode(GREEN_LED_PIN, OUTPUT);
//  pinMode(BLUE_LED_PIN, OUTPUT);
//  pinMode(SWITCH_PIN, INPUT);
//  ledColour(0);
  penServo.attach(penServoPin);
  shoulderServo.attach(shoulderServoPin);
  elbowServo.attach(elbowServoPin);

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


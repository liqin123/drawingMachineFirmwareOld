#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Servo.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <ESP8266httpUpdate.h>
#include "DrawingArm.h"

String compileTime = __TIME__;
String compileDate = __DATE__;

int eepromAutoFlag = 20;
int eepromAutoCount = eepromAutoFlag + sizeof(int);
int eepromAbortFlag = eepromAutoCount + sizeof(int);

const byte DNS_PORT = 53;
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);
const char *APssid = "drawingMachine";
const char *APpassword = "";

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
const float servoDeadBand = 0;

float lastShoulderServoAngle;
float lastElbowServoAngle;
float lastPenServoAngle;

unsigned long shoulderMoveDoneTime;
unsigned long elbowMoveDoneTime;
unsigned long penMoveDoneTime;

// Servo motion rates in us/degree
const int shoulderServoMoveRate = 6000;
const int elbowServoMoveRate = 6000;
const int penServoMoveRate = 6000;

const int maxReach = 1975; //relative to arm lenghth, where arm = 1000
const int minReach = 120;

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

DrawingArm arm;

///wifi
//const char* ssid = "durrellphone";
//const char* ssid = "durrell";
const char* ssid = "orb";
//const char* password = "0101010101";
const char* password = "Blenet2238";
WiFiManager wifiManager;
WiFiServer server(1337);
WiFiServer socketPolicyServer(843);

float xValue = 1000;
float yValue = 1000;
float zValue = 1000;

float shoulderServoAngle;
float elbowServoAngle;

float lastShoulderAngle;
float lastElbowAngle;

long waitCounter = 0;

int armLength = 1000; //not actual length, just use 1000 to make user side hardware independent

void setup(void) {
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.println();
  Serial.println(ESP.getChipId());

  Serial.print("This version complied: ");
  Serial.print(compileDate);
  Serial.print(" ");
  Serial.println(compileTime);

  arm.attach(shoulderServoPin, elbowServoPin, penServoPin);

  //liftAndHome();
  arm.home();

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

  //  //Start mDNS
  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }
  MDNS.addService("drawing", "tcp", 1337);
  // Start TCP server.
  server.begin();
  socketPolicyServer.begin();
  Serial.println("Start");
  //delay(1000);

  if(checkAbortFlag())
  {
    Serial.println("Autodraw was aborted - clearing");
    doGesture(21);
    clearAbortFlag();
  }

  if (checkAutoDraw())
  {
    setAbortFlag();
    Serial.print("Doing and auto drawing number: ");
    int i = checkAutoDraw();
    Serial.println(i);
    String fileName = "client/files/pic_" + String(i) + String(".txt");
    Serial.println(fileName);
    downloadAndDraw1("robertpoll.com", fileName);
    incrementAutoDraw();
    clearAbortFlag();
  } else {
    Serial.println("No Auto Drawing");
  }
  if (EEPROM.read(eepromAutoFlag) == 29)
  {
    setAbortFlag();
    Serial.println("Drawing default drawing");
    downloadAndDraw1("robertpoll.com", "client/files/pic_0.txt");
    clearAbortFlag();
  }
}

void loop(void) {

  // Check if module is still connected to WiFi.
  //keepConnected();
  dnsServer.processNextRequest();
  WiFiClient socketPolicyClient = socketPolicyServer.available();
  if(socketPolicyClient)
  {
    Serial.println("Connection on policy socket");
    socketPolicyClient.println("<?xml version=\"1.0\"?>");
    socketPolicyClient.println("<!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\">");
    socketPolicyClient.println("<cross-domain-policy>");
    socketPolicyClient.println("   <site-control permitted-cross-domain-policies=\"master-only\"/>");
    socketPolicyClient.println("   <allow-access-from domain=\"*\" to-ports=\"1337\" />");
    socketPolicyClient.println("</cross-domain-policy>");
  }

  WiFiClient client = server.available();

  if (client) {
    Serial.println("Client connected.");
    client.print("This version complied: ");
    client.print(compileDate);
    client.print(" ");
    client.println(compileTime);
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
        if (req[0] == 0)
        {
          // edit out  a null charecter
          req = req.substring(1, req.length());
        }

        int Num = 0;
        bool lineDone = false;

        Num = req.indexOf('x');// has to be an x
        if (Num > 0)
        {
          Serial.print("Received: ");
          Serial.println(req);
          parseString(req, Num);
          arm.fastMove(xValue, yValue, zValue);
          //fastMove(xValue, yValue, zValue);
          //delay(30);
          lineDone = true;
        }

        //Gesture tests
        int gCmd = req.indexOf('g');
        if (gCmd > 0)
        {
          int gestureValue = req.substring(0, gCmd).toInt();
          //Serial.println(String("gesture") += gestureValue);
          if(gestureValue == 30)
          {
            client.print("[OK]\0");
          } else {
            doGesture(gestureValue);
          }
          lineDone = true;
        }
        if (! lineDone)
        {
          int zCmd = req.indexOf('z');
          zValue = req.substring(0, zCmd).toFloat();
          Serial.println("Just do Z");
          arm.fastMove(xValue, yValue, zValue);
        }
      }
    }
    // Client has disconnected
    Serial.println("Client disconnected.");
    client.stop();
    //liftAndHome();
    arm.home();
  }
}

void setAbortFlag()
{
  Serial.println("set abort flag");
  EEPROM.write(eepromAbortFlag, 1);
  EEPROM.commit();
}

void clearAbortFlag()
{
  Serial.println("clear abort flag");
  EEPROM.write(eepromAbortFlag, 0);
  EEPROM.commit();
}

int checkAbortFlag()
{
  Serial.printf("Abort flag = %d\n", EEPROM.read(eepromAbortFlag));
  return EEPROM.read(eepromAbortFlag);
}

// int downloadAndDraw(String website, String path)
// {
//   WiFiClient client;
//   if ( !client.connect("robertpoll.com", 80) ) {
//     return false;
//   }
//
//   // Make an HTTP GET request
//   client.print("GET ");
//   client.print(path);
//   client.println(" HTTP/1.1");
//   client.print("Host: ");
//   client.println(website);
//   client.println("Connection: close");
//   client.println();
//
//   String thisLine = "";
//
//   while (true)
//   {
//     if ( client.available() ) {
//       char c = client.read();
//       thisLine += c;
//       if (c == '\n')
//       {
//         Serial.print(thisLine);
//         parseFileLine(thisLine);
//         arm.fastMove(xValue, yValue, zValue);
//         thisLine = "";
//       }
//       yield();
//     } else {
//       Serial.println("waiting...");
//       waitCounter++;
//       delay(100);
//     }
//
//     // If the server has disconnected, stop the client and WiFi
//     if ( !client.connected() ) {
//       Serial.println();
//
//       // Close socket
//       client.stop();
//       Serial.printf("Finished AutoDrawing. waitCounter: %d secs\n", waitCounter / 10);
//       waitCounter = 0;
//       return (0);
//     }
//   }
// }

int downloadAndDraw1(String website, String path)
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
      char c;
      String thisLine;

      bool done = false;
      while (! done)
      {
        if ( stream.available() ) {
          char c = stream.read();
          thisLine += c;
          if (c == '\n')
          {
            //Serial.print(thisLine);
            parseFileLine(thisLine);
            arm.fastMove(xValue, yValue, zValue);
            thisLine = "";
            delay(30);
          }
          yield();
        } else {
          Serial.println("waiting");
          waitCounter++;
          delay(100);
        }

        // If the server has disconnected, stop the client and WiFi
        if ( !stream.connected() ) {
          Serial.println();

          // Close socket
          stream.stop();
          Serial.printf("Finished AutoDrawing. waitCounter: %d secs\n", waitCounter / 10);
          done = true;
        }
      }



      // read all data from server
      //      while (http.connected() && (len > 0 || len == -1)) {
      //        // get available data size
      //        size_t size = stream.available();
      //        Serial.printf("Stream avail: %d\n", size);
      //        if (size)
      //        {
      //          yield();
      //          //String thisLine = stream.readStringUntil('\n');
      //
      //          c = stream.read();
      //          thisLine += c;
      //          if (c == '\n')
      //          {
      //            Serial.print(thisLine);
      //            thisLine = "";
      //          }
      //          yield();
      //
      //          //Serial.println(thisLine);
      //          //parseFileLine(thisLine);
      //          //arm.fastMove(xValue, yValue, zValue);
      //        } else {
      //          delay(100);
      //        }
      //      }
    }
  }
  Serial.println("File Done..");
}

int checkAutoDraw()
{
  if (EEPROM.read(eepromAutoFlag) != 27 && EEPROM.read(eepromAutoFlag) != 28)
  {
    Serial.println("AutoDraw never set");
    return (0);
  } else {
    if (EEPROM.read(eepromAutoFlag) == 28)
    {
      Serial.printf("Autodraw count returning %d\n", EEPROM.read(eepromAutoCount));
      return EEPROM.read(eepromAutoCount);
    } else {
      Serial.println("Autodraw not set");
      return (0);
    }
  }
}

int incrementAutoDraw()
{
  Serial.print("Incrementing eepromAutoCount to: ");
  if (EEPROM.read(eepromAutoCount) == 366)
  {
    EEPROM.write(eepromAutoCount, 1);
  } else {
    EEPROM.write(eepromAutoCount, EEPROM.read(eepromAutoCount) + 1);
  }
  EEPROM.commit();
  Serial.println(EEPROM.read(eepromAutoCount));
}

void doGesture(int gesture)
{
  switch (gesture)
  {
    case 1 :
      drawCircle(1000, 700, 300, 100, 2, 1000);
      drawCircle(1000, 700, 300, 100, 2, 1000);
      drawCircle(1000, 700, 300, 100, 2, 1000);
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

    case 10:
      downloadAndDraw1("drawingmachine.s3-website-us-west-2.amazonaws.com", "Durrell/pic_31.txt");
      break;

    case 11:
      downloadAndDraw1("www.robertpoll.com", "client/files/pic_31.txt");
      break;

    case 12:
      downloadAndDraw1("192.168.0.36", "pic_31.txt");
      break;

    //set AutoDraw
    case 20 :
      if (EEPROM.read(eepromAutoFlag) != 27 && EEPROM.read(eepromAutoFlag) != 28 && EEPROM.read(eepromAutoFlag) != 29)
      {
        Serial.println("AutoDraw set (new)");
        EEPROM.write(eepromAutoFlag, 28);
        EEPROM.write(eepromAutoCount, 1);
      } else {
        Serial.println("AutoDraw set");
        EEPROM.write(eepromAutoFlag, 28);
      }
      EEPROM.commit();
      break;

    //clear AutoDraw
    case 21 :
      if (EEPROM.read(eepromAutoFlag) != 27 && EEPROM.read(eepromAutoFlag) != 28 && EEPROM.read(eepromAutoFlag) != 29)
      {
        Serial.println("AutoDraw unset (new)");
        EEPROM.write(eepromAutoFlag, 27);
        EEPROM.write(eepromAutoCount, 1);
      } else {
        Serial.println("AutoDraw unset");
        EEPROM.write(eepromAutoFlag, 27);
      }
      EEPROM.commit();
      break;

    //reset AutoDraw
    case 22 :
      Serial.println("AutoDraw counter cleared");
      EEPROM.write(eepromAutoCount, 1);
      EEPROM.commit();
      break;

    //set DefaultAutoDraw
    case 23 :
      Serial.println("Default AutoDraw set");
      EEPROM.write(eepromAutoFlag, 29);
      EEPROM.commit();
      break;

    //Update firmware
    case 99 :
      Serial.println("Updating firmware...");
      ESPhttpUpdate.update("www.robertpoll.com", 80, "/client/files/firmware.bin");
      //ESPhttpUpdate.update("raw.githubusercontent.com", 80, "/robertpoll/drawingMachine/master/machineFirmware/.pioenvs/huzzah/firmware.bin");
      Serial.println("Firmware update failed.");
      break;
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

int drawLine(float startX, float startY, float endX, float endY, int steps, double drawTime, int zValue)
{
  for (int i = 0; i < steps; i++)
  {
    arm.fastMove(startX + (endX - startX) * i / steps, startY + (endY - startY) * i / steps, zValue);
    delay(drawTime / steps * 1000);
  }
}

int drawCircle(float centreX, float centreY, float radius, int steps, double drawTime, int zValue)
{
  drawArc(centreX, centreY, radius, 0, 360, steps, drawTime, zValue);
}

int drawArc(float centreX, float centreY, float radius, float startAngle, float endAngle, int steps, double drawTime, int zValue)
{
  for (int i = 0; i < steps; i++)
  {
    float angle = (endAngle / 180 * PI - startAngle / 180 * PI) / steps * i + startAngle / 180 * PI;
    arm.fastMove(centreX + radius * sin(angle), centreY + radius * cos(angle), zValue);
    delay(drawTime / steps * 1000);
  }
}

void configModeCallback (WiFiManager * myWiFiManager)
{
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void parseFileLine(String req)
{
  int commaOffset = req.indexOf(',');
  xValue = req.substring(0, commaOffset).toFloat() * 4;
  //Serial.print("xValue: ");
  //Serial.println(xValue);
  req = req.substring(commaOffset + 1, req.length());

  commaOffset = req.indexOf(',');
  yValue = req.substring(0, commaOffset).toFloat() * 4;
  //Serial.print("yValue: ");
  //Serial.println(yValue);
  req = req.substring(commaOffset + 1, req.length());

  commaOffset = req.indexOf(',');
  zValue = req.substring(0, commaOffset).toFloat();
  //Serial.print("zValue: ");
  //Serial.println(zValue);
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

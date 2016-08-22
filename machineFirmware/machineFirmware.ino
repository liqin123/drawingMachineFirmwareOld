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
#include "HTTPRangeClient.h"

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

const int shoulderServoPin = 4;
const int elbowServoPin = 5;
const int penServoPin = 12;

DrawingArm arm;

///wifi
WiFiManager wifiManager;
WiFiServer server(1337);
WiFiServer socketPolicyServer(843);

float xValue = 1000;
float yValue = 1000;
float zValue = 1000;

long waitCounter = 0;

void setup(void) {
  Serial.begin(115200);
  EEPROM.begin(512);
  Serial.println();
  Serial.printf("ChipID: %d\n", ESP.getChipId());

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
  Serial.printf("Heap: %d\n", ESP.getFreeHeap());
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
    String fileName = "/default/pic_" + String(i) + String(".txt");
    Serial.println(fileName);
    downloadAndDraw("drawingmachine.s3-website-us-west-2.amazonaws.com", fileName);
    incrementAutoDraw();
    clearAbortFlag();
  } else {
    Serial.println("No Auto Drawing");
  }
  if (EEPROM.read(eepromAutoFlag) == 29)
  {
    setAbortFlag();
    Serial.println("Drawing default drawing");
    downloadAndDraw("drawingmachine.s3-website-us-west-2.amazonaws.com", "/default/pic_0.txt");
    clearAbortFlag();
  }
  Serial.printf("Heap: %d\n", ESP.getFreeHeap());
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
            client.println("OK");
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
          arm.pen(zValue);
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

int downloadAndDraw(String website, String path)
{
  HTTPRangeClient http;
  
  String fullPath = "http://" + website + path;
  Serial.println(fullPath);

  if (http.begin(fullPath)) {
    // file found at server
    Serial.printf("DownloadAndDraw: Heap: %d\n", ESP.getFreeHeap());
    char c;
    String thisLine;
    bool done = false;
    while (! done)
    {
      if ( http.available() ) {
        char c = http.getChar();
        thisLine += c;
        if (c == '\n')
        {
          //Serial.print(thisLine);
          parseFileLine(thisLine);
          arm.fastMove(xValue, yValue, zValue);
          thisLine = "";
          //delay(30);
        }
        http.update();
        yield();
      } else {
        return 0;
      }

      // If the server has disconnected, stop the client and WiFi
      if ( !http.connected() ) {
        Serial.println();
        Serial.printf("Finished AutoDrawing. waitCounter: %d secs\n", waitCounter / 10);
        done = true;
      }
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

    case 40:
      for(int i = 200; i <= 1000; i += 100)
      {
        arm.fastMove(i, 700, 1000);
        drawLine(i, 700, i, -700, 500, 0, 0);
        arm.pen(1000);
      }
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
  Serial.println(WiFi.SSID());
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

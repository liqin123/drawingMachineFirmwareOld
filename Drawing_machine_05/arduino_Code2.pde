//https://evothings.com/how-to-connect-your-phone-to-your-esp8266-module/
//http://192.168.0.29:1337/'H'
//#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
//#include <Servo.h>
//#include <ServoRob.h>

//  int LED_PIN = 2;

float shoulderServoMin = 5;
float shoulderServoMax = 175;
float elbowServoMin = 5;
float elbowServoMax = 175;
float penServoMin = 0;
float penServoMax = 180;

float lastShoulderServoAngle;
float lastElbowServoAngle;
float lastPenServoAngle;

int shoulderMoveDoneTime;
int elbowMoveDoneTime;
int penMoveDoneTime;

// Servo motion rates in us/degree
int shoulderServoMoveRate = 3000;
int elbowServoMoveRate = 3000;
int penServoMoveRate = 3000;

// Variable to hold time in millis when the move will be completed
int timeWhenMoveDone = 0;

// Shoulder
//Servo shoulderServo;
//int shoulderServoPin = 15;
// Elbow
//Servo elbowServo;
//int elbowServoPin = 13;
// Up/down
//Servo penServo;
// int penServoPin = 5;

///wifi

//char* ssid = "durrell";
//  char* ssid = "orb";
// char* password = "0101010101";
//  char* password = "Blenet2238";
//WiFiServer server(1337);
//void printWiFiStatus();

float xValue;
float yValue;
float zValue;

float shoulderServoAngle;
float elbowServoAngle;

float lastShoulderAngle;
float lastElbowAngle;

int armLength = 1000; //not actual length, just use 1000 to make user side hardware independent

void A_setup() {
  //Serial.begin(115200);
  //WiFi.begin(ssid, password);

  //initPins();

  //Lift pen
  // ******* penServo.writeMicroseconds(2300);

  // Start TCP server.
  //server.begin();
  // println("Start");
}

void a_loop(String message) {

  // Check if module is still connected to WiFi.
  //keepConnected();

  //WiFiClient client = server.available();

  //if (client) {
  // println("Client connected.");

  // while (client.connected())
  // {
  //   if (client.available() )
  //{
  //  digitalWrite(LED_PIN, HIGH);
  //String req = client.readStringUntil(';');
  String req = message;//client.readStringUntil('m');

  //if (req[0] == 0)
  // {
  //   // edit out  a null charecter
  //   req = req.substring(1, req.length());
  //}

  int Num = 0;

  Num = req.indexOf('x');// has to be an x
  // println(Num);
  // println(req);
  // println(req.length());
  // println();
  if (Num > 0)
  {

    parseString(req, Num);
    //delay(10);
  }

  computeArmAngles(xValue, yValue);

  //waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  //Serial.print(xValue);
  //Serial.print(",");
  //Serial.print(yValue);
  //Serial.print(",");
  // println(zValue);
  servoWrite(shoulderServoAngle, elbowServoAngle, zValue / 1000 );
  //digitalWrite(LED_PIN, LOW);

  //delay(5);//20
  // }
  //}
  // stuck in a while loop
  //  println("Client disconnected.");
  //client.stop();

  //Lift and home pen
  //waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  //raise pen
 // computeArmAngles(xValue, yValue);
 // servoWrite(shoulderServoAngle, elbowServoAngle, 1000 / 1000 * 180);
  //waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  //home
  //computeArmAngles(1000, 1000);
 // seoWrite(shoulderServoAngle, elbowServoAngle, 1000 / 1000 * 180);
  //waitForServos(shoulderMoveDoneTime, elbowMoveDoneTime, penMoveDoneTime);
  //}
}

void waitForServos(int shoulderMoveDoneTime, int elbowMoveDoneTime, int penMoveDoneTime)
{
  int waitTime = 0;
  if (shoulderMoveDoneTime > waitTime)
  {
    //println("a");
    waitTime = shoulderMoveDoneTime;
  }
  if (elbowMoveDoneTime > waitTime)
  {
     //println("b");
    waitTime = elbowMoveDoneTime;
  }
  if (penMoveDoneTime > waitTime)
  {
    //println("c" +    penMoveDoneTime +"   " + waitTime);
    waitTime = penMoveDoneTime;
  }
  long  nowMillis = millis();
  if (waitTime > nowMillis)
  {
    if (waitTime - nowMillis > 0)
    {
      //print("Waiting for ");
      //println(waitTime - nowMillis);
      // delay(int(waitTime - nowMillis));
    }
  }
}

void servoWrite(float shoulderServoAngle, float elbowServoAngle, float penServoAngle)
{

  if (shoulderServoAngle < shoulderServoMin)
  {
    shoulderServoAngle = shoulderServoMin;
    println("@@@SSSSSSSS@@@");
  }
  if (shoulderServoAngle > shoulderServoMax)
  {
    shoulderServoAngle = shoulderServoMax;
    println("@@@SSSSSSSS@@@");
  }
  // Write Value to servo
  // shoulderServo.writeMicroseconds((180 - shoulderServoAngle) / 180 * 1600 + 700);
  //servoToScreenShoulder((180 - shoulderServoAngle) / 180 * 1600 + 700);
  servoToScreenShoulder(shoulderServoAngle);
  //Calculate when move will complete
  shoulderMoveDoneTime = int(millis() + abs(lastShoulderServoAngle - shoulderServoAngle) * shoulderServoMoveRate/1000);
  lastShoulderServoAngle = shoulderServoAngle;

  //limits checking
  if (elbowServoAngle < elbowServoMin)
  {
    elbowServoAngle = elbowServoMin;
    println("@@@@@@@@@@@@");
  }
  if (elbowServoAngle > elbowServoMax)
  {
    elbowServoAngle = elbowServoMax;
    println("@@@@@@@@@@@@");
  }

  // Write Value to servo
  //elbowServo.writeMicroseconds((180 - elbowServoAngle) / 180 * 1600 + 700);
 // servoToScreenElbow((180 - elbowServoAngle) / 180 * 1600 + 700);
 servoToScreenElbow(elbowServoAngle);
  //Calculate when move will complete
  elbowMoveDoneTime = int(millis() + abs(lastElbowServoAngle - elbowServoAngle) * elbowServoMoveRate/1000);
  lastElbowServoAngle = elbowServoAngle;

  // Pen servo
  if (penServoAngle < penServoMin)
  {
    penServoAngle = penServoMin;
    println("@@@PPPPPPPP@@@");
  }
  if (penServoAngle > penServoMax)
  {
    penServoAngle = penServoMax;
    println("@@@PPPPPPPP@@@");
  }
  // Write Value to servo
  //*******   penServo.writeMicroseconds(penServoAngle / 180 * 1600 + 700);

  //Calculate when move will complete
  penMoveDoneTime = int(millis() + abs(lastPenServoAngle - penServoAngle) * penServoMoveRate/1000);
  lastPenServoAngle = penServoAngle;
}

void computeArmAngles(float x, float y)
{
  println("m =",x,y);
  float a1;
  float a2;

  if (x == 0)
  {
    a1 = PI / 2;
  } else {
    a1 = atan2(y, x);
  }
//trace("k", x,y);
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
  //   println(elbowServoAngle);
}

void parseString(String req, int letterOffset) {
  //"A  B  C"
  int fromNum = 0;

  // println("parse");
  // println(req.substring(fromNum, letterOffset));
  xValue = float(req.substring(fromNum, letterOffset));
  fromNum = letterOffset;
  // println(xValue);

  letterOffset = req.indexOf("y");
  if (letterOffset > 0)
  {
    yValue = float(req.substring(fromNum + 1, letterOffset));
    // println(yValue);
    fromNum = letterOffset;
  }

  letterOffset = req.indexOf("z");
  if (letterOffset > 0)
  {
    zValue = float(req.substring(fromNum + 1, letterOffset));
    // println(zValue);
    fromNum = letterOffset;
  }
}

void printWiFiStatus() {
  //   println("");
  //  print("Connected to ");
  //    println(ssid);
  //  print("IP address: ");
  // println(WiFi.localIP());
}

void initPins() {

  //   pinMode(LED_PIN, OUTPUT);
  //  digitalWrite(LED_PIN, LOW);
  //   shoulderServo.attach(shoulderServoPin);
  //   elbowServo.attach(elbowServoPin);
  //   penServo.attach(penServoPin);
}

void keepConnected() {
  /*  if (WiFi.status() != WL_CONNECTED) {
   while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   //Serial.write(200);// tell somone on serial that I dont have a connection
   println("waiting");//
   }
   // Print the new IP to Serial.
   println("I have connected to a wifi and will print my IP");
   printWiFiStatus();
   if (!MDNS.begin("esp8266")) {
   println("Error setting up MDNS responder!");
   }
   println("mDNS responder started");
   }
   */
}
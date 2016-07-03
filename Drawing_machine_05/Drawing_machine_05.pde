import processing.net.*; 
import javax.swing.JFileChooser;

Client myClient; 

int lastX, lastY;
StringList loadedData;
String SavePath = "";
String SaveName = "";
int playPos = 0;

boolean wifi = true;
boolean homeFlag = true;
boolean mouseDownFlag= false;
boolean playFlag= false;

int sendTimer = 0;
boolean sendFlag = false;
int sendTimerDelay;
int zState = 0;

char myMode = 'l';


int  mousePosX;
int  mousePosY;


////Grid & box
int myState = 0;
int cntr1 = 0;
int xNumSteps ;
int yNumSteps;
int lineSpaceX ;
int lineSpaceY ;
int sendX ;
int sendY;
int thisX;
int thisY;
int origX;
int origY;
int theWidth;
int theHeight;
int theStepSize;
////
int stageWith = 800;
int stageHeight = 800;

String send;
float prop;

int YImageOffset = 0;
int XImageOffset = 0;

int simplifyData = 0;

int armScreenLength = 300;//stageWith/2;// in arduino = 1000;

int  preliftDelay;
int  postliftDelay;
 int lastZ;
 
 int minReach = 410;
 int maxReach = 1975;
void setup() {
  noCursor();
setUpCopy();
  loadedData = new StringList();

  if (wifi) {
    myClient = new Client(this, "esp8266.local", 1337);
    print(myClient);
  }
  size(800, 800);
  background(255, 255, 255);
  initRecord();
preliftDelay = 50;
postliftDelay = 700;
  
  sendTimerDelay = 50;
simplifyData = 0;
  prop = 1000.0/armScreenLength;
  drawedges();
}



void draw() {
preliftDelay = 0;
postliftDelay = 0;

  checkTimer();// sets sendflag
  if (sendFlag == true) {

    background(255, 255, 255);

    switch (myMode) {
    case 'l':

      mousePosX  = mouseX;
      mousePosY = mouseY;
     
      sendToServo(mousePosX, mousePosY, zState);
     
      a_loop(send); // caculate elbow and sholder
        background(255, 255, 255);
        drawImage();
      drawArm(); 
       drawedges();
      drawRecordList();
      drawText();
   
      break;

    case 'r':

      mousePosX  = mouseX;
      mousePosY = mouseY;
      // if (mouseDownFlag) {
       
      sendToServo(mousePosX, mousePosY, zState);
         background(255, 255, 255);
        drawImage();
        drawedges();
      addToRecord(mousePosX, mousePosY, zState);
      drawRecordList();
      drawText();
      //}
      break;
    case 'p':
        preliftDelay = 50;
postliftDelay = 800;

 
      if (playPos > (list.size() -1)) {
        playPos = 0;
        myMode = 'l';
      } else {
        mousePosX  = int(list.get(playPos));
        mousePosY = int(list.get(playPos + 1));
        int t = int(list.get(playPos + 2));
        zState = t;
        if (checkDifferance(mousePosX, mousePosY, simplifyData)) {
          sendToServo(mousePosX, mousePosY, zState);
          a_loop(send); // caculate elbow and sholder
             background(255, 255, 255);
        drawImage();
            drawedges();
          drawArm(); 
          drawRecordList();
          drawText();
        }
        playPos = playPos + 3;
      }

      break;
    case 'g':
      drawGrid();// moves
      sendToServo(mousePosX, mousePosY, zState);
      a_loop(send); // caculate elbow and sholder
          background(255, 255, 255);
        drawImage();
  drawedges();
      addToRecord(mousePosX, mousePosY, zState); 
      drawArm();
      drawText();
      drawRecordList();
      break;


    case 'b':
      drawBox();// moves
      sendToServo(mousePosX, mousePosY, zState);
      a_loop(send); // caculate elbow and sholder
         background(255, 255, 255);
        drawImage();
  drawedges();
      addToRecord(mousePosX, mousePosY, zState); 
      drawArm();
      drawText();
      drawRecordList();

      //

      break;
    }
  }
}


///limit timer

void checkTimer() {
  if (millis() > sendTimer) {
    sendFlag = true;
    sendTimer = millis()+ sendTimerDelay;
  } else {
    sendFlag = false;
  }
}

/// servo

void preDraw(int x, int y, int z) {
  //println(x, y, z);
  sendToServo(x, y, z);
}


void  sendToServo(int a1, int a2, int a3) {

  //a1 = 300;
  //a2 = 300;

  int temp_a1 = int((a1- sholderScreenLocation.x)*prop);
  int temp_a2 = int((sholderScreenLocation.y- a2) *prop);
  int temp_a3 = a3;//

  if ( a3 < 1 ) {
    temp_a3 = 1000;
  }

  if (homeFlag == true) {
    homeFlag = false;
    temp_a3 = 1000;
    temp_a1 = 1000;
    temp_a2 = 1000;
  } 
if (temp_a3 != lastZ) {
      delay(preliftDelay);
    }
    
  send = (str(temp_a1)+'x'+str(temp_a2)+'y'+str(temp_a3)+'z'+';');
  if (wifi) {
     if (lineFlag){
       
    myClient.write(send);
    if (temp_a3 != lastZ) {
      delay(postliftDelay);
      lastZ = temp_a3;
    }
     }
  }
}


boolean checkDifferance(int ax, int ay, int diff) {
  boolean changeX = ax < (lastX - diff) || ax > (lastX + diff);
  boolean changeY = ay < (lastY - diff) || ay > (lastY + diff); 
  //println(changeX,changeY);
  boolean ret = false;
  if ( changeX || changeY) {
    lastX = ax;
    lastY = ay;
    ret = true;
  }
  return(ret);
}

//////DRAWING SHAPES

//  initGrid(-500, -500, 2300, 2300, 6, 40);
void initGrid(int aorigX, int aorigY, int aWidth, int aHeight, int aStepSize, int numlines) {
  xNumSteps = aWidth / aStepSize;
  yNumSteps = aHeight / aStepSize;

  lineSpaceX = aWidth/ (numlines -1);
  lineSpaceY = aHeight/ (numlines -1);

  theWidth = aWidth;
  theHeight = aHeight;

  origX = aorigX;
  origY = aorigY;

  thisX = origX;
  thisY = origY;

  sendX = origX;
  sendY= origY;

  theStepSize = aStepSize;
  cntr1 = 0;
  myState = 0;
}  

void drawGrid() {

  switch(myState) {
  case 0:
    if (thisX <= (origX + theWidth)) {
      if (cntr1 <= yNumSteps) {
        mousePosX = thisX ;
        mousePosY = origY + (theStepSize * cntr1);
        // preDraw(sendX, sendY, 1);
        cntr1++;
        zState = 1;
      } else {
        // preDraw(sendX, sendY, 0);
        cntr1 = 0;
        thisX = thisX + lineSpaceX;
        zState = 0;
      }
    } else {
      // reset variables
      cntr1 = 0;
      myState = 1;
      thisY = origY;
      zState = 0;
    }

    break;
  case 1:
    //println(thisY ,(origY + theHeight));
    if (thisY <= (origY + theHeight)) {
      if (cntr1 <= xNumSteps) {
        mousePosY = thisY ;
        mousePosX = origX + (theStepSize * cntr1);
        //preDraw(sendX, sendY, 0);
        cntr1++;
        zState = 1;
      } else {
        //preDraw(sendX, sendY, 0);
        cntr1 = 0;
        thisY = thisY + lineSpaceY;
        zState = 0;
      }
    } else {
      //exit case & reset variables
      cntr1 = 0;
      myState = 0;
      thisY = origY;
      thisX = origX;
      zState = 0;
      myMode = 'l';
    }

    break;
  }
}

///BOX

void initBox(int aorigX, int aorigY, int aWidth, int aHeight, int aStepSize) {
  theStepSize = aStepSize;

  theWidth = aWidth;
  theHeight = aHeight;

  xNumSteps = aWidth / aStepSize;
  yNumSteps = aHeight / aStepSize;

  mousePosX = origX;
  mousePosY= origY;

  origX = aorigX;
  origY = aorigY;

  cntr1 = 0;
  myState = 0;
}


void drawBox() {
  switch(myState) {
  case 0:
    if (cntr1 <= xNumSteps) {
      mousePosX = origX + (theStepSize * cntr1);
      mousePosY = origY;
      zState = 1;
      cntr1 ++;
    } else {
      myState = 1;
      cntr1 = 0;
    }
    break;

  case 1:
    if (cntr1 <= yNumSteps) {
      mousePosX = mousePosX;
      mousePosY = origY + (theStepSize * cntr1);
      zState = 1;
      cntr1 ++;
    } else {
      cntr1 = 0;
      myState = 2;
    }
    break;
  case 2:
    if (cntr1 <= xNumSteps) {
      mousePosY = mousePosY;
      mousePosX = origX + theWidth - (theStepSize * cntr1);
      zState = 1;
      cntr1 ++;
    } else {
      myState = 3;
      cntr1 = 0;
    }
    break;
  case 3:
    if (cntr1 <= yNumSteps) {
      zState = 1;
      mousePosX = mousePosX;
      mousePosY = origY + theHeight - (theStepSize * cntr1);
      zState = 1;
      cntr1 ++;
    } else {
      myState = 0;
      cntr1 = 0;
      zState = 0;
      myMode = 'l';
    }
    break;
  }
}
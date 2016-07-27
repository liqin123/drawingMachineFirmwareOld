
String[] recordStringStates = {"OFF", "ON"}; 
String[] offlineText  = {"OFF LINE", "MOVE ARM"}; 
boolean lineFlag = true;
void mousePressed() {
  println("mousePressed");
  zState = 1;
  if (myMode != 'r') {

    clearRecord();
    myMode = 'r';
  }
  mouseDownFlag = true;
}

void mouseReleased() {
  println("mouseReleased");
  zState = 0;
}

void keyPressed() {
  int amount = 5;
  YImageOffset =  0;
  XImageOffset =  0;
  if (key == CODED) {
    if (keyCode == UP) {
      YImageOffset = YImageOffset - amount;
    } else if (keyCode == DOWN) {
      YImageOffset = YImageOffset = YImageOffset + amount;
    } else if (keyCode == LEFT) {
      XImageOffset = XImageOffset - amount;
    } else if (keyCode == RIGHT) {
      XImageOffset = XImageOffset + amount;
    } 
    alterRecordList(XImageOffset, YImageOffset);
  }
  switch (key) {
  case 'w':
    imgLocY = imgLocY  - 10;
    break;
  case 'x':
    imgLocY = imgLocY  + 10;
    break;
  case 'a':
    imgLocX = imgLocX  - 10;
    break;
  case 'd':
    imgLocX = imgLocX  + 10;
    break;
    
    case '-':
     armScreenLength = armScreenLength - 10;
     prop = 1000.0/armScreenLength;
    break;
     case '=':
    armScreenLength = armScreenLength + 10;
    prop = 1000.0/armScreenLength;
    break;
    
    
  case 'i':
    getBackground("http://johnsonbanks.co.uk/thoughtfortheweek/files/409_IMG_0411.jpg");
    break;

  case 'r':
    // FINISHED RECORDING Save and GO BACK TO LIVE
    loadedData.clear(); 
    saveData("tempDrawing");
    //println("tempDrawing");
    myMode = 'l';
    mouseDownFlag = false;

    break;

  case 'p':
    myMode = 'p';
    //palatFlag = true;
    break;

  case 'g':
    initGrid(200, 200, 200, 200, 5, 6);
    myMode = 'g';
    break;

  case 'b':
    initBox(200, 200, 100, 100, 10);
    myMode = 'b';
    break;

  case 'l':
    clearRecord();
    String[] temp = loadData("test");
    turnDataIntoRecording(temp);
    //loadedData.append(temp);
    //println(loadedData);
    loadedData.clear();
    break;

  case 's':
    loadedData.clear(); 
    saveData("tempDrawing");
    println("tempDrawing");
    break;  

  case ' ':
    if (lineFlag == true) {
      lineFlag = false;
    } else {
      lineFlag = true;
    }
    break;
  }
}



void drawText() {

  //textSize(32);
  //fill(0, 102, 153);
  String s = "r = RECORDING... = " + recordStringStates[int(mouseDownFlag)] + "\nl = LOAD \ng = Grid \nb = BOX  \np = PLAY IMAGE \nmove arm = " + offlineText[int(lineFlag)];

  fill(50);
  text(s, stageWith - 150, 20);  // Text wraps within text bo
}
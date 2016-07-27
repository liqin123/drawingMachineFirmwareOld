StringList list;
//int playPos = 0;

void initRecord() {
  list = new StringList();
}

void clearRecord() {
  list.clear();
}

void startRecording() {
  //clearRecord();
  mouseDownFlag= true;
}

void stopRecord() {
  mouseDownFlag= false;
}


void turnDataIntoRecording(String[] temp) {
  for (int a = 0; a < temp.length; a = a + 3 ) {
    int x = int(temp[a]);
    int y = int(temp[a+1]);
    int z = int(temp[a+2]);
    addToRecord(x, y, z) ;
  }
}

void addToRecord(int ax, int ay, int az) {
  if (checkDifferance(ax, ay, simplifyData)) {
    list.append(str(ax));
    list.append(str(ay));
    list.append(str(az));
  }
}


void drawRecordList() {

  stroke(255, 0, 0);
  ellipse(mousePosX, mousePosY, 2, 2);
  if (list.size() > 4) {
    fill(200);
    strokeWeight(1);
    for (int x = 5; x <(list.size()); x = x + 3) {
      int t = (parseInt(list.get(x-3))); 
      if  ( t == 1) {
        line(parseInt(list.get(x - 5)), parseInt(list.get(x - 4)), parseInt(list.get(x - 2)), parseInt(list.get(x - 1)));
      }
    }
  }
}

void alterRecordList(int xNum, int yNum) {

  for (int p = 0; p <(list.size() - 3); p = p + 3) {
    int t = (parseInt(list.get(p))); 
    println(t, list.get(t ), list.get(t) + xNum);
    int a = parseInt(list.get(p))+ xNum ;
    int b = parseInt(list.get(p+ 1))+ yNum ;
    list.set(p, str(a));
    list.set(p+1, str(b));
  }
}
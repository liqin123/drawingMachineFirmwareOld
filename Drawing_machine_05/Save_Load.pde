
void saveData(String anam) {
  String tempName = SaveScreen(anam);
  if (tempName == "empty") {
    //
  } else {
    // Writes the strings to a file, each on a separate line
    anam = tempName + ".txt";
    if (list.size()> 0) {
      saveStrings(SavePath + "/"+tempName, list.array());
    }
  }
}


String[] loadData(String anam) {
  anam = loadScreen();
  //println("here" +anam);
  println(SavePath +"/" + anam);
  //anam = anam + ".txt";
  String lines[] = loadStrings(SavePath +"/" + anam);
  return(lines);
}




String loadScreen() {
  String returnName = "empty";
  try {

    JFileChooser fc = new JFileChooser();
    fc.setSelectedFile(new File(dataPath("") ));
    int returnVal = fc.showOpenDialog(null);

    if (returnVal == JFileChooser.APPROVE_OPTION) {
      SavePath = fc.getSelectedFile().getParent();
      returnName = fc.getSelectedFile().getName();
    } else {
      println("Open command cancelled by user.");
    }
  }
  catch (Exception e) {
    e.printStackTrace();
  }
  return (returnName);
}
String SaveScreen(String aNam) {
  JFileChooser chooser = new JFileChooser();
  chooser.setSelectedFile(new File(dataPath("") + ".txt"));
  int returnVal = chooser.showSaveDialog(null);
  String returnName = "empty";
  if (returnVal == JFileChooser.APPROVE_OPTION) 
  {
    SavePath = chooser.getSelectedFile().getParent();
    returnName = chooser.getSelectedFile().getName();
  }
  return(returnName);
}
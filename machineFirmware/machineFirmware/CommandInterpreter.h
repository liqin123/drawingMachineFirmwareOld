/*  Class to take a parsed line and translate it to a command for
    the arm to execute.
*/
#ifndef CommandInterpreter_h
#define CommandInterpreter_h
#include <Arduino.h>
#include "DrawingArm.h"
#include <vector>

enum class CommandTypes {fast, line, gesture, firmare, home, setting, factoryReset, error};
struct ArmCommand
{
  CommandTypes commandType;
  bool hasX = false;
  float x;
  bool hasY = false;
  float y;
  bool hasZ = false;
  float z;
  bool hasS = false;
  String s;
  bool hasP = false;
  String p;
};

struct gcodeField
{
  char cmdLetter;
  String data;
};

class CommandInterpreter
{
  public:
    CommandInterpreter();
    ArmCommand interpretCommand(std::vector<String> commandLine);



  private:
    ArmCommand doG00(std::vector<String> commandLine);
    ArmCommand doG01(std::vector<String> commandLine);
    gcodeField splitGcodeField(String command);
};
#endif

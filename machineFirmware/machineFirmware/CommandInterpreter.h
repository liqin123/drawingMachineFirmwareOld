/*  Class to take a parsed line and translate it to a command for
    the arm to execute.
*/
#ifndef CommandInterpreter_h
#define CommandInterpreter_h
#include <Arduino.h>
#include "DrawingArm.h"
#include <vector>

using namespace std;

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
    ArmCommand interpretCommand(const vector<String>& commandLine);

  private:
    ArmCommand doG00(vector<String> commandLine);
    ArmCommand doG01(vector<String> commandLine);
    gcodeField splitGcodeField(const String& command);
};
#endif

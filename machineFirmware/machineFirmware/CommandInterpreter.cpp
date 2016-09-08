/*  Class to take a parsed line and translate it to a command for
    the arm to execute.
*/

#include <Arduino.h>
#include "CommandInterpreter.h"

CommandInterpreter::CommandInterpreter()
{

}

ArmCommand CommandInterpreter::interpretCommand(const std::vector<String>& commandLine)
{
  gcodeField command = splitGcodeField(commandLine[0]);
  ArmCommand armCmd;

  if(command.cmdLetter == 'G'&& command.data.toInt() == 0)
  {
    armCmd = doG00(commandLine);
    return armCmd;
  }

  if(command.cmdLetter == 'G'&& command.data.toInt() == 1)
  {
    armCmd = doG01(commandLine);
    return armCmd;
  }
  armCmd.commandType = CommandTypes::error;
}

gcodeField CommandInterpreter::splitGcodeField(const String& command)
{
  gcodeField field;
  String s = command;
  s.toUpperCase();
  field.cmdLetter = s.charAt(0);
  field.data = command.substring(1);
  return field;
}

ArmCommand CommandInterpreter::doG00(std::vector<String> commandLine)
{
  ArmCommand armCmd;
  armCmd.commandType = CommandTypes::fast;
  commandLine.erase(commandLine.begin());     // remove the first itme in the vector, whic is G01

  Serial.println("G00");
  for(String& s : commandLine)
  {
    gcodeField field = splitGcodeField(s);
    switch(field.cmdLetter)
    {
      case 'X':
      armCmd.hasX = true;
      armCmd.x = field.data.toFloat();
      break;

      case 'Y':
      armCmd.hasY = true;
      armCmd.y = field.data.toFloat();
      break;

      case 'Z':
      armCmd.hasZ = true;
      armCmd.z = field.data.toFloat();
      break;
    }
  }
  return armCmd;
}

ArmCommand CommandInterpreter::doG01(std::vector<String> commandLine)
{
  ArmCommand armCmd;
  armCmd.commandType = CommandTypes::line;
  commandLine.erase(commandLine.begin());     // remove the first itme in the vector, whic is G01

  Serial.println("G01");
  for(String& s : commandLine)
  {
    gcodeField field = splitGcodeField(s);
    switch(field.cmdLetter)
    {
      case 'X':
      armCmd.hasX = true;
      armCmd.x = field.data.toFloat();
      break;

      case 'Y':
      armCmd.hasY = true;
      armCmd.y = field.data.toFloat();
      break;

      case 'Z':
      armCmd.hasZ = true;
      armCmd.z = field.data.toFloat();
      break;

      case 'S':
      armCmd.hasS = true;
      armCmd.s = field.data.toFloat();
      break;
    }
  }
  return armCmd;
}

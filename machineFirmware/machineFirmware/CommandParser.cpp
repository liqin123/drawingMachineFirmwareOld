/*  Class to parse commands into GCode
*/

#include <Arduino.h>
#include "CommandParser.h"

CommandParser::CommandParser(HTTPRangeClient& source)
:source{&source}
{

}

std::vector<String> CommandParser::getCommand(void)
{
  return splitLine(getLine());
}

String CommandParser::getLine(void)
{
  String line;

  for(;;)
  {
    char c = source->getChar();
    switch(c)
    {
      case 0:           // End of file
      return line;
      break;

      case '\n':
      return line;
      break;

      default:
      line += c;
    }
  }
  return line;
}

std::vector<String> CommandParser::splitLine(const String& line)
{
  std::vector<String> lineFields;
  int numberOfFields = 0;
  String field;
  bool lastCharWasntWhitespace = false;

  for(int i = 0; i < line.length(); i++)
  {
    char c = line[i];
    if(!isspace(c))       // TODO deal with quotes
    {
      field += c;
      lastCharWasntWhitespace = true;
    } else {
      if(lastCharWasntWhitespace)
      {
        lineFields.push_back(field);
        field = "";
        numberOfFields++;
        lastCharWasntWhitespace = false;
      }
    }
  }
  lineFields.push_back(field);
  return lineFields;
}

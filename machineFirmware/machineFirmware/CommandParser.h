/*  Class to parse commands into GCode
*/

#ifndef CommandParser_h
#define CommandParser_h
#include <Arduino.h>
#include <vector>
#include "HTTPRangeClient.h"

class CommandParser
{
public:
  CommandParser(HTTPRangeClient& source);
  std::vector<String> getCommand(void);

  private:
  HTTPRangeClient* source;

  String getLine(void);
  std::vector<String> splitLine(const String& line);

};
#endif

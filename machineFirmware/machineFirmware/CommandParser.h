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
  std::vector<String> getCommand();

  private:
  HTTPRangeClient* source;

  String getLine(void);
  std::vector<String> splitLine(String line);

};
#endif

/*  Class to do a drawing job
*/
#ifndef DrawingJob_h
#define DrawingJob_h
#include <Arduino.h>
#include "HTTPRangeClient.h"
#include "CommandParser.h"
#include "DrawingArm.h"
#include "CommandInterpreter.h"

class DrawingJob
{
  public:
    DrawingJob(HTTPRangeClient& HTTPSource, DrawingArm& arm);
    DrawingJob(WiFiClient& SocketSource, DrawingArm& arm);
    bool finished();
    void doLine(void);

  private:
    enum class SourceTypes {http, socket};
    SourceTypes sourceType;
    CommandParser* parser;
    DrawingArm* arm;

    HTTPRangeClient* HTTPSource;
    WiFiClient* SocketSource;

    std::vector<String> getParsedLine();
};
#endif

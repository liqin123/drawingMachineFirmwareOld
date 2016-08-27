/*  Class to do a drawing job
    The class deals with the different possible sources so that the caller
    does not need to.
*/

#include <Arduino.h>
#include "DrawingJob.h"

DrawingJob::DrawingJob(HTTPRangeClient& HTTPSource, DrawingArm& arm)
:HTTPSource{&HTTPSource},
arm{&arm},
sourceType{SourceTypes::http}
{
  parser = new CommandParser(HTTPSource);
}

DrawingJob::DrawingJob(WiFiClient& SocketSource, DrawingArm& arm)
:SocketSource{&SocketSource},
arm{&arm},
sourceType{SourceTypes::socket}
{
  //TODO get a commandparser once it supports sockets
}

bool DrawingJob::finished()
{
  switch(sourceType)
  {
    case SourceTypes::http :
    return !HTTPSource->available();

    case SourceTypes::socket :
    return true; //TODO add code for socket
  }
}

std::vector<String> DrawingJob::getParsedLine()
{
  switch(sourceType)
  {
    case SourceTypes::http :
    return parser->getCommand();

    //TODO add code for socket
  }
}

void DrawingJob::drawLine(void)
{

}

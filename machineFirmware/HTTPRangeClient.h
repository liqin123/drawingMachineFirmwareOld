/*  Class to download an HTTP document using byte range requests
*/

#ifndef HTTPRangeClient_h
#define HTTPRangeClient_h
#include <Arduino.h>
#include <ESP8266HTTPClient.h>

class HTTPRangeClient
{
  public:
    HTTPRangeClient();
    int connect(String url);
    void update();
    bool available();
    char getChar();

  private:
    int fillBuffer(int buf, int start);

    enum bufState {empty, partial, full};
    enum bufLast {last, notLast};

    bool connected;
    int numberOfChunks;
    bool lastChunkPartial;
    int chunksDownloaded;

    HTTPClient http;
    String url;
    int contentLength;
    String contentLastModified;
    String contentAcceptRanges;
    int maxRetries;
    int retryDelay;

    String buffers[2];
    String buffer0;
    String buffer1;
    bufState bufferState[2];
    bufLast bufferIsLast[2];
    int bufContains[2];
    int bufferSize;
    int currentBuffer;
    int bufferCurrentPosition;
};

#endif

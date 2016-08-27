/*  Class to download an HTTP document using byte range requests
*/

#include <Arduino.h>
#include "HTTPRangeClient.h"

HTTPRangeClient::HTTPRangeClient()
{
  state = HTTPState::disconnected;
  bufferSize = 2048;
  maxRetries = 5;
  retryDelay = 100;
}

/*
  Used to setup a conection. The function does an initial request to check that
  the site supports byte range and returns false if it doesn't.
*/
bool HTTPRangeClient::begin(String urlName)
{
  currentBuffer = 0;
  bufferCurrentPosition = 0;
  chunksDownloaded = 0;
  buffers[0] = buffer0;
  buffers[1] = buffer1;
  bufferState[0] = empty;
  bufferState[1] = empty;
  state = HTTPState::disconnected;

  url = urlName;

  const char * headerKeys[] = {"Last-Modified", "Accept-Ranges", "Content-Length", "Content-Range"} ;
  size_t headerKeysSize = sizeof(headerKeys)/sizeof(char*);

  http.begin(url);
  http.addHeader("Range", "bytes=0-0");
  http.collectHeaders(headerKeys, headerKeysSize);

  bool success = false;
  int retryCount;
  for(retryCount = 0; retryCount < maxRetries; retryCount++)
  {
    httpCode = http.GET();
    if(httpCode == HTTP_CODE_PARTIAL_CONTENT)
    {
      //Serial.print("!");
      success = true;
      break;
    }
    Serial.printf("\n===>Connect got %d, retrying(%d)\n", httpCode, retryCount);
    delay(retryDelay * 2 ^ retryCount);
  }

  if(!success)
  {
    return false;
  }
  // Store useful info from headers
  String s = http.header("Content-Range");
  contentLength = s.substring(s.indexOf("/") + 1).toInt();
  //Serial.printf("File length: %d\n", contentLength);
  contentLastModified = http.header("Last-Modified");
  contentAcceptRanges = http.header("Accept-Ranges");
  http.end();

  int fullChunks = contentLength / bufferSize;
  if(fullChunks * bufferSize == contentLength)
  {
    lastChunkPartial = false;
    numberOfChunks = fullChunks;
    //.printf("%d chunks\n", numberOfChunks);
  } else {
    lastChunkPartial = true;
    numberOfChunks = fullChunks + 1;
    //Serial.printf("%d chunks, last one partial\n", numberOfChunks);
  }

  chunksDownloaded = 0;
  state = HTTPState::connected;
  update();
  return true;
}

void HTTPRangeClient::end(void)
{
  currentBuffer = 0;
  bufferCurrentPosition = 0;
  chunksDownloaded = 0;
  bufferState[0] = empty;
  bufferState[1] = empty;
  state = HTTPState::disconnected;
}

bool HTTPRangeClient::connected(void)
{
  if(state == HTTPState::connected)
  {
    return true;
  } else {
    return false;
  }
}

int HTTPRangeClient::GET(void)
{
  return httpCode;
}

int HTTPRangeClient::getSize(void)
{
  return contentLength;
}

String HTTPRangeClient::errorToString(int error)
{
  return HTTPClient::errorToString(error);
}

void HTTPRangeClient::update()
{
  if(state != HTTPState::connected)
  {
    return;
  }

  for(int i = 0; i < 2; i++)
  {
    if(bufferState[i] == empty)
    {
      if(chunksDownloaded < numberOfChunks)
      {
        fillBuffer(i, chunksDownloaded * bufferSize);
        bufContains[i] = chunksDownloaded++;
        if(chunksDownloaded == numberOfChunks)
        {
          if(lastChunkPartial)
          {
            bufferState[i] = partial;
          } else {
            bufferState[i] = full;
          }
        }
      }
    }
  }
}

int HTTPRangeClient::fillBuffer(int buf, int start)
{
  const char * headerKeys[] = {"Last-Modified", "Content-Length"} ;
  size_t headerKeysSize = sizeof(headerKeys)/sizeof(char*);

  http.begin(url);
  http.addHeader("Range", String("bytes=") + String(start) + String("-") + String(start + bufferSize - 1));
  http.collectHeaders(headerKeys, headerKeysSize);

  bool success = false;
  int retryCount;
  for(retryCount = 0; retryCount < maxRetries; retryCount++)
  {
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_PARTIAL_CONTENT)
    {
      //Serial.print("!");
      success = true;
      break;
    }
    //Serial.printf("\n===>Got %d, retrying(%d)\n", httpCode, retryCount);
    delay(retryDelay * 2 ^ retryCount);
  }
  if(success)
  {
    buffers[buf] = http.getString();
    bufferState[buf] = full;
    http.end();
    return 0;
  } else {
    state = HTTPState::error;
    return httpCode;
  }
}

bool HTTPRangeClient::available()
{
  if(bufContains[currentBuffer] == (numberOfChunks - 1)) // we're on the last buffer
  {
    if(bufferCurrentPosition >= buffers[currentBuffer].length())
    {
      return false;
    }
  }
  return true;
}

char HTTPRangeClient::getChar()
{
  if(state == HTTPState::error || state == HTTPState::eof)
  {
    return 0;
  }

  if(bufferState[currentBuffer] != empty && bufferCurrentPosition < buffers[currentBuffer].length())
  {
    return buffers[currentBuffer].charAt(bufferCurrentPosition++);  // data left in current buffer
  } else {
    //Serial.print("X")
    if(bufContains[currentBuffer] == (numberOfChunks - 1))            // finished last chunk
    {
      state = HTTPState::eof;
      //Serial.printf("\nFinished last chunk: %d\n", numberOfChunks);
      return 0;
    }
    bufferState[currentBuffer] = empty;
    bufferCurrentPosition = 0;
    //Serial.printf("\nBuffer %d empty, switching\n", currentBuffer);
    if(++currentBuffer == 2)
    {
      currentBuffer = 0;
    }
    if(bufferState[currentBuffer] == empty)
    {
      update();
    }
    //bufferCurrentPosition++;
    return buffers[currentBuffer].charAt(bufferCurrentPosition++);
  }
}

HTTPState HTTPRangeClient::status()
{
  return state;
}

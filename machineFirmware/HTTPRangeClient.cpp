/*  Class to download an HTTP document using byte range requests
*/

#include <Arduino.h>
#include "HTTPRangeClient.h"

HTTPRangeClient::HTTPRangeClient()
{
  connected = false;
  bufferSize = 4096;
  maxRetries = 5;
  retryDelay = 100;
  currentBuffer = 0;
  bufferCurrentPosition = 0;
  buffers[0] = buffer0;
  buffers[1] = buffer1;
  bufferState[0] = empty;
  bufferState[1] = empty;
}

/*
  Used to setup a conection. The function does an initial request to check that
  the site supports byte range and returns false if it doesn't.
*/
int HTTPRangeClient::connect(String urlName)
{
  currentBuffer = 0;
  bufferCurrentPosition = 0;
  chunksDownloaded = 0;
  bufferState[0] = empty;
  bufferState[1] = empty;
  connected = false;

  url = urlName;

  const char * headerKeys[] = {"Last-Modified", "Accept-Ranges", "Content-Length", "Content-Range"} ;
  size_t headerKeysSize = sizeof(headerKeys)/sizeof(char*);

  http.begin(url);
  http.addHeader("Range", "bytes=0-0");
  http.collectHeaders(headerKeys, headerKeysSize);
  // int httpCode = http.GET();
  // if(httpCode != HTTP_CODE_PARTIAL_CONTENT)
  // {
  //   Serial.printf("Couldn't connect: %d", httpCode);
  //   return 0;
  // }

  int retryCount;
  for(retryCount = 0; retryCount < maxRetries; retryCount++)
  {
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_PARTIAL_CONTENT)
    {
      //Serial.print("!");
      break;
    }
    Serial.printf("\n===>Connect got %d, retrying(%d)\n", httpCode, retryCount);
    delay(retryDelay * 2 ^ retryCount);
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
  connected = true;
  update();
  return contentLength;
}

void HTTPRangeClient::update()
{
  if(! connected)
  {
    return;
  }

  for(int i = 0; i < 2; i++)
  {
    //Serial.printf("\nBuffer %d is %d\n", i, bufferState[i]);
    if(bufferState[i] == empty)
    {
      if(chunksDownloaded < numberOfChunks)
      {
        //Serial.printf("\nFilling buffer %d with chunk %d\n", i, chunksDownloaded);
        fillBuffer(i, chunksDownloaded * bufferSize);
        bufContains[i] = chunksDownloaded;
        chunksDownloaded++;
        if(chunksDownloaded == numberOfChunks)
        {
          if(lastChunkPartial)
          {
            bufferState[i] = partial;
            //Serial.println("Buffer is partial");
          } else {
            bufferState[i] = full;
          }
        }
      }
    } else {
      //Serial.print("#");
      //return;
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

  int retryCount;
  for(retryCount = 0; retryCount < maxRetries; retryCount++)
  {
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_PARTIAL_CONTENT)
    {
      //Serial.print("!");
      break;
    }
    Serial.printf("\n===>Got %d, retrying(%d)\n", httpCode, retryCount);
    delay(retryDelay * 2 ^ retryCount);
  }

  if(http.header("Content-Length").toInt() != bufferSize)
  {
    //Serial.println("Partial Buffer");
  }
  buffers[buf] = http.getString();
  bufferState[buf] = full;
  http.end();
  return 1;
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
  if(bufferCurrentPosition < buffers[currentBuffer].length())
  {
    return buffers[currentBuffer].charAt(bufferCurrentPosition++);  // data left in current buffer
  } else {
    //Serial.print("X")
    if(bufContains[currentBuffer] == (numberOfChunks - 1))            // finished last chunk
    {
      connected = false;
      Serial.printf("\nFinished last chunk: %d\n", numberOfChunks);
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

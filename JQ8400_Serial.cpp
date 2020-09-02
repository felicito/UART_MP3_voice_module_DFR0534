/** 
 * Arduino Library for JQ8400 MP3 Module
 * 
 * Copyright (C) 2019 James Sleeman, <http://sparks.gogo.co.nz/jq6500/index.html>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * @author James Sleeman, http://sparks.gogo.co.nz/
 * @license MIT License
 * @file
 */

#include "mbed.h"
#include "JQ8400_Serial.hpp"

void  JQ8400_Serial::play() {
  this->sendCommand(MP3_CMD_PLAY);
}


void  JQ8400_Serial::restart() {
  this->sendCommand(MP3_CMD_STOP); // Make sure really will restart
  this->sendCommand(MP3_CMD_PLAY);
}


void  JQ8400_Serial::pause() {
  this->sendCommand(MP3_CMD_PAUSE);
}


void  JQ8400_Serial::stop() {
  this->sendCommand(MP3_CMD_STOP);
}


void  JQ8400_Serial::next() {
  this->sendCommand(MP3_CMD_NEXT);
}


void  JQ8400_Serial::prev() {
  this->sendCommand(MP3_CMD_PREV);
}


void  JQ8400_Serial::playFileByIndexNumber(int fileNumber) {  
  // this->sendCommand(MP3_CMD_PLAY_IDX, (fileNumber>>8) & 0xFF, fileNumber & (int)0xFF);
  this->sendCommand(MP3_CMD_PLAY_IDX, fileNumber);
}


void  JQ8400_Serial::interjectFileByIndexNumber(int fileNumber) {  
  int buf[3] = { getSource(), (int)((fileNumber>>8)&0xFF), (int)(fileNumber & (int)0xFF) };
  this->sendCommandData(MP3_CMD_INSERT_IDX, buf, 3, 0, 0);
}


void  JQ8400_Serial::seekFileByIndexNumber(int fileNumber) {  
  // this->sendCommand(MP3_CMD_SEEK_IDX, (fileNumber>>8) & 0xFF, fileNumber & (int)0xFF);
  this->sendCommand(MP3_CMD_SEEK_IDX, fileNumber);
}


void JQ8400_Serial::abLoopPlay(int secondsStart, int secondsEnd) {
  int buf[4] = { (int)(secondsStart / 60), (int)(secondsStart % 60), (int)(secondsEnd / 60), (int)(secondsEnd % 60) };
  this->sendCommandData(MP3_CMD_AB_PLAY, buf, sizeof(buf), 0, 0);
}


void JQ8400_Serial::abLoopClear() {
  this->sendCommand(MP3_CMD_AB_PLAY_STOP);
}


void JQ8400_Serial::fastForward(int seconds) {
  //this->sendCommand(MP3_CMD_FFWD, (seconds>>8)&0xFF, seconds&0xFF);
  this->sendCommand(MP3_CMD_FFWD, seconds);
}


void JQ8400_Serial::rewind(int seconds) {
  //this->sendCommand(MP3_CMD_RWND, (seconds>>8)&0xFF, seconds&0xFF);
  this->sendCommand(MP3_CMD_RWND, seconds);
}


void  JQ8400_Serial::nextFolder() {
  this->sendCommand(MP3_CMD_NEXT_FOLDER);
}


void  JQ8400_Serial::prevFolder() {
  this->sendCommand(MP3_CMD_PREV_FOLDER);
}


void  JQ8400_Serial::playFileNumberInFolderNumber(int folderNumber, int fileNumber) {
  // This is kinda weird, the wildcard is *REQUIRED*, without it, it WILL NOT find the file you want.
  //
  // Really Weird.  Anyway this is the format for the data
  //  the first int is the source (as the int value not ascii number)
  //  then slash separated path components, with a trailing wildcard fvor each one REQUIRED
  //  the basename of the file must have the wildcard also and the extention must be the 
  //  3 question mark character wildcards, you can't even match on ".mp3", damn this is weird
  
  char buf[] = " /42*/032*???";
  buf[0] = this->getSource();
  
  int i = 2; // 1st digit folder component
  if(folderNumber<10) {
    buf[i] = '0';
    i++;
  }
  itoa(folderNumber,&buf[i], 10);
  
  i = 4;
  buf[i] = '*'; // itoa clobbered this with it's null
  
  i = 6; // 1st digit filename component
  if(fileNumber < 100)
  {
    buf[i] = '0';
    i++;
  }
  
  if(fileNumber < 10) {
      buf[i] = '0';
      i++;
  }
  itoa(fileNumber, &buf[i], 10);
  
  buf[9] = '*'; // itoa clobbered this with it's null
  
  this->sendCommandData(MP3_CMD_PLAY_FILE_FOLDER, (int*)buf, sizeof(buf)-1, 0, 0);
}


void  JQ8400_Serial::playInFolderNumber(int folderNumber) {
  char buf[] = " /42*/*???";
  
  buf[0] = this->getSource();
  
  int i = 2; // 1st digit folder component
  if(folderNumber<10)
  {
    buf[i] = '0';
    i++;
  }
  itoa(folderNumber,&buf[i], 10);
  
  i = 4;
  buf[i] = '*'; // itoa clobbered this with it's null
  
  this->sendCommandData(MP3_CMD_PLAY_FILE_FOLDER, (int*)buf, sizeof(buf)-1, 0, 0);
}


void JQ8400_Serial::playSequenceByFileNumber(int playList[], int listLength) {
  char buf[listLength*2+1]; // itoa will need an extra null
  
  int i = 0;
  for(int x = 0; x < listLength; x++)
  {
    if(playList[x]<10)
    {
      buf[i++] = '0';
    }
    itoa(playList[x], &buf[i++], 10);
  }
  
  this->sendCommandData(MP3_CMD_PLAYLIST, (int *)buf, sizeof(buf)-1, 0, 0);
}


void JQ8400_Serial::playSequenceByFileName(const char * playList[], int listLength) {
  char buf[listLength*2];
  
  int i = 0;
  for(int x = 0; x < listLength; x++)
  {
    buf[i++] = playList[x][0];
    buf[i++] = playList[x][1];
  }
  
  this->sendCommandData(MP3_CMD_PLAYLIST, (int *)buf, sizeof(buf), 0, 0);
}


void  JQ8400_Serial::volumeUp() {
  if(currentVolume < 30) currentVolume++;
  this->sendCommand(MP3_CMD_VOL_UP); // We still send the command just in case we got out of sync somehow
}


void  JQ8400_Serial::volumeDn() {
  if(currentVolume > 0 ) currentVolume--;
  this->sendCommand(MP3_CMD_VOL_DN); // We still send the command just in case we got out of sync somehow
}


void  JQ8400_Serial::setVolume(int volumeFrom0To30) {
  currentVolume = volumeFrom0To30;
  this->sendCommand(MP3_CMD_VOL_SET, volumeFrom0To30);
}


void  JQ8400_Serial::setEqualizer(int equalizerMode) {
  currentEq = equalizerMode;
  this->sendCommand(MP3_CMD_EQ_SET, equalizerMode);
}

void  JQ8400_Serial::setLoopMode(int loopMode) {
  currentLoop = loopMode;
  this->sendCommand(MP3_CMD_LOOP_SET, loopMode);
}


int JQ8400_Serial::getAvailableSources()  {
  return this->sendCommandWithintResponse(MP3_CMD_GET_SOURCES);
}


void  JQ8400_Serial::setSource(int source) {
  this->sendCommand(MP3_CMD_SOURCE_SET, source);
}


int JQ8400_Serial::getSource()  {
  return this->sendCommandWithintResponse(MP3_CMD_GET_SOURCE);
}


void  JQ8400_Serial::sleep() {
  this->sendCommand(MP3_CMD_SLEEP);
  this->sendCommand(MP3_CMD_STOP);
}


void  JQ8400_Serial::reset() {
  int retry = 5; // Try really hard to make ourselves heard.
  do {
    // The datasheet defined two stop commands but has no reset command
    //  I have elected to make what looks more like "universal stop" 0x10
    //  to be stop, and have defined for sake of convenience the other stop
    //  command as "RESET", we will issue both to be sure and then 
    //  set things back to "defaults", in absense of an actual reset
    
    this->sendCommand(MP3_CMD_STOP);  wait_ms(1000); // There seems to be something
    this->sendCommand(MP3_CMD_RESET); wait_ms(1000); //  related to timing here
        
    // Reset to the startup defaults
    this->setVolume(20);
    this->setEqualizer(0);
    this->setLoopMode(2);
    this->seekFileByIndexNumber(1);
    this->sendCommand(MP3_CMD_STOP);
    
    int timeout = 9;
    while(timeout-- > 0 ) {
      if(getAvailableSources()) {
        retry = 0;
        break; 
      }
      wait_ms(1000);
    }
  }
  while(retry-- > 0);
}


int JQ8400_Serial::getStatus() {
      if(MP3_STATUS_CHECKS_IN_AGREEMENT <= 1) {
        return this->sendCommandWithintResponse(MP3_CMD_STATUS); 
      }
      
      int statTotal = 0;
      int stat      = 0;
      do {
        statTotal = 0;
        for(int x = 0; x < MP3_STATUS_CHECKS_IN_AGREEMENT; x++) {
          stat = this->sendCommandWithintResponse(MP3_CMD_STATUS);      
          if(stat == 0) return 0; // STOP is fairly reliable
          statTotal += stat;
        }
      } while (statTotal != 1 * MP3_STATUS_CHECKS_IN_AGREEMENT && statTotal != 2 * MP3_STATUS_CHECKS_IN_AGREEMENT);
      
  return statTotal / MP3_STATUS_CHECKS_IN_AGREEMENT;      
}


int  JQ8400_Serial::getVolume() { 
  return currentVolume; 
}


int  JQ8400_Serial::getEqualizer() { 
  return currentEq;
}


int  JQ8400_Serial::getLoopMode() {
  return currentLoop;
}


int  JQ8400_Serial::countFiles() {
  return this->sendCommandWithUnsignedIntResponse(MP3_CMD_COUNT_FILES); 
}


int  JQ8400_Serial::currentFileIndexNumber() {
  return this->sendCommandWithUnsignedIntResponse(MP3_CMD_CURRENT_FILE_IDX); 
}


int  JQ8400_Serial::currentFilePositionInSeconds() {
  int buf[3];
  // This turns on continuous position reporting, every second
  this->sendCommandData(MP3_CMD_CURRENT_FILE_POS, 0, 0, buf, 3);
  // Stop it doing that
  this->sendCommand(MP3_CMD_CURRENT_FILE_POS_STOP);
  return (buf[0]*60*60) + (buf[1]*60) + buf[2];
}


int JQ8400_Serial::currentFileLengthInSeconds() {
  int buf[3];
  this->sendCommandData(MP3_CMD_CURRENT_FILE_LEN, 0, 0, buf, 3);
  return (buf[0]*60*60) + (buf[1]*60) + buf[2];
  return 0; /* FIXME this->sendCommandWithUnsignedIntResponse(MP3_CMD_CURRENT_FILE_LEN_SEC); */ 
}


void JQ8400_Serial::currentFileName(char *buffer, int bufferLength) {
  this->sendCommand(MP3_CMD_CURRENT_FILE_NAME, (int *)buffer, bufferLength);
  buffer[bufferLength-1] = 0; // Ensure null termination since this is a string.
}


int JQ8400_Serial::sendCommandWithUnsignedIntResponse(int command) {      
  int buffer[4];
  this->sendCommand(command, buffer, sizeof(buffer));
  return ((int)buffer[0]<<8) | ((int)buffer[1]);
}


int JQ8400_Serial::sendCommandWithintResponse(int command) {
  int response = 0;
  this->sendCommand(command, &response, 1);
  return response;
}


void  JQ8400_Serial::sendCommandData(int command, int *requestBuffer, int requestLength, int *responseBuffer, int bufferLength) {
      // Calculate the checksum which forms the end int
  int MP3_CHECKSUM = MP3_CMD_BEGIN + command + requestLength;
  for(int x = 0; x < requestLength; x++) {
    MP3_CHECKSUM += (int)requestBuffer[x];
  }
  
  // If there is any random garbage on the line, clear that out now.
  while(this->waitUntilAvailable(10)) this->_Serial->read();

  this->_Serial->putc(MP3_CMD_BEGIN);
  this->_Serial->putc(command);
  this->_Serial->putc(requestLength);
  for(int x = 0; x < requestLength; x++) {
    this->_Serial->write(requestBuffer[x]);
  }
  this->_Serial->putc(MP3_CHECKSUM);
            
  if(responseBuffer && bufferLength) {
    memset(responseBuffer, 0, bufferLength);
  } else { // If we don't expect a response (or don't care) don't wait for ones
    return;
  }
      
  // Allow some time for the device to process what we did and 
  // respond, up to 1 second, but typically only a few ms.
  this->waitUntilAvailable(1000);
            
  // The response format is the same as the command format
  //  AA [CMD] [DATA_COUNT] [B1..N] [SUM]
  MP3_CHECKSUM = 0;
      
  int      i = 0;
  int      j = 0;
  int      dataCount = 0;
  while(this->waitUntilAvailable(150)) {
    j = this->_Serial->read();
    if(i == 2) {
      // The number of data ints to read
      dataCount = j;
    }
        
    // We only record the data ints so ints 0,1 and 2 are discarded
    //   except for calculating checksum
    if(i <= 2) {
      MP3_CHECKSUM += j;
      i++;
      continue;
    } else {
      if(dataCount > 0) {
        // This is a dataint to read
        if((i-3) <= (bufferLength-1)) {
          responseBuffer[i-3] = j;
        }
        i++;
        dataCount--;
        MP3_CHECKSUM += j;
      } else {
        // This is the checksum int
        if((MP3_CHECKSUM & 0xFF) != j) {
          // Checksum Failed
          /* #if MP3_DEBUG
              Serial.print(" ** CHECKSUM FAILED " );
              HEX_PRINT((MP3_CHECKSUM & 0xFF)); 
              Serial.print(" != ");
              HEX_PRINT(j); 
          #endif */
          memset(responseBuffer, 0, bufferLength);
        } else {
               /* #if MP3_DEBUG
                Serial.print(" ** CHECKSUM OK " );
                HEX_PRINT((MP3_CHECKSUM & 0xFF)); 
                Serial.print(" == ");
                HEX_PRINT(j); 
              #endif */
        }
      }
    }
  }      
}
    

// Waits until data becomes available, or a timeout occurs
int JQ8400_Serial::waitUntilAvailable(int maxWaitTime)
{
  int c = 0;
  Timer startTime;
  startTime.start();
  do {
    c = this->_Serial->available();
    if (c) {
      break;
      startTime.stop()
    }
  } while(startTime.read() < maxWaitTime);
  return c;
}

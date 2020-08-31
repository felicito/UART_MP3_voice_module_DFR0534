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

#ifndef JQ8400_h
#define JQ8400_h

    #define MP3_EQ_NORMAL           0
    #define MP3_EQ_POP              1
    #define MP3_EQ_ROCK             2
    #define MP3_EQ_JAZZ             3
    #define MP3_EQ_CLASSIC          4
    #define MP3_SRC_USB             0
    #define MP3_SRC_SDCARD          1
    #define MP3_SRC_FLASH           2
    #define MP3_SRC_BUILTIN         MP3_SRC_FLASH
    #define MP3_LOOP_ALL            0       // ALL = all tracks loop, ALL_STOP all tracks then stop, ALL_RANDOM all tracks randomly
    #define MP3_LOOP_ALL_STOP       7   
    #define MP3_LOOP_ALL_RANDOM     3
    #define MP3_LOOP_ONE            1       // ONE = one track loop, ONE_STOP (default) one track then stop
    #define MP3_LOOP_ONE_STOP       2
    #define MP3_LOOP_FOLDER         4       // FOLDER = tracks within folder loop, FOLDER_STOP tracks within folder then stop, FOLDER_RANDOM random in folder
    #define MP3_LOOP_FOLDER_RANDOM  5
    #define MP3_LOOP_FOLDER_STOP    6
    #define MP3_LOOP_NONE           2 
    #define MP3_STATUS_STOPPED      0
    #define MP3_STATUS_PLAYING      1
    #define MP3_STATUS_PAUSED       2
    #define MP3_STATUS_CHECKS_IN_AGREEMENT 1
    #define MP3_DEBUG               0
    #define HEX_PRINT(a) if(a < 16) Serial.print(0); Serial.print(a, HEX);

    class JQ8400_Serial : public Serial
    {        
    public: 
        JQ8400_Serial(PinName _tx, PinName _rx) : Serial(_tx,_rx)

        void    play();
        void    restart();
        void    pause();
        void    stop();
        void    fastForward(uint16_t seconds = 5);
        void    rewind(uint16_t seconds = 5);
        void    next();
        void    prev();
        void    nextFolder();
        void    prevFolder();
        void    playFileByIndexNumber(uint16_t fileNumber);        
        void    interjectFileByIndexNumber(uint16_t fileNumber);        
        void    playFileNumberInFolderNumber(uint16_t folderNumber, uint16_t fileNumber);
        void    playInFolderNumber(uint16_t folderNumber);
        void    seekFileByIndexNumber(uint16_t fileNumber);
        void    abLoopPlay(uint16_t secondsStart, uint16_t secondsEnd);
        void    abLoopClear();
        void    volumeUp();
        void    volumeDn();
        void    setVolume(byte volumeFrom0To30);
        void    setEqualizer(byte equalizerMode); // EQ_NORMAL to EQ_BASS
        void    setLoopMode(byte loopMode);
        void    setSource(byte source);
        uint8_t getSource();
        uint8_t sourceAvailable(uint8_t source) {
          return getAvailableSources() & 1<<source;
        }
        
        void    sleep();
        void    reset();
        byte    getStatus();
        uint8_t busy() { return getStatus() == MP3_STATUS_PLAYING; }
        byte    getVolume();
        byte    getEqualizer();
        byte    getLoopMode();
        uint16_t   countFiles();    
        uint16_t   currentFileIndexNumber();
        uint16_t   currentFilePositionInSeconds();
        uint16_t   currentFileLengthInSeconds();
        void       currentFileName(char *buffer, uint16_t bufferLength);    
        void    playSequenceByFileNumber(uint8_t playList[], uint8_t listLength);
        void    playSequenceByFileName(const char *playList[], uint8_t listLength);       
        
        
    protected:
        void        sendCommandData(uint8_t command, uint8_t *requestBuffer, uint8_t requestLength, uint8_t *responseBuffer, uint8_t bufferLength);
        inline void sendCommand(uint8_t command, uint8_t *responseBuffer = 0, uint8_t bufferLength = 0) { 
          sendCommandData(command, NULL,  0, responseBuffer, bufferLength);
        }

        inline void sendCommand(uint8_t command, uint8_t arg, uint8_t *responseBuffer = 0, uint8_t bufferLength = 0) { 
        sendCommandData(command, &arg, 1, responseBuffer, bufferLength); 
        }
        
        inline void sendCommand(uint8_t command, uint16_t arg, uint8_t *responseBuffer = 0, uint8_t bufferLength = 0) { 
          #if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__    
              sendCommandData(command, ((uint8_t *)(&arg)), 2, responseBuffer, bufferLength);
          #else
              uint8_t buf[] = { *(((uint8_t *)(&arg))+1), *((uint8_t *)(&arg)) };
              sendCommandData(command, buf, 2, responseBuffer, bufferLength);
          #endif
        }
            
        uint16_t  sendCommandWithUnsignedIntResponse(byte command);        
        uint8_t   sendCommandWithByteResponse(uint8_t command);
        uint8_t   getAvailableSources();
        int       waitUntilAvailable(uint16_t maxWaitTime = 1000);
        uint8_t   currentVolume = 20; ///< Record of current volume level (JQ8400 has no way to query)
        uint8_t   currentEq     = 0;  ///< Record of current equalizer (JQ8400 has no way to query)
        uint8_t   currentLoop   = 2;  ///< Record of current loop mode (JQ8400 has no way to query)

        static const uint8_t MP3_CMD_BEGIN                    = 0xAA;        
        static const uint8_t MP3_CMD_PLAY                     = 0x02;
        static const uint8_t MP3_CMD_PAUSE                    = 0x03;
        static const uint8_t MP3_CMD_FFWD                     = 0x23;
        static const uint8_t MP3_CMD_RWND                     = 0x22;            
        static const uint8_t MP3_CMD_STOP                     = 0x10; // Not sure, maybe 0x04?
        static const uint8_t MP3_CMD_NEXT                     = 0x06;
        static const uint8_t MP3_CMD_PREV                     = 0x05;
        static const uint8_t MP3_CMD_PLAY_IDX                 = 0x07;
        static const uint8_t MP3_CMD_SEEK_IDX                 = 0x1F;
        static const uint8_t MP3_CMD_INSERT_IDX               = 0x16;
        static const uint8_t MP3_CMD_AB_PLAY                  = 0x20;
        static const uint8_t MP3_CMD_AB_PLAY_STOP             = 0x21;
        static const uint8_t MP3_CMD_NEXT_FOLDER              = 0x0F;
        static const uint8_t MP3_CMD_PREV_FOLDER              = 0x0E;
        static const uint8_t MP3_CMD_PLAY_FILE_FOLDER         = 0x08;
        static const uint8_t MP3_CMD_VOL_UP                   = 0x14;
        static const uint8_t MP3_CMD_VOL_DN                   = 0x15;
        static const uint8_t MP3_CMD_VOL_SET                  = 0x13;
        static const uint8_t MP3_CMD_EQ_SET                   = 0x1A;
        static const uint8_t MP3_CMD_LOOP_SET                 = 0x18;    
        static const uint8_t MP3_CMD_SOURCE_SET               = 0x0B;
        static const uint8_t MP3_CMD_SLEEP                    = 0x04;    // I am not sure about these, see implmentation of sleep() and reset()
        static const uint8_t MP3_CMD_RESET                    = 0x04;    //  what I have done seems to work maybe, maybe.
        static const uint8_t MP3_CMD_STATUS                   = 0x01;
        static const uint8_t MP3_CMD_GET_SOURCES              = 0x09;
        static const uint8_t MP3_CMD_GET_SOURCE               = 0x0A;
        static const uint8_t MP3_CMD_COUNT_FILES              = 0x0C; 
        static const uint8_t MP3_CMD_COUNT_IN_FOLDER          = 0x12; 
        static const uint8_t MP3_CMD_CURRENT_FILE_IDX         = 0x0D; 
        static const uint8_t MP3_CMD_FIRST_FILE_IN_FOLDER_IDX = 0x11; 
        static const uint8_t MP3_CMD_CURRENT_FILE_LEN         = 0x24;
        static const uint8_t MP3_CMD_CURRENT_FILE_POS         = 0x25; // This turns on continuous reporting of position
        static const uint8_t MP3_CMD_CURRENT_FILE_POS_STOP    = 0x26; // This stops that
        static const uint8_t MP3_CMD_CURRENT_FILE_NAME        = 0x1E;
        static const uint8_t MP3_CMD_PLAYLIST                 = 0x1B;
    };

#endif //JQ8400_h
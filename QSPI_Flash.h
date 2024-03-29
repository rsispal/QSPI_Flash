#ifndef   _QSPIFLASHMEMORY_H
#define   _QSPIFLASHMEMORY_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>
#include <Adafruit_QSPI_GD25Q.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_QSPI.h>
#include "Path.h"

/*
Debug Levels (CREATE CONSTANTS)
    0 = None
    1 = Minimal
    2 = Show warnings
    3 = Show intermediate values
    9 = Show extended debug output
*/

class QSPIFlashMemory {

    public:
        Path path;

        uint8_t manufacturerID;
        uint8_t deviceID;
        uint16_t pageCount;
        uint16_t pageSize;
        uint32_t chipModelID;
        uint32_t chipAddress;

        uint8_t getManufacturerID();
        uint8_t getDeviceID();
        uint16_t getFlashPages();
        uint16_t getFlashPageSize();
        uint32_t getFlashChipID();
        uint32_t getFlashChipAddress();
        int8_t initialise();
        int8_t initialise(int8_t debugLevel);
        bool checkIfFlashMemoryIsReady();
        int8_t setDebugLevel(int8_t debugLevel);
        int8_t getDebugLevel();
        Adafruit_QSPI_GD25Q getFlashQSPIInterface();
        Adafruit_W25Q16BV_FatFs getFlashFileSystemInterface();
        int format();
        File getFilesInDirectory(char directory[]);
        bool checkFileExists(char directory[], char filename[]);
        bool checkDirectoryExists(char directory[]);
        File getFile(char directory[], char filename[]);
        int createDirectory(char directory[]);
        int createFile(char directory[], char filename[]);
        int saveFile(char directory[], char filename[], char content[], bool overwriteExistingContent);




        int appendToFile(char directory[], char filename[], char content[]);
        int appendToFile(char directory[], char filename[], int content[], int contentLength, bool writeLiterally);
        int appendToFile(char directory[], char filename[], int content, bool writeLiterally);
        int appendToFile(char directory[], char filename[], char content);






        int getFilesize(char directory[], char filename[]);
        int readFileContents(char directory[], char filename[], uint8_t fileContent[], long maxReadSize);
        int deleteFile(char directory[], char filename[]);
        int deleteDirectory(char directory[]);
    private:
        int _debugLevel = 0;
        char resolvedPath[260];
};

#endif // _QSPIFLASHMEMORY_H

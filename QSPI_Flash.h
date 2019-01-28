#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_SPIFlash_FatFs.h>
#include <Adafruit_QSPI_GD25Q.h>
#include <Adafruit_SPIFlash.h>
#include <Adafruit_QSPI.h>

#include <string.h>



#ifndef   _QSPIFLASHMEMORY_H
#define   _QSPIFLASHMEMORY_H

/*
Debug Levels (CREATE ENUMS)
0 = None
1 = Minimal
2 = Show warnings
3 = Show intermediate values
9 = Show extended debug output
*/



class QSPIFlashMemory {

    public:
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
        int initialise();
        int initialise(int debugLevel);
        bool checkIfFlashMemoryIsReady();
        int setDebugLevel(int debugLevel);
        int getDebugLevel(int debugLevel);
        Adafruit_QSPI_GD25Q getFlashQSPIInterface();
        Adafruit_W25Q16BV_FatFs getFlashFileSystemInterface();
        int format();
        File getFilesInDirectory(char directory[]);
        bool checkFileExists(char directory[], char filename[]);
        bool checkDirectoryExists(char directory[]);
        File getFile(char directory[], char filename[]);
        int createDirectory(char directory[]);
        int createFile(char directory[], char filename[]);
        bool saveFile(char directory[], char filename[], char content[], bool overwriteExistingContent);
        int getFilesize(char directory[], char filename[]);
        int readFileContents(char directory[], char filename[], char fileContent[], int maxReadSize);
        int deleteFile(char directory[], char filename[]);
        int deleteDirectory(char directory[]);

    private:

        int _debugLevel = 0;
        char resolvedPath[512] = { 0 };
};

#endif // _QSPIFLASHMEMORY_H

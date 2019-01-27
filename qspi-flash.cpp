#include "qspi-flash.h"

#define FLASH_TYPE    SPIFLASHTYPE_W25Q16BV  // Flash chip type.

Adafruit_QSPI_GD25Q flash;
Adafruit_W25Q16BV_FatFs fs(flash);

/*
Method: getFlashPages()
Description: Get the hardware manufacturer ID

Input: None
Output: uint8_t of manufacturer ID
*/
uint8_t FlashMemory::getManufacturerID() {
     flash.GetManufacturerInfo(&manufacturerID, &deviceID);
     return manufacturerID;
}



/*
Method: getDeviceID()
Description: Get the hardware device ID

Input: None
Output: uint8_t of device ID
*/
uint8_t FlashMemory::getDeviceID() {
    flash.GetManufacturerInfo(&manufacturerID, &deviceID);
    return deviceID;
}



/*
Method: getFlashPages()
Description: Get the page count from the hardware

Input: None
Output: uint16_t of page count
*/
uint16_t FlashMemory::getFlashPages() {
    return pageCount = flash.numPages();
}

/*
Method: getFlashPageSize()
Description: Get the size of the flash pages from the hardware

Input: None
Output: uint16_t of page size
*/
uint16_t FlashMemory::getFlashPageSize() {
    return pageSize = flash.pageSize();
}

/*
Method: getFlashChipID()
Description: Get the chip JEDECID ID from the hardware

Input: None
Output: uint32_t of JEDECID
*/
uint32_t FlashMemory::getFlashChipID() {
    return chipModelID = flash.GetJEDECID();
}


/*
Method: getFlashChipAddress()
Description: Get the bus address of the hardware

Input: None
Output: uint32_t of device bus address
*/
uint32_t FlashMemory::getFlashChipAddress() {
    return chipAddress = flash.getAddr();
}

/*
Method: initialise()
Description: Initialise with no debug enabled (default)

Input: None
Output: 0 = success
*/
int FlashMemory::initialise() {
    flash.setFlashType(FLASH_TYPE);
    _debugLevel = 0;

    if (checkIfFlashMemoryIsReady() == false) {
        return -1;
    }
    getManufacturerID();
    getDeviceID();
    pageCount = getFlashPages();
    pageSize = getFlashPageSize();
    chipModelID = getFlashChipID();
    chipAddress = getFlashChipAddress();
    return 0;
}

/*
Method: initialise()
Description: Initialise with a specific debug level

Input: debugLevel integer (0 - 254)
Output: 0 = success, -1 = chip not ready
*/
int FlashMemory::initialise(int debugLevel) {
    flash.setFlashType(FLASH_TYPE);
    if (debugLevel >= 0 && debugLevel < 255) {
        _debugLevel = debugLevel;
    }
    if (checkIfFlashMemoryIsReady() == false) {
        return -1;
    }
    pageCount = getFlashPages();
    pageSize = getFlashPageSize();
    chipModelID = getFlashChipID();
    chipAddress = getFlashChipAddress();
    return 0;
}

/*
Method: checkIfFlashMemoryIsReady()
Description:

Input: None
Output: true = chip ready, false = chip not ready
*/
bool FlashMemory::checkIfFlashMemoryIsReady() {
    // TODO: Add logic to check 5 times then return false if still unavailable
    if (!flash.begin()) {
        if (_debugLevel >= 0 && _debugLevel < 255) { Serial.println("Could not find flash on QSPI bus!"); }
        return false;
    }
    return true;
}

/*
Method: setDebugLevel()
Description: Override existing debug level

Input: debugLevel integer (0 - 254)
Output: 0 = success, -1 = error
*/
int FlashMemory::setDebugLevel(int debugLevel) {
    if (debugLevel >= 0 && debugLevel < 255) {
        _debugLevel = debugLevel;
        return 0;
    }
    return -1;
}

/*
Method: getDebugLevel()
Description: Get the current debug level value

Input: None
Output: debug level integer
*/
int FlashMemory::getDebugLevel(int debugLevel) {
    return _debugLevel;
}

/*
Method: format()
Description: Format the flash memory into a single partition

Input: None
Output: 0 = success, -1/-2/-3 = error
*/
int FlashMemory::format() {
    if (_debugLevel > 0) { Serial.println("\n Formatting Flash Chip"); }

    fs.activate();

    // Partition the flash with 1 partition that takes the entire space.
    if (_debugLevel > 0) { Serial.println(" -> Partitioning flash with 1 primary partition using 100% available space"); }
    DWORD plist[] = { 100, 0, 0, 0 };  // 1 primary partition with 100% of space.
    uint8_t buf[512] = { 0 };          // Working buffer for f_fdisk function.
    FRESULT r = f_fdisk(0, plist, buf);
    if (r != FR_OK) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, f_fdisk failed with error code: "); Serial.println(r, DEC); }
        return -1;
    }
    if (_debugLevel > 0) { Serial.println(" -> Partitioned flash!"); }

    // Make filesystem.
    if (_debugLevel > 0) { Serial.println(" -> Making FAT file system (takes ~60s)"); }
    r = f_mkfs("", FM_ANY, 0, buf, sizeof(buf));
    if (r != FR_OK) {
        if (_debugLevel > 0) { Serial.print(" -> Error, f_mkfs failed with error code: "); Serial.println(r, DEC); }
        return -2;
    }
    if (_debugLevel > 0) { Serial.println(" -> Format complete"); }

    // Finally test that the filesystem can be mounted.
    if (_debugLevel > 0) { Serial.println(" -> Testing filesystem is functional"); }
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.println(" -> Error, failed to mount newly formatted filesystem!"); }
        return -3;
    }
    if (_debugLevel > 0) { Serial.println(" -> Filesystem available"); }
    if (_debugLevel > 0) { Serial.println(" -> Complete!\n"); }
    return 0;
}

/*
Method: getFilesInDirectory()
Description: Get the folder representation to access files

Input: directory char array
Output: File object for the directory, null = directory doesnt exist/error
*/
File FlashMemory::getFilesInDirectory(char directory[]) {
    if (checkDirectoryExists(directory) == false) {
        return NULL;
    }
    File dir = fs.open(directory);
    return dir;
}

/*
Method: checkFileExists()
Description: Check if a specific file exists in the queried directory

Input: directory char array, filename char array (with extension)
Output: true = exists, false = doesnt exist
*/
bool FlashMemory::checkFileExists(char directory[], char filename[]) {
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    return fs.exists(path) == true ? true : false;
}

/*
Method: checkDirectoryExists()
Description: Check if a specific directory exists in the queried directory path

Input: directory char array
Output: true = exists, false = doesnt exist
*/
bool FlashMemory::checkDirectoryExists(char directory[]) {
    strcat(path, directory);
    return fs.exists(path) == true ? true : false;
}

/*
Method: getFile()
Description: Get specific file by directory and filename

Input: directory char array, filename char array (with extension)
Output: File object
*/
File FlashMemory::getFile(char directory[], char filename[]) {
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File f = fs.open(path);
    return f;
}

/*
Method: createDirectory()
Description: Create an empty file in the specified directory with the specified name

Input: directory char array, filename char array (with extension)
Output: 0 = success, -1 = already exists, -2 = error creating
*/
int FlashMemory::createDirectory(char directory[]) {
    if (checkDirectoryExists(directory) == true) {
        return -1;
    }
    if (!fs.mkdir(directory)) {
        return -2;
    }
    return 0;
}

/*
Method: createFile()
Description: Create an empty file in the specified directory with the specified name

Input: directory char array, filename char array (with extension)
Output: 0 = success, -1 = already exists, -2 = error creating
*/
int FlashMemory::createFile(char directory[], char filename[]) {
    if (checkFileExists(directory, filename) == true) {
        return -1;
    }
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File cf = fs.open(path, FILE_WRITE);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("Error, failed to open file for writing"); }
        return -2;
    }
    cf.close();
    return 0;
}

/*
Method: saveFile()
Description: Save content to file

Input: directory char array, filename char array (with extension), content char array, boolean flag to override existing content
Output:  0 = success, -1 = file didnt exist and failed to create it,, -2 file already has content and user requested not to over write
*/
bool FlashMemory::saveFile(char directory[], char filename[], char content[], bool overwriteExistingContent) {
    if (checkFileExists(directory, filename) == false) {
        if (createFile(directory, filename) == false) {
           return -1;
        }
    }

    if (getFilesize(directory, filename) > 0) {
        if (overwriteExistingContent == false) {
            return -2;
        }
    }
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File wf = fs.open(path, FILE_WRITE);
    if (!wf) {
        Serial.println("Error, failed to open test.txt for writing!");
    }
    if (overwriteExistingContent == true) {
        wf.seek(0);
        wf.print(content);
    } else {
        wf.print(content);
    }

    // Close the file when finished writing.
    wf.close();
    return 0;
}

/*
Method: getFilesize()
Description: Get filesize of the file path provided

Input: directory char array, filename char array (with extension)
Output: 0 = success, -1 = Doesnt exist, -2 = error reading
*/
int FlashMemory::getFilesize(char directory[], char filename[]) {
    if (checkFileExists(directory, filename) == false) {
        return -1;
    }
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File cf = fs.open(path, FILE_READ);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("Error, failed to open file for reading"); }
        return -2;
    }
    return cf.size();
}

/*
Method: readFileContents()
Description: Read file content to provided fileContent array

Input: directory char array, filename char array (with extension), content char array, boolean flag to override existing content
Output: true = content read and stored in fileContent[] array, false = content not written
*/
int FlashMemory::readFileContents(char directory[], char filename[], char fileContent[], int maxReadSize) {
    if (checkFileExists(directory, filename) == false) {
        return -1;
    }
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File cf = fs.open(path, FILE_READ);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("Error, failed to open file for reading"); }
        return -2;
    }
    int i = 0;
    while (cf.available() && i < maxReadSize) {
        char c = cf.read();
        Serial.print(c);
        fileContent[i] = c;
        i++;
    }
    return 0;
}

/*
Method: deleteFile()
Description: Delete a file by its filename in the specified directory

Input: directory char array, filename char array (with extension)
Output: 0 = success, -1 = could not delete
*/
int FlashMemory::deleteFile(char directory[], char filename[]) {
  // Delete a file with the remove command.  For example create a test2.txt file
    // inside /test/foo and then delete it.
    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    File cf = fs.open(path, FILE_WRITE);
    cf.close();
    if (!fs.remove(path)) {
      Serial.println("Error, couldn't delete test.txt file!");
      return -1;
    }
    Serial.println("Deleted file!");
    return 0;
}

/*
Method: deleteDirectory()
Description: Delete directory and all sub-content

Input: directory char array
Output: 0 = success, -1 = could not delete, -2 = directory wasnt deleted
*/
int FlashMemory::deleteDirectory(char directory[]) {
    // Delete a directory with the rmdir command.  Be careful as
    // this will delete EVERYTHING in the directory at all levels!
    // I.e. this is like running a recursive delete, rm -rf, in
    // unix filesystems!

    if (!fs.rmdir(directory)) {
      Serial.println("Error, couldn't delete directory!");
      return -1;
    }
    // Check that test is really deleted.
    if (fs.exists(directory)) {
      Serial.println("Error, directory was not deleted!");
      return -2;
    }
    Serial.println("Directory was deleted!");
    return 0;
}

/*
Method: getFlashQSPIInterface()
Description: get the raw QSPI_GD25Q flash object

Input:
Output: Adafruit_QSPI_GD25Q object
*/
Adafruit_QSPI_GD25Q FlashMemory::getFlashQSPIInterface() {
    return flash;
}

/*
Method: getFlashFileSystemInterface()
Description: get the raw FatFs object for low-level controls

Input:
Output: Adafruit_W25Q16BV_FatFs object
*/
Adafruit_W25Q16BV_FatFs FlashMemory::getFlashFileSystemInterface() {
    return fs;
}

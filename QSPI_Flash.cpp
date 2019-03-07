#include <QSPI_Flash.h>
#define FLASH_TYPE    SPIFLASHTYPE_W25Q16BV  // Flash chip type.

Adafruit_QSPI_GD25Q flash;
Adafruit_W25Q16BV_FatFs fs(flash);


/*
Method: initialise()
Description: Initialise with no debug enabled (default)
Input: None
Output:
    0: success
*/
int QSPIFlashMemory::initialise() {
    flash.setFlashType(FLASH_TYPE);
    _debugLevel = 0;
    path.initialise(_debugLevel);
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
Output:
     0: success
    -1: chip not ready
*/
int QSPIFlashMemory::initialise(int debugLevel) {
    flash.setFlashType(FLASH_TYPE);
    if (debugLevel >= 0 && debugLevel < 255) {
        _debugLevel = debugLevel;
        path.initialise(debugLevel);
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
Method: setDebugLevel()
Description: Override existing debug level
Input:
    int debugLevel: Desired debug level integer (0 - 254) - See QSPI_Flash.h for details
Output:
     0: Value changed successfully
    -1: Illegal value
*/
int QSPIFlashMemory::setDebugLevel(int debugLevel) {
    if (debugLevel >= 0 && debugLevel < 255) {
        _debugLevel = debugLevel;
        path.initialise(_debugLevel);
        return 0;
    }
    return -1;
}

/*
Method: getDebugLevel()
Description: Get the current debug level value
Input: None
Output:
    int (0 - 254): Current debug level integer (0 - 254) - See QSPI_Flash.h for details
*/
int QSPIFlashMemory::getDebugLevel(int debugLevel) {
    return _debugLevel;
}

/*
Method: checkIfFlashMemoryIsReady()
Description:
Input: None
Output:
    true: chip ready
    false: chip not ready
*/
bool QSPIFlashMemory::checkIfFlashMemoryIsReady() {
    // TODO: Add logic to check 5 times then return false if still unavailable
    if (!flash.begin()) {
        if (_debugLevel > 0 && _debugLevel < 255) { Serial.println("\nCould not find flash on QSPI bus!"); }
        return false;
    }
    return true;
}

/*
Method: getFlashPages()
Description: Get the hardware manufacturer ID
Input: None
Output:
    uint8_t of manufacturer ID
*/
uint8_t QSPIFlashMemory::getManufacturerID() {
     flash.GetManufacturerInfo(&manufacturerID, &deviceID);
     return manufacturerID;
}

/*
Method: getDeviceID()
Description: Get the hardware device ID
Input: None
Output:
    uint8_t of device ID
*/
uint8_t QSPIFlashMemory::getDeviceID() {
    flash.GetManufacturerInfo(&manufacturerID, &deviceID);
    return deviceID;
}

/*
Method: getFlashPages()
Description: Get the page count from the hardware
Input: None
Output:
    uint16_t of page count
*/
uint16_t QSPIFlashMemory::getFlashPages() {
    return pageCount = flash.numPages();
}

/*
Method: getFlashPageSize()
Description: Get the size of the flash pages from the hardware
Input: None
Output:
    uint16_t of page size
*/
uint16_t QSPIFlashMemory::getFlashPageSize() {
    return pageSize = flash.pageSize();
}

/*
Method: getFlashChipID()
Description: Get the chip JEDECID ID from the hardware
Input: None
Output:
    uint32_t of JEDECID
*/
uint32_t QSPIFlashMemory::getFlashChipID() {
    return chipModelID = flash.GetJEDECID();
}


/*
Method: getFlashChipAddress()
Description: Get the bus address of the hardware
Input: None
Output:
    uint32_t of device bus address
*/
uint32_t QSPIFlashMemory::getFlashChipAddress() {
    return chipAddress = flash.getAddr();
}

/*
Method: format()
Description: Format the flash memory into a single partition
Input: None
Output:
    0: success
    -1/-2/-3: error
@TODO: Explain properly what the error codes are
*/
int QSPIFlashMemory::format() {
    if (_debugLevel > 0) { Serial.print("\n\n Formatting Flash Chip"); }

    fs.activate();

    // Partition the flash with 1 partition that takes the entire space.
    if (_debugLevel > 0) { Serial.print("\n -> Partitioning flash with 1 primary partition using 100% available space"); }
    DWORD plist[] = { 100, 0, 0, 0 };  // 1 primary partition with 100% of space.
    uint8_t buf[512] = { 0 };          // Working buffer for f_fdisk function.
    FRESULT r = f_fdisk(0, plist, buf);
    if (r != FR_OK) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, f_fdisk failed with error code: "); Serial.print(r, DEC); }
        return -1;
    }
    if (_debugLevel > 0) { Serial.print("\n -> Partitioned flash!"); }

    // Make filesystem.
    if (_debugLevel > 0) { Serial.print("\n -> Making FAT file system (takes ~60s)"); }
    r = f_mkfs("", FM_ANY, 0, buf, sizeof(buf));
    if (r != FR_OK) {
        if (_debugLevel > 0) { Serial.print(" -> Error, f_mkfs failed with error code: "); Serial.print(r, DEC); }
        return -2;
    }
    if (_debugLevel > 0) { Serial.print("\n -> Format complete"); }

    // Finally test that the filesystem can be mounted.
    if (_debugLevel > 0) { Serial.print("\n -> Testing filesystem is functional"); }
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount newly formatted filesystem!"); }
        return -3;
    }
    if (_debugLevel > 0) { Serial.print("\n -> Filesystem available"); }
    if (_debugLevel > 0) { Serial.print("\n -> Complete!\n"); }
    return 0;
}

/*
Method: getFilesInDirectory()
Description: Get the folder representation to access files
Input:
    char directory[]: user-specified directory (leading /)
Output:
    NULL: Directory does not exist or an error occurred
    File: File object for the directory
*/
File QSPIFlashMemory::getFilesInDirectory(char directory[]) {
    if (checkDirectoryExists(directory) == false) {
        return NULL;
    }
    return fs.open(directory);
}

/*
Method: checkFileExists()
Description: Check if a specific file exists in the queried directory
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
Output:
    true: File exists
    false: File doesn't exist
*/
bool QSPIFlashMemory::checkFileExists(char directory[], char filename[]) {
    path.resolve(resolvedPath, directory, filename);
    if (fs.exists(resolvedPath)) {
        if (_debugLevel > 0) { Serial.print("\nQSPIFlashMemory::checkDirectoryExists - Exists"); }
        return true;
    }
    if (_debugLevel > 0) { Serial.print("\nQSPIFlashMemory::checkDirectoryExists - Doesnt Exist"); }
    return false;
}

/*
Method: checkDirectoryExists()
Description: Check if a specific directory exists in the queried directory path
Input:
    char directory[]: user-specified directory (leading /)
Output:
    true: File exists
    false: File doesn't exist
*/
bool QSPIFlashMemory::checkDirectoryExists(char directory[]) {
    path.resolve(resolvedPath, directory);
    if (fs.exists(resolvedPath)) {
        if (_debugLevel > 0) { Serial.print("\nQSPIFlashMemory::checkDirectoryExists - Exists"); }
        return true;
    }
    if (_debugLevel > 0) { Serial.print("\nQSPIFlashMemory::checkDirectoryExists - Doesnt Exist"); }
    return false;
}

/*
Method: getFile()
Description: Get specific file by directory and filename
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
Output:
    NULL: Directory does not exist or an error occurred
    File: File object for the directory
@TODO: Check file exists first and check if
*/
File QSPIFlashMemory::getFile(char directory[], char filename[]) {
    path.resolve(resolvedPath, directory, filename);
    return fs.open(resolvedPath);
}

/*
Method: createDirectory()
Description: Create an empty file in the specified directory with the specified name
Input:
    char directory[]: user-specified directory (leading /)
Output:
     0: success
    -1: already exists
    -2: error creating
    -3: Filesystem could not be mounted/accessed
    -9: flash not ready
*/
int QSPIFlashMemory::createDirectory(char directory[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkIfFlashMemoryIsReady() == false) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createDirectory() - Flash not ready"); }
        return -9;
    }
    if (checkDirectoryExists(directory) == true) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createDirectory() - Directory already exists"); }
        return -1;
    }
    if (!fs.mkdir(directory)) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createDirectory() - Directory creation error"); }
        return -2;
    }
    if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createDirectory() - Directory created"); }
    return 0;
}

/*
Method: createFile()
Description: Create an empty file in the specified directory with the specified name
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
Output:
     0: success
    -1: file already exists
    -2: Create Directory error already exists
    -3: Filesystem could not be mounted/accessed
    -4: Create directory failed
    -5: error creating
    -9: flash not ready

*/
int QSPIFlashMemory::createFile(char directory[], char filename[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkIfFlashMemoryIsReady() == false) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - Flash not ready"); }
        return -9;
    }

    if (checkFileExists(directory, filename) == true) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - File already exists"); }
        return -1;
    }

    if (checkDirectoryExists(directory) == false) {
        int createDirectoryRes = createDirectory(directory);
        switch (createDirectoryRes) {
            case -1: {
              if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - Directory already exists"); }
                return -2;
            }
            case -2: {
                if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - Directory nonexistent but error occurred during creation"); }
                return -4;
            }
            case 0: {
                if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - Directory Created "); }
            }
        }
    }

    path.resolve(resolvedPath, directory, filename);
    if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - creating file "); Serial.print(resolvedPath); }

    File cf = fs.open(resolvedPath, FILE_WRITE);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("\nError, failed to open file for writing"); }
        return -5;
    }

    cf.close();
    if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::createFile() - File created"); }

    return 0;
}

/*
Method: saveFile()
Description: Save content to file
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
    char content[]: User-specified file content (NULL-terminated)
    bool overwriteExistingContent: true = overwrite, false = append to file
Output:
     0: success
    -1: file didnt exist and failed to create it
    -2: file already has content and user requested not to over write
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::saveFile(char directory[], char filename[], char content[], bool overwriteExistingContent) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkFileExists(directory, filename) == false) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::saveFile() - File doesnt exist"); }

        if (createFile(directory, filename) != 0) {
            if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::saveFile() - File doesnt exist, error creating it "); }
           return -1;
        }
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::saveFile() - File didnt exist, so created it"); }
    }

    if (getFilesize(directory, filename) > 0) {
        if (overwriteExistingContent == false) {
            return -2;
        }
    }

    path.resolve(resolvedPath, directory, filename);

    File wf;
    if (overwriteExistingContent == true) {
        fs.remove(resolvedPath);
    } else {
        wf.seek(wf.available());
    }
    wf = fs.open(resolvedPath, FILE_WRITE);
    if (!wf) {
        if (_debugLevel > 0) { Serial.println("\nQSPIFlashMemory::saveFile() - Error, failed to open file for appending content!"); }
    }
    wf.print(content);

    wf.close();
    return 0;
}

/*
Method: appendFile()
Description: Save content to file
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
    char content[]: User-specified file content (NULL-terminated)
Output:
     0: success
    -1: file didnt exist and failed to create it
    -2: file already has content and user requested not to over write
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::appendToFile(char directory[], char filename[], char content[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkFileExists(directory, filename) == false) {
        if (createFile(directory, filename) == false) {
           return -1;
        }
    }

    path.resolve(resolvedPath, directory, filename);
    // fs.activate();
    File wf = fs.open(resolvedPath, FILE_WRITE);
    if (!wf) {
        if (_debugLevel > 0) { Serial.println("\nError, failed to open test.txt for writing!"); }
        return -2;
    }

    wf.seek(wf.available());
    wf.print(content);
    wf.close();
    return 0;
}

/*
Method: getFilesize()
Description: Get filesize of the file path provided
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
Output:
     0: success
    -1: Doesnt exist
    -2: error reading
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::getFilesize(char directory[], char filename[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkFileExists(directory, filename) == false) {
        return -1;
    }
    path.resolve(resolvedPath, directory, filename);
    File cf = fs.open(resolvedPath, FILE_READ);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("\nError, failed to open file for reading"); }
        return -2;
    }
    int size = cf.size();
    cf.close();
    return size;
}

/*
Method: readFileContents()
Description: Read file content to provided content array
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
    char content[]: User-specified file content (NULL-terminated)
    long maxReadSize: Read specific number of bytes
Output: true = content read and stored in content[] array, false = content not written
     0: success
    -1: File doesnt exist
    -2: error opening file to read
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::readFileContents(char directory[], char filename[], uint8_t content[], long maxReadSize) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }
    if (checkFileExists(directory, filename) == false) {
        return -1;
    }
    path.resolve(resolvedPath, directory, filename);
    File cf = fs.open(resolvedPath, FILE_READ);
    if (!cf) {
        if (_debugLevel > 0) { Serial.println("\nError, failed to open file for reading"); }
        return -2;
    }
    int i = 0;
    while (cf.available() && i < maxReadSize) {
        content[i] = cf.read();
        i++;
    }
    return 0;
}

/*
Method: deleteFile()
Description: Delete a file by its filename in the specified directory
Input:
    char directory[]: user-specified directory (leading /)
    char filename[]: User-specified filename (with extension)
Output:
     0: success
    -1: File could not be deleted
    -1: File was not deleted
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::deleteFile(char directory[], char filename[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }

    path.resolve(resolvedPath, directory, filename);
    if (!fs.remove(resolvedPath)) {
        if (_debugLevel > 0) { Serial.println("\nError, couldn't delete test.txt file!"); }
        return -1;
    }
    if (fs.exists(resolvedPath)) {
        if (_debugLevel > 0) { Serial.println("\nError, file was not deleted!"); }
        return -2;
    }
    if (_debugLevel > 0) { Serial.println("\nDeleted file!"); }
    return 0;
}

/*
Method: deleteDirectory()
Description: Delete directory and all sub-content
Input:
    char directory[]: user-specified directory (leading /)
Output:
     0: success
    -1: Directory could not be deleted
    -2: Directory was not deleted
    -3: Filesystem could not be mounted/accessed
*/
int QSPIFlashMemory::deleteDirectory(char directory[]) {
    fs.activate();
    if (!fs.begin()) {
        if (_debugLevel > 0) { Serial.print("\n -> Error, failed to mount filesystem!"); }
        return -3;
    }

    if (!fs.rmdir(directory)) {
        if (_debugLevel > 0) { Serial.println("\nError, couldn't delete directory!"); }
        return -1;
    }
    if (fs.exists(directory)) {
        if (_debugLevel > 0) { Serial.println("\nError, directory was not deleted!"); }
        return -2;
    }
    if (_debugLevel > 0) { Serial.println("\nDirectory was deleted!"); }
    return 0;
}

/*
Method: getFlashQSPIInterface()
Description: get the raw QSPI_GD25Q flash object
Input: None
Output: Adafruit_QSPI_GD25Q object
*/
Adafruit_QSPI_GD25Q QSPIFlashMemory::getFlashQSPIInterface() {
    return flash;
}

/*
Method: getFlashFileSystemInterface()
Description: get the raw FatFs object for low-level controls
Input: None
Output: Adafruit_W25Q16BV_FatFs object
*/
Adafruit_W25Q16BV_FatFs QSPIFlashMemory::getFlashFileSystemInterface() {
    return fs;
}

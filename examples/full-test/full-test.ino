#include <Arduino.h>
#include <QSPI_Flash.h>

QSPIFlashMemory flashMemory;


void setup() {
    Serial.begin(115200);
    while(!Serial);

    flashMemory.initialise(0);  // Set to a nonzero value to view additional action

    while (flashMemory.checkIfFlashMemoryIsReady() != true) {
        Serial.print("Flash chip unavailable. Retrying...");
        delay(2000);
    }
    Serial.print("Flash Chip Found");

    Serial.print("\nManufacturer: 0x");Serial.print(flashMemory.manufacturerID, HEX);
    Serial.print("\nDevice ID: 0x");Serial.print(flashMemory.deviceID, HEX);
    Serial.print("\nChip ID: 0x");Serial.print(flashMemory.chipModelID, HEX);
    Serial.print("\nAddress: 0x");Serial.print(flashMemory.chipAddress);
    Serial.print("\nFlash Pages: ");Serial.print(flashMemory.pageCount);
    Serial.print("\nFlash Page Size: ");Serial.print(flashMemory.pageSize);
    Serial.print("\nDrive Size: ");Serial.print(flashMemory.pageCount * flashMemory.pageSize);Serial.print(" bytes");
    Serial.print("\n\n");

    // *************************************************************************
    // * Format Flash
    // *************************************************************************
    Serial.print("\n\nFormatting Filesystem\n");

    int res = flashMemory.format();
    if (res < 0) {
        return;
    }

    // *************************************************************************
    // * Read Empty Flash
    // *************************************************************************
    Serial.print("\n\nTEST 1: Create directory, create files and list directory\n");
    flashMemory.createDirectory("/test-directory-1");

    Serial.print("\n -> Create file1.txt");
    flashMemory.createFile("/test-directory-1", "file1.txt");

    Serial.print("\n -> Create file2.txt");
    flashMemory.createFile("/test-directory-1", "file2.txt");

    Serial.print("\n -> Create file3.txt");
    flashMemory.createFile("/test-directory-1", "file3.txt");

    Serial.print("\n -> Create file4.txt");
    flashMemory.createFile("/test-directory-1", "file4.txt");

    Serial.print("\n -> Create file5.txt");
    flashMemory.createFile("/test-directory-1", "file5.txt");

    Serial.print("\n -> Create file6.txt");
    flashMemory.createFile("/test-directory-1", "file6.txt");

    Serial.print("\n -> Create file6.txt again (should conflict)");
    flashMemory.createFile("/test-directory-1", "file6.txt");


    File rootDir = flashMemory.getFilesInDirectory("/test-directory-1");

    if (!rootDir) {
        Serial.print(" -> Error, failed to open directory! May not exist");
        return;
    }
    if (!rootDir.isDirectory()) {
        Serial.print(" -> Error, requested path is not a directory!");
        return;
    }
    Serial.print("\n -> Directory Contents:\n");
    File child = rootDir.openNextFile();
    if (child) {
        while (child) {
            Serial.print("- "); Serial.print(child.name());
            child.isDirectory() ? Serial.print(" (directory)") : Serial.print("");
            child = rootDir.openNextFile();
            Serial.print("\n");
        }
    } else {
        Serial.print(" -> EMPTY");
    }
    rootDir.rewindDirectory();

    // *************************************************************************
    // * Save content to test file by overwriting
    // *************************************************************************
    Serial.print("\n\n*** TEST 2: Create directory, create file with one line of text and save content to it ***\n");
    File testFile2;
    flashMemory.createDirectory("/test-directory-2");

    Serial.print("\n -> Create file1.txt inside /test-directory-2");
    flashMemory.createFile("/test-directory-2", "file1.txt");

    char test2Line1[] = "This is one line of text";
    flashMemory.saveFile("/test-directory-2", "file1.txt", test2Line1, true);
    testFile2 = flashMemory.getFile("/test-directory-2", "file1.txt");
    Serial.print("\n -> Reading back file contents (should have 1 line): \n");
    while (testFile2.available()) {
        char c = testFile2.read();
        Serial.print(c);
    }
    char test2Line2[] = "This is one line of text\nThis is another line of text";
    flashMemory.saveFile("/test-directory-2", "file1.txt", test2Line2, true);

    testFile2 = flashMemory.getFile("/test-directory-2", "file1.txt");
    Serial.print("\n -> Reading back file contents (should have two lines): \n");
    while (testFile2.available()) {
        char c = testFile2.read();
        Serial.print(c);
    }

    // *************************************************************************
    // * Append content to test file by overwriting
    // *************************************************************************
    Serial.print("\n\n*** TEST 3: Create directory, create file with one line of text and append content to it ***\n");
    File testFile3;
    flashMemory.createDirectory("/test-directory-3");

    Serial.print("\n -> Create file1.txt inside /test-directory-3");
    flashMemory.createFile("/test-directory-3", "file1.txt");

    char test3Line1[] = "This is one line of text";
    flashMemory.appendToFile("/test-directory-3", "file1.txt", test3Line1);
    testFile3 = flashMemory.getFile("/test-directory-3", "file1.txt");
    Serial.print("\n -> Reading back file contents (should have 1 line): \n");
    while (testFile3.available()) {
        char c = testFile3.read();
        Serial.print(c);
    }
    char test3Line2[] = "This is one line of text\nThis is another line of text";
    flashMemory.appendToFile("/test-directory-3", "file1.txt", test3Line2);

    testFile3 = flashMemory.getFile("/test-directory-3", "file1.txt");
    Serial.print("\n -> Reading back file contents (should have two lines): \n");
    while (testFile3.available()) {
        char c = testFile3.read();
        Serial.print(c);
    }

    // *************************************************************************
    // * Write Test Files
    // *************************************************************************

    Serial.print("\n\n*** TEST 4: Query file details for a newly written file ***\n");
    File testFile4;
    flashMemory.createDirectory("/test-directory-4");

    // Create file by saving it directly
    char test4Line1[] = "Title\nLine 1\nLine 2\nLine 3\nLine 4\nLine 5";
    flashMemory.saveFile("/test-directory-4", "file1.txt", test4Line1, true);
    testFile4 = flashMemory.getFile("/test-directory-4", "file1.txt");
    Serial.print("\n -> Reading back file contents (should have 5 lines):\n");
    while (testFile4.available()) {
        char c = testFile4.read();
        Serial.print(c);
    }
    Serial.print("\nFile size (with helper method): "); Serial.print(flashMemory.getFilesize("/test-directory-4", "file1.txt"));
    Serial.print("\nFile size (direct with File object): "); Serial.print(testFile4.size());


    // *************************************************************************
    // * Test 5 - Create file, save data, use method to copy file contents into char array
    // *************************************************************************

    Serial.print("\n\n*** TEST 5: Copy file contents of newly written file into an array ***\n");
    File testFile5;
    flashMemory.createDirectory("/test-directory-5");

    // Create file by saving it directly
    char test5Line1[] = "Title\nLine 1\nLine 2\nLine 3\nLine 5\nLine 5";
    flashMemory.saveFile("/test-directory-5", "file1.txt", test5Line1, true);
    testFile5 = flashMemory.getFile("/test-directory-5", "file1.txt");

    uint8_t test5Content[40];
    int test5Res = flashMemory.readFileContents("/test-directory-5", "file1.txt", test5Content, sizeof(test5Content));

    Serial.print("\n -> Returned file content: \n");
    for (int i = 0 ; i < sizeof(test5Content) ; i++) {
        Serial.print(char(test5Content[i]));
    }

    // *************************************************************************
    // * Test 6 - Create file, then test deletion
    // *************************************************************************

    Serial.print("\n\n*** TEST 6: Create a file and test its deletion ***\n");
    File testFile6;
    flashMemory.createDirectory("/test-directory-6");

    // Create file by saving it directly
    char test6Line1[] = "Title\nLine 1\nLine 2\nLine 3\nLine 6\nLine 6";
    flashMemory.saveFile("/test-directory-6", "file1.txt", test6Line1, true);

    bool t6Exists;
    t6Exists = flashMemory.checkFileExists("/test-directory-6", "file1.txt");
    Serial.print("\n -> Does [/test-directory-6/file1.txt] exist? "); Serial.print(t6Exists ? "YES" : "NO");

    Serial.print("\n -> Performing deletion");
    flashMemory.deleteFile("/test-directory-6", "file1.txt");

    t6Exists = flashMemory.checkFileExists("/test-directory-6", "file1.txt");
    Serial.print("\n -> Does [/test-directory-6/file1.txt] exist? "); Serial.print(t6Exists ? "YES" : "NO");


    // *************************************************************************
    // * Test 7 - Create file, then test deletion of the entire directory
    // *************************************************************************

    Serial.print("\n\n*** TEST 7: Create a file and test its deletion ***\n");
    File testFile7;
    flashMemory.createDirectory("/test-directory-7");

    // Create file by saving it directly
    flashMemory.saveFile("/test-directory-7", "test7-file1.txt", "Content", true);
    flashMemory.saveFile("/test-directory-7", "test7-file2.txt", "Content", true);
    flashMemory.saveFile("/test-directory-7", "test7-file3.txt", "Content", true);
    flashMemory.saveFile("/test-directory-7", "test7-file4.txt", "Content", true);
    flashMemory.saveFile("/test-directory-7", "test7-file5.txt", "Content", true);
    flashMemory.saveFile("/test-directory-7", "test7-file7.txt", "Content", true);

    rootDir = flashMemory.getFilesInDirectory("/test-directory-7");
    rootDir.isDirectory();

    if (!rootDir) {
        Serial.print(" -> Error, failed to open directory! May not exist");
        return;
    }
    if (!rootDir.isDirectory()) {
        Serial.print(" -> Error, requested path is not a directory!");
        return;
    }
    Serial.print(" -> Directory Contents:");
    File test7Child = rootDir.openNextFile();
    if (test7Child) {
        while (test7Child) {
            Serial.print("- "); Serial.print(test7Child.name());
            test7Child.isDirectory() ? Serial.print(" (directory)") : Serial.print("");
            test7Child = rootDir.openNextFile();
            Serial.print("\n");
        }
    } else {
        Serial.print(" -> EMPTY");
    }
    rootDir.rewindDirectory();

    bool t7Exists;
    t7Exists = flashMemory.checkDirectoryExists("/test-directory-7");
    Serial.print("\n -> Does [/test-directory-7] exist? "); Serial.print(t7Exists ? "YES" : "NO");

    Serial.print("\n -> Performing deletion");
    flashMemory.deleteDirectory("/test-directory-7");

    t7Exists = flashMemory.checkDirectoryExists("/test-directory-7");
    Serial.print("\n -> Does [/test-directory-7] exist? "); Serial.print(t7Exists ? "YES" : "NO");


    // *************************************************************************
    // * Raw access to fat file system
    // *************************************************************************
    Adafruit_W25Q16BV_FatFs fatfs = flashMemory.getFlashFileSystemInterface();
    // File rootDir = fatfs.open("/test");


    Serial.print("\n\n... All Tests Complete ...\n");

}

void loop(){
  // Unused
}

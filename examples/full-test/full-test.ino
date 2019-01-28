#include <Arduino.h>
#include <QSPI_Flash.h>

QSPIFlashMemory flashMemory;


void setup() {
    Serial.begin(115200);
    while(!Serial);

    flashMemory.initialise(1);

    while (flashMemory.checkIfFlashMemoryIsReady() != true) {
        Serial.println("Flash chip unavailable. Retrying...");
        delay(2000);
    }
    Serial.println("Flash Chip Found");

    // Serial.print("Signal Select Pin: "); Serial.print(flash.);
    Serial.print("\nManufacturer: 0x");Serial.print(flashMemory.manufacturerID, HEX);
    Serial.print("\nDevice ID: 0x");Serial.print(flashMemory.deviceID, HEX);
    Serial.print("\nChip ID: 0x");Serial.print(flashMemory.chipModelID, HEX);
    Serial.print("\nAddress: 0x");Serial.print(flashMemory.chipAddress);
    Serial.print("\nFlash Pages: ");Serial.print(flashMemory.pageCount);
    Serial.print("\nFlash Page Size: ");Serial.print(flashMemory.pageSize);
    Serial.print("\nDrive Size: ");Serial.print(flashMemory.pageCount * flashMemory.pageSize);Serial.print(" bytes");
    Serial.println("");



    // *************************************************************************
    // * Format Flash
    // *************************************************************************
    flashMemory.format();

    // *************************************************************************
    // * Read Empty Flash
    // *************************************************************************



    // *************************************************************************
    // * Write Test Files
    // *************************************************************************
    // char filename1[] = "file1.txt";
    // char content1[] = "This is file 1!";
    // writeFileToDirectory(testDirectory, filename1, content1);
    //
    // char filename2[] = "file2.txt";
    // char content2[] = "This is file 2!";
    // writeFileToDirectory(testDirectory, filename2, content2);
    //
    // char filename3[] = "file3.txt";
    // char content3[] = "This is file 3!";
    // writeFileToDirectory(testDirectory, filename3, content3);
    //
    // char filename4[] = "file4.txt";
    // char content4[] = "This is file 4!";
    // writeFileToDirectory(testDirectory, filename4, content4);
    //
    // char filename5[] = "file5.txt";
    // char content5[] = "This is file 5!";
    // writeFileToDirectory(testDirectory, filename5, content5);

    // *************************************************************************
    // * Read back test files
    // *************************************************************************

    // readDirectory(testDirectory);

}

void loop(){
  //don't even do anything
}

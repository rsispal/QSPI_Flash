#include <Arduino.h>
#include "Path.h"

void Path::resolve(char path[], char directory[], char filename[]) {
    // Serial.print("\n -> Path.resolve - BEFORE RESET= " ); Serial.print(path);
    resetPath(path);
    // Serial.print("\n -> Path.resolve - AFTER RESET= " ); Serial.print(path);

    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    // Serial.print("\n -> Path.resolve - AFTER WRITE= " ); Serial.print(path);

}

void Path::resolve(char path[], char directory[]) {
    // Serial.print("\n -> Path.resolve - BEFORE RESET= " ); Serial.print(path);
    resetPath(path);
    // Serial.print("\n -> Path.resolve - AFTER RESET= " ); Serial.print(path);

    strcat(path, directory);
    // Serial.print("\n -> Path.resolve - AFTER WRITE= " ); Serial.print(path);

}

void Path::resetPath(char path[]) {
    for (int i = 0 ; i < sizeof(path) ; i++) {
        path[i] = 0x00;
    }
}

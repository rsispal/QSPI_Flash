#include <Arduino.h>
#include "Path.h"


/*
Method: initialise()
Description: Initialise with no debug enabled (default)
Input: None
Output: 0 = success
*/
int Path::initialise() {
    _debugLevel = 0;
    return 0;
}

/*
Method: initialise()
Description: Initialise with a specific debug level
Input: debugLevel integer (0 - 254)
Output: 0 = success, -1 = chip not ready
*/
int Path::initialise(int debugLevel) {
    if (debugLevel >= 0 && debugLevel < 255) {
        _debugLevel = debugLevel;
    }
    return 0;
}

/*
Method: resolve()
Description: Resolve a filename to a directory
Input:
    char path[]: Array where the path is to be stored
    char directory[]: User-specified directory (leading /)
    char filename[]: User-specified filename and extension (no /)
Output: N/A
@TODO: Check the / are in the correct place and resolved path doesnt exceed target array
*/
void Path::resolve(char path[], char directory[], char filename[]) {
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - BEFORE RESET= " ); Serial.print(path); }
    resetPath(path);
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - AFTER RESET= " ); Serial.print(path); }

    strcat(path, directory);
    strcat(path, "/");
    strcat(path, filename);
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - AFTER WRITE= " ); Serial.print(path); }

}

/*
Method: resolve()
Description: Resolve a directory =
Input:
    char path[]: Array where the path is to be stored
    char directory[]: User-specified directory (leading /)
Output: N/A
@TODO: Check the / are in the correct place and resolved path doesnt exceed target array
*/
void Path::resolve(char path[], char directory[]) {
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - BEFORE RESET= " ); Serial.print(path); }
    resetPath(path);
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - AFTER RESET= " ); Serial.print(path); }

    strcat(path, directory);
    if (_debugLevel > 0) { Serial.print("\n -> Path.resolve - AFTER WRITE= " ); Serial.print(path); }

    if (_debugLevel > 0) { Serial.print("\n Size of path: ");Serial.print(sizeof(path)); }
    if (_debugLevel > 0) { Serial.print("\n Size of directory: ");Serial.print(sizeof(directory)); }
}

/*
Method: resetPath()
Description: Erase path content
Input:
    char path[]: Array where the path is to be stored
Output: N/A
@TODO: Use a global constant for the max path size
*/
void Path::resetPath(char path[]) {
    for (int i = 0 ; i < 260 ; i++) {
        path[i] = 0x00;
    }
}

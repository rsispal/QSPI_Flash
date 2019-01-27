# QSPI Flash Fat File System Library
```
For Adafruit SAMD51 boards using onboard QSPI flash chips.

This library provides a FAT filesystem with an SD-like interface for Adafruit's SAMD51 boards.

To simplify usage, i've written a number of helper methods which allows faster usage and also provided methods for advanced users to access the QSPI connection and Filesystem.
```

## Compatibility
This library has been tailored for the Adafruit ItsyBitsy M4 and exposes:
- Adafruit_QSPI_GD25Q
- Adafruit_W25Q16BV_FatFs

Once I figure out a way to determine which flash chip the Adafruit libraries (which are a dependency) initialise, I can integrate those. For now, do it manually in the `qspi-flash.h` and `qspi-flash.cpp`.


## Contribute
Want to make this better? Open a pull request and issue explaining what you're contributing!

## Credits
All functionality is derived from Adafruit's repositories, linked below. I hold no rights or ownership of the code in those libraries. As such, no warranty is provided by your use of this library.

Adafruit_QSPI,
https://github.com/adafruit/Adafruit_QSPI.git

Adafruit_SPIFlash,
https://github.com/adafruit/Adafruit_SPIFlash.git

# simplestopwatch
A simple stop watch which uses SDL2 and is written in C.
# Installation
To install this program you have to run `make install` which will install the program to PREFIX/bin.
# Configuration
To configure this progams installation you must configure `configure.mk` and to configure the program itself you might want to configure `src/config.h` though for some things you must configure `src/simplestopwatch.h`. To test this program without installing it you must configure the paths (for images and fonts) in `src/config.h`

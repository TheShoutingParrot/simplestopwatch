# simplestopwatch's Makefile
# See LICENSE file for license details
# Copyright © 2020 Victor Ocampo 
# All rights reserved

include config.mk

OBJ = src/simplestopwatch.o src/util.o

all: clean simplestopwatch

.c.o:
	$(CC) -c $(CFLAGS) $<

$(OBJ): src/simplestopwatch.h src/config.h 

simplestopwatch: $(OBJ)
	$(CC) -o $@ simplestopwatch.o util.o $(LDFLAGS)

install: all
	mkdir -p $(PREFIX)/bin
	cp simplestopwatch $(PREFIX)/bin/simplestopwatch
	chmod +x $(PREFIX)/bin/simplestopwatch
	cp simplestopwatch.desktop $(APPLICATIONS)/simplestopwatch.desktop
	mkdir -p $(PREFIX)/share/simplestopwatch
	cp -r images/ $(PREFIX)/share/simplestopwatch/images/
	cp -r fonts/ $(PREFIX)/share/simplestopwatch/fonts/

uninstall: clean
	rm -f $(PREFIX)/bin/simplestopwatch
	rm -f -r $(PREFIX)/share/simplestopwatch

clean:
	rm -f simplestopwatch simplestopwatch.o util.o 

.PHONY: all simplestopwatch install uninstall clean

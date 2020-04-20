# paths
PREFIX 		= /usr/local
APPLICATIONS	= /usr/share/applications
SDLINC 		= /usr/include/SDL2

# SDL
SDLLIB 		= SDL2
SDLIMGLIB	= SDL2_image
SDLTTFLIB	= SDL2_ttf

# libraries
INCS	 	= -I$(SDLINC)
LIBS 		= -l$(SDLLIB) -l$(SDLIMGLIB) -l$(SDLTTFLIB)

# flags
CFLAGS		= -Wall -D_REENTRAT $(INCS)
LDFLAGS		= $(LIBS) 

# compiler
CC = gcc

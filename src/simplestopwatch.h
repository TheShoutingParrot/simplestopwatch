/*
 * simplestopwatch
 * Copyright © 2020 Victor Ocampo
 * All rights reserved
 */

#ifndef _SIMPLESTOPWATCH_H
#define _SIMPLESTOPWATCH_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define WNAME 		"simplestopwatch"

#define IMGPAUSE	"pausebutton"
#define IMGRESTART	"restartbutton"
#define IMGDARKMODE	"_darkmode"
#define IMGFILETYPE	"png"

#define DEF_CLIPS	4

enum defaultButtonState {
	MOUSE_OUT,
	MOUSE_OVER
};

enum pauseButtonState {
	MOUSE_OUT_PAUSE,
	MOUSE_OVER_PAUSE,
	MOUSE_OUT_RESUME,
	MOUSE_OVER_RESUME
};

typedef struct {
	SDL_Texture	*texture;
	size_t		width,
			height;
	SDL_Rect	clip[DEF_CLIPS];
	SDL_Rect	location;
} texture_t;

typedef struct {
	texture_t	texture;
	size_t		buttonWidth,
			buttonHeight;
	uint8_t 	state;
} button_t;

SDL_Window	*gWindow;
SDL_Renderer	*gRenderer;
TTF_Font 	*gFont;

texture_t	timeText;
button_t	pauseButton,
		restartButton;

char		*timeString;

uint32_t 	startTime,
		pausedTime;

int		wwidth,
		wheight,
		renderColor;

bool		timerRunning;


bool verbosity; 	
bool darkmode;

SDL_Color textColor;

bool initSdl(void);
void closeSdl(void);

texture_t loadTexture(const char *path);
texture_t loadTextureFromRenderedText(const char *text, SDL_Color color);
bool loadMedia(void);

bool isInsideOf(button_t *b);
void handleEvent(SDL_Event *e);

void resizeWindow(int x, int y);
void updateWindow(void);

void updateTimeString(void);

void timerPause(void);
void timerRestart(void);

void info(const char *fname, const size_t l, const char *msg, ...);
void verbose_info(const char *fname, const size_t l, const char *msg, ...);
void warning(const char *fname, const size_t l, const char *msg, ...);
void die(const char *fname, const size_t l, const char *msg, ...);
void usage(void);

#endif

#include "simplestopwatch.h"
#include "config.h"

int main(int argc, char *args[]) {
	SDL_Event e;
	int w, h;
	size_t i;
	bool quit;

	timerRunning 	= true;
	verbosity	= DEFAULT_VERBOSITY;
	darkmode	= DEFAULT_DARKMODE;

	wwidth 		= SCREEN_WIDTH;
	wheight 	= SCREEN_HEIGHT;

	timeString	= (char *)malloc(10 * sizeof(char));
	if(timeString == (char *)NULL)
		die(__FILE__, __LINE__, "Couldn't allocate memory!");

	for(i = 1; i < argc; i++) { // Goes through the command line arguments
		if(!strcmp(args[i], "-v") || !strcmp(args[i], "-verbose"))
			verbosity = true;
		else if(!strcmp(args[i], "-d") || !strcmp(args[i], "-darkmode"))
			darkmode = true;
		else if(!strcmp(args[i], "-l") || !strcmp(args[i], "-lightmode"))
			darkmode = false;
		else if(!strcmp(args[i], "-w") || !strcmp(args[i], "-width")) {
			if(argc <= i+1)
				usage();
			wwidth = atoi(args[++i]);
		}
		else if(!strcmp(args[i], "-h") || !strcmp(args[i], "-height")) {
			if(argc <= i+1)
				usage();
			wheight = atoi(args[++i]);
		}
		else
			usage();
	}

	if(!darkmode) {
		renderColor	= 0xff;

		textColor.r	= 0x00;
		textColor.g	= 0x00;
		textColor.b	= 0x00;
	}

	else {
		renderColor = 0x00;

		textColor.r = 0xff;
		textColor.g = 0xff;
		textColor.b = 0xff;

	}

	if(wwidth <= 0 || wheight <= 0) {
		warning(__FILE__, __LINE__, "Width or height is under 0");
		usage();
	}

	verbose_info(__FILE__, __LINE__, "\nThe program is starting with the following settings:\n"
			"verbosity: %d\ndarkmode: %d\nwindow width: %d\nwindow height: %d\n", verbosity, darkmode, wwidth, wheight);

	if(!initSdl())
		die(__FILE__, __LINE__, "Failed to initialize SDL!");
	verbose_info(__FILE__, __LINE__, "Initialization successful!");
	

	if(!loadMedia())
		die(__FILE__, __LINE__, "Failed to load media!");
	verbose_info(__FILE__, __LINE__, "The media has been loaded successfully!");

	quit = false;

	pauseButton.state = MOUSE_OUT_PAUSE;
	restartButton.state = MOUSE_OUT;

	startTime = SDL_GetTicks();

	SDL_GetWindowSize(gWindow, &w, &h);
	
	updateWindow();
	resizeWindow(w, h);

	while(!quit) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q))
				quit = true;
			else
				handleEvent(&e);
		}

		updateWindow();
	}

	closeSdl();

	return EXIT_SUCCESS;
}

bool initSdl(void) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		warning(__FILE__, __LINE__, "Failed to initialize SDL! %s", SDL_GetError());
		return false;
	}

	if(!IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) {
		warning(__FILE__, __LINE__, "Failed to initialize SDL_image! %s", IMG_GetError());
		return false;
	}

	if(TTF_Init() == -1) {
		warning(__FILE__, __LINE__, "Failed to initialize SDL_ttf! %s", TTF_GetError());
		return false;
	}

	gWindow = SDL_CreateWindow(WNAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			wwidth, wheight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(gWindow == NULL) {
		warning(__FILE__, __LINE__, "Failed to create the window! %s", SDL_GetError());
		return false;
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(gRenderer == NULL) {
		warning(__FILE__, __LINE__, "Failed to create the renderer! %s", SDL_GetError());
		return false;
	}

	return true;
}

// frees the textures, renderer and window, then quits
void closeSdl(void) {
	SDL_DestroyTexture(pauseButton.texture.texture);
	SDL_DestroyTexture(restartButton.texture.texture);
	SDL_DestroyTexture(timeText.texture);
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

texture_t loadTexture(const char *path) {
	texture_t newTexture;
	SDL_Surface *loadedSurface;

	loadedSurface = IMG_Load(path);
	if(loadedSurface == NULL) {
		warning(__FILE__, __LINE__, "Failed to load image! %s", IMG_GetError());
		newTexture.texture = NULL;
		return newTexture;
	}

	// This makes it so that light blue becomes transparent (for the background of the buttons)
	SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0x0, 0xff, 0xff));

	newTexture.texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
	if(newTexture.texture == NULL) {
		warning(__FILE__, __LINE__, "Failed to create texture! %s", SDL_GetError());
		newTexture.texture = NULL;
		return newTexture;
	}

	newTexture.width = loadedSurface->w;
	newTexture.height = loadedSurface->h;

	return(newTexture);
}

texture_t loadTextureFromRenderedText(const char *text, SDL_Color color) {
	texture_t newTextTexture;
	SDL_Surface *loadedTextSurface;

	loadedTextSurface = TTF_RenderText_Solid(gFont, text, color);
	if(loadedTextSurface == NULL) {
		warning(__FILE__, __LINE__, "Failed to load text! %s", TTF_GetError());
		newTextTexture.texture = NULL;
		return newTextTexture;
	}

	newTextTexture.texture = SDL_CreateTextureFromSurface(gRenderer, loadedTextSurface);
	if(newTextTexture.texture == NULL) {
		warning(__FILE__, __LINE__, "Failed to create texture! %s", SDL_GetError());
		return newTextTexture;
	}

	newTextTexture.width = loadedTextSurface->w;
	newTextTexture.height = loadedTextSurface->h;

	SDL_FreeSurface(loadedTextSurface);

	return newTextTexture;
}

bool loadMedia(void) {
	if(!darkmode) {
		pauseButton.texture = loadTexture(IMGDIR IMGPAUSE "." IMGFILETYPE);
		if(pauseButton.texture.texture == NULL) {
			warning(__FILE__, __LINE__, "Failed to load %s", IMGDIR IMGPAUSE "." IMGFILETYPE);
			return false;
		}

		restartButton.texture = loadTexture(IMGDIR IMGRESTART "." IMGFILETYPE);
		if(restartButton.texture.texture == NULL) {
			warning(__FILE__, __LINE__, "Failed to load %s", IMGDIR IMGRESTART "." IMGFILETYPE);
			return false;
		}
	}
	else {
		pauseButton.texture = loadTexture(IMGDIR IMGPAUSE IMGDARKMODE "." IMGFILETYPE);
		if(pauseButton.texture.texture == NULL) {
			warning(__FILE__, __LINE__, "Failed to load %s", IMGDIR IMGPAUSE IMGDARKMODE "." IMGFILETYPE);
			return false;
		}

		restartButton.texture = loadTexture(IMGDIR IMGRESTART IMGDARKMODE"." IMGFILETYPE);
		if(restartButton.texture.texture == NULL) {
			warning(__FILE__, __LINE__, "Failed to load %s", IMGDIR IMGRESTART IMGDARKMODE "." IMGFILETYPE);
			return false;
		}
	}

	gFont = TTF_OpenFont(FONTDIR FONT, 40);
	if(gFont == NULL) {	
		warning(__FILE__, __LINE__, "Failed to load font %s", FONTDIR FONT);
		return false;
	}

	pauseButton.buttonWidth = 35;
	pauseButton.buttonHeight = 50;

	pauseButton.texture.clip[0].x = 0;
	pauseButton.texture.clip[0].y = 0;
	pauseButton.texture.clip[0].w = pauseButton.buttonWidth;
	pauseButton.texture.clip[0].h = pauseButton.buttonHeight;

	pauseButton.texture.clip[1].x = 35;
	pauseButton.texture.clip[1].y = 0;
	pauseButton.texture.clip[1].w = pauseButton.buttonWidth;
	pauseButton.texture.clip[1].h = pauseButton.buttonHeight;

	pauseButton.texture.clip[2].x = 70;
	pauseButton.texture.clip[2].y = 0;
	pauseButton.texture.clip[2].w = pauseButton.buttonWidth;
	pauseButton.texture.clip[2].h = pauseButton.buttonHeight;

	pauseButton.texture.clip[3].x = 105;
	pauseButton.texture.clip[3].y = 0;
	pauseButton.texture.clip[3].w = pauseButton.buttonWidth;
	pauseButton.texture.clip[3].h = pauseButton.buttonHeight;

	restartButton.buttonWidth = 35;
	restartButton.buttonHeight = 50;

	restartButton.texture.clip[0].x = 0;
	restartButton.texture.clip[0].y = 0;
	restartButton.texture.clip[0].w = restartButton.buttonWidth;
	restartButton.texture.clip[0].h = restartButton.buttonHeight;

	restartButton.texture.clip[1].x = 35;
	restartButton.texture.clip[1].y = 0;
	restartButton.texture.clip[1].w = restartButton.buttonWidth;
	restartButton.texture.clip[1].h = restartButton.buttonHeight;

	return true;
}

bool isInsideOf(button_t *b) {
	int x, y;

	SDL_GetMouseState(&x, &y);
	if(x < b->texture.location.x || x > b->texture.location.x + b->buttonWidth
			|| y < b->texture.location.y || y > b->texture.location.y + b->buttonHeight)
		return false;

	return true;
}

void handleEvent(SDL_Event *e) {
	int w, h;

	switch(e->type) {
		case SDL_WINDOWEVENT:
			if(e->window.event == SDL_WINDOWEVENT_RESIZED) { // This is so that the window's variables are only resized when the window itself is resized
				SDL_GetWindowSize(gWindow, &w, &h);

				resizeWindow(w, h);
			}
			break;
		case SDL_KEYDOWN:
			if(e->key.keysym.mod == KMOD_NONE) {
				switch(e->key.keysym.sym) {
					case SDLK_RETURN:
					case SDLK_BACKSPACE:
					case SDLK_SPACE:
						timerPause();
						break;
					case SDLK_r:
						timerRestart();
						break;
					default:
						break;
				}
			}

			return;

		case SDL_MOUSEBUTTONDOWN:
			if(e->button.button == SDL_BUTTON_LEFT) {
				if(isInsideOf(&pauseButton)) {
					if(pauseButton.state == MOUSE_OUT_PAUSE || pauseButton.state == MOUSE_OVER_PAUSE)
						timerPause();
					else if(pauseButton.state == MOUSE_OUT_RESUME || pauseButton.state == MOUSE_OVER_RESUME)
						timerPause();
				}

				if(isInsideOf(&restartButton))
					timerRestart();
			}
			
			break;

		case SDL_MOUSEMOTION:
			if(!isInsideOf(&pauseButton)) {
				if(pauseButton.state == MOUSE_OUT_PAUSE || pauseButton.state == MOUSE_OVER_PAUSE)
					pauseButton.state = MOUSE_OUT_PAUSE;
				else if(pauseButton.state == MOUSE_OUT_RESUME || pauseButton.state == MOUSE_OVER_RESUME)
					pauseButton.state = MOUSE_OUT_RESUME;
			}

			else {
				if(pauseButton.state == MOUSE_OUT_PAUSE || pauseButton.state == MOUSE_OVER_PAUSE)
					pauseButton.state = MOUSE_OVER_PAUSE;
				else if(pauseButton.state == MOUSE_OUT_RESUME || pauseButton.state == MOUSE_OVER_RESUME)
					pauseButton.state = MOUSE_OVER_RESUME;
			}

			if(!isInsideOf(&restartButton))
				restartButton.state = MOUSE_OUT;

			else
				restartButton.state = MOUSE_OVER;

			break;

		default:
			break;
	}
}

void updateTimeString(void) {
	uint8_t sec, 
		min;
	uint16_t mil;

	if(timerRunning) {
		min = (SDL_GetTicks() - startTime) / (1000 * 60);
		sec = ((SDL_GetTicks() - startTime) - (min * (60 * 1000))) / 1000;
		mil = ((SDL_GetTicks() - startTime) - (min * (60 * 1000) + sec * 1000));
	}

	else if(!timerRunning) {
		min = (SDL_GetTicks() - (SDL_GetTicks() - pausedTime)) / (1000 * 60);
		sec = ((SDL_GetTicks() - (SDL_GetTicks() - pausedTime)) - (min * (60 * 1000))) / 1000;
		mil = ((SDL_GetTicks() - (SDL_GetTicks() - pausedTime)) - (min * (60 * 1000) + sec * 1000));
	}

	snprintf(timeString, 10, "%02d:%02d:%03d", min, sec, mil);
}

// The following function changes the location of the textures with the new size of the window
void resizeWindow(int w, int h) {
	pauseButton.texture.location.x = ((w - pauseButton.buttonWidth) / 2) + restartButton.buttonWidth;
	pauseButton.texture.location.y = (h / 2) + 100;
	pauseButton.texture.location.w = pauseButton.texture.clip[0].w;
	pauseButton.texture.location.h = pauseButton.texture.clip[0].h;

	restartButton.texture.location.x = ((w - restartButton.buttonWidth) / 2) - pauseButton.buttonWidth;
	restartButton.texture.location.y = (h / 2) + 100;
	restartButton.texture.location.w = pauseButton.texture.clip[0].w;
	restartButton.texture.location.h = pauseButton.texture.clip[0].h;

	timeText.location.x = (w - (timeText.width + 10)) / 2;
	timeText.location.y = (h / 2) - 25;
	timeText.location.w = timeText.width;
	timeText.location.h = timeText.height;
}

void updateWindow(void) {
	int orgX, orgY;

	SDL_SetRenderDrawColor(gRenderer, renderColor, renderColor, renderColor, renderColor);
	SDL_RenderClear(gRenderer);

	updateTimeString();

	orgX = timeText.location.x;
	orgY = timeText.location.y;
	timeText = loadTextureFromRenderedText(timeString, textColor);
	if(timeText.texture == NULL)
		die(__FILE__, __LINE__, "Failed to load text");
 
	timeText.location.w = timeText.width;
	timeText.location.h = timeText.height;
	timeText.location.x = orgX;
	timeText.location.y = orgY;

	SDL_RenderCopy(gRenderer, pauseButton.texture.texture, &pauseButton.texture.clip[pauseButton.state], &(pauseButton.texture.location));
	SDL_RenderCopy(gRenderer, restartButton.texture.texture, &restartButton.texture.clip[restartButton.state], &(restartButton.texture.location));
	SDL_RenderCopy(gRenderer, timeText.texture, NULL, &(timeText.location));

	SDL_RenderPresent(gRenderer);
}

void timerPause(void) {
	if(timerRunning) {
		timerRunning = false;

		pausedTime = SDL_GetTicks() - startTime;
		startTime = 0;
		verbose_info(__FILE__, __LINE__, "Timer paused at %d miliseconds!", SDL_GetTicks() - (SDL_GetTicks() - pausedTime));

		if(!isInsideOf(&pauseButton))
			pauseButton.state = MOUSE_OUT_RESUME;
		else
			pauseButton.state = MOUSE_OVER_RESUME;
	}

	else {
		timerRunning = true;

		startTime = SDL_GetTicks() - pausedTime;
		pausedTime = 0;
		verbose_info(__FILE__, __LINE__, "Timer unpaused, continuing at %d miliseconds!", SDL_GetTicks() - startTime);

		if(!isInsideOf(&pauseButton))
			pauseButton.state = MOUSE_OUT_PAUSE;
		else
			pauseButton.state = MOUSE_OVER_PAUSE;
	}
}

void timerRestart(void) {
	timerRunning = true;

	timerPause();

	pausedTime = 0;

	verbose_info(__FILE__, __LINE__, "The timer has been restarted!");
}

#include "emul8r.h"

/* SDL surface handles to the screen and backbuffer */
SDL_Surface *screen = NULL;
SDL_Surface *buffer = NULL;

int   screen_width = 64;
int   screen_height = 32;
int   screen_zoom = 8;

/* Initialize video & SDL */
int init_video() {
	if (SDL_Init(SDL_INIT_VIDEO) == -1) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return 0;
	}
	screen = SDL_SetVideoMode(screen_width * screen_zoom,
                              screen_height * screen_zoom, 32, SDL_SWSURFACE);
	if (screen == NULL) {
		fprintf(stderr, "SDL set video mode failed: %s\n", SDL_GetError());
		return 0;
	}
	buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, screen_width * screen_zoom,
	                              screen_height * screen_zoom, 32, 0, 0, 0, 0);
	if (buffer == NULL) {
		fprintf(stderr, "Backbuffer creation failed: %s\n", SDL_GetError());
		return 0;
	}
	SDL_WM_SetCaption("emul8r", NULL);
	return 1;
}

/* Puts a pixel on the backbuffer */
void drawpixel(int x, int y, int color) {
	SDL_Rect pixel;
	Uint32 colorval;
	switch (color) {
		case 0:
			colorval = SDL_MapRGB(buffer->format, 0x00, 0x00, 0x00);
			break;
		default:
			colorval = SDL_MapRGB(buffer->format, 0xff, 0xff, 0xff);
			break;
	}
	pixel.x = x * screen_zoom;
	pixel.y = y * screen_zoom;
	pixel.w = screen_zoom;
	pixel.h = screen_zoom;
	if (SDL_MUSTLOCK(buffer)) {
		SDL_LockSurface(buffer);
	}
	SDL_FillRect(buffer, &pixel, colorval);
	if (SDL_MUSTLOCK(buffer)) {
		SDL_UnlockSurface(buffer);
	}
}

/* Blits the backbuffer to the screen and flips */
void update() {
	SDL_Rect dstrect;
	dstrect.x = 0;
	dstrect.y = 0;
	while (SDL_BlitSurface(buffer, NULL, screen, &dstrect) == -2) {
		while (SDL_LockSurface(buffer) < 0) {
			SDL_Delay(10);
		}
		SDL_UnlockSurface(buffer);
	}
	SDL_Flip(screen);
}


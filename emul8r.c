#include "emul8r.h"

int init(void) {
	if (!init_video()) {
		return 0;
	}
	return 1;
}

void cleanup() {
	SDL_Quit();
}

void play(const char* filename) {
	int quit = 0;
	SDL_Event event;
	
	/* Create a new Chip-8 state */
	CHIP8* state = NULL;
	state = chip8_new();
	if (state == NULL) {
		fprintf(stderr, "Could not initialize new Chip-8 state\n");
		return;
	}
	
	/* Load a ROM and get ready to play! */
	state->memory = rom_load(filename);
	if (state->memory == NULL) {
		fprintf(stderr, "Could not load ROM\n");
		return;
	}
	
	/* Debugging shit */
	rom_dump(state->memory);
	drawpixel(0, 0, 1);
	
	/* Handle game cycles */
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			/* Handle events */
			switch (event.type) {
				case SDL_QUIT:
					quit = 1;
					break;
				case SDL_KEYDOWN:
					break;
			}
		}
		/* If processing is over, exit the game loop */
		if (!chip8_cycle(state)) {
			quit = 1;
		}
		/* Update the whole screen */
		update();
	}
}

int main(int argc, char *argv[]) {
	const char* filename;
	int c;
	
	/* Initialize everything */
	if (!init()) {
		return 1;
	}
	atexit(cleanup);
	
	/* Parse command-line arguments */
	while ((c = getopt(argc, argv, "hv")) != -1) {
		switch (c) {
			case 'h':
				/* TODO: show help/usage */
				break;
			case 'v':
				/* TODO: show version */
				break;
			case '?':
				if (isprint(optopt)) {
					fprintf(stderr, "Unknown option `-%c'\n", optopt);
				}
				return 1;
				break;
		}
	}
	
	/* Get ROM filename */
	if (optind < argc) {
		filename = argv[optind];
		printf("Using ROM `%s'\n", filename);
		play(filename);
	}
	else {
		/* TODO: menu */
		printf("There will be a menu here at some point\n");
	}
				
	
	return 0;
}

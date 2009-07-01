#if defined(_WIN32) || defined(WIN32)
#define OS_WIN
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "SDL/SDL.h"

#define FIRMWARE  "FIRMWARE"
#define MEMSIZE   0x1000
#define FIRMADDR  0x000
#define FIRMSIZE  0x100
#define STARTADDR 0x200
#define FONTADDR  0x0a0
#define STACKADDR 0x0e0
#define VIDEOADDR 0x100
#define ROMSIZE   MEMSIZE-STARTADDR

typedef unsigned char* ROM;

/* Represents a Chip-8 State */
typedef struct chip8 {
	int pc;              /* program counter           */
	int index;           /* index register            */
	int sp;              /* stack pointer             */
	int delay_timer;     /* delay timer               */
	int sound_timer;     /* sound timer               */
	unsigned char v[16]; /* general purpose registers */
	ROM memory;          /* rom memory image          */
} CHIP8;

/* emul8r.c */

int  init(void);
void cleanup(void);
void play(const char*);

/* rom.c */

ROM  rom_load(const char*);
void rom_dump(ROM);

/* video.c */

extern SDL_Surface* screen;
extern SDL_Surface* buffer;
extern int          screen_width;
extern int          screen_height;
extern int          screen_zoom;

int  init_video(void);
void drawpixel(int, int, int);
void update(void);

/* chip8.c */

CHIP8* chip8_new(void);
int    chip8_cycle(CHIP8*);

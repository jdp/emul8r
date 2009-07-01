#include "emul8r.h"

/* Returns a ROM image from a file, also loads firmware */
ROM rom_load(const char* filename) {
	FILE* file;
	unsigned char* rom_image = NULL;
	size_t result;
	
	/* Allocate a sufficient memory block of MEMSIZE bytes */
	rom_image = malloc(sizeof(unsigned char)*MEMSIZE+1);
	if (rom_image == NULL) {
		fprintf(stderr, "Could not allocate %d bytes for ROM\n", MEMSIZE);
		fclose(file);
		return NULL;
	}
	memset(rom_image, 0, ROMSIZE);
	
	/* Load the firmware into memory */
	file = fopen(FIRMWARE, "rb");
	if (!file) {
		fprintf(stderr, "Unable to load firmware `%s'\n", FIRMWARE);
		return NULL;
	}
	result = fread(rom_image+FIRMADDR, 1, FIRMSIZE, file);
	fclose(file);
	if (result < FIRMSIZE) {
		fprintf(stderr, "Incomplete firmware load:%d:%d\n", result, FIRMSIZE);
		return NULL;
	}
	
	/* Load the ROM into memory */
	file = fopen(filename, "rb");
	if (!file) {
		fprintf(stderr, "Unable to load ROM `%s'\n", filename);
		return NULL;
	}
	long filesize;
	fseek(file, 0, SEEK_END);
	filesize = ftell(file);
	fseek(file, 0, SEEK_SET);
	result = fread(rom_image+STARTADDR, 1, ROMSIZE, file);
	fclose(file);
	if (result < filesize) {
		fprintf(stderr, "Incomplete ROM load:%d:%ld\n", result, filesize);
		return NULL;
	}

	/* Return the entiry ROM image */
	return rom_image;
}

/* Dumps a ROM image to stdout */
void rom_dump(ROM rom_image) {
	int b;
	for (b = FIRMADDR; b < ROMSIZE; b++) {
		printf("%.2X ", rom_image[b]);
		if ((b % 4) == 3) {
			printf(" ");
		}
		if ((b % 16) == 15) {
			printf("\n");
		}
	}
	if ((b % 16) != 15) {
		printf("\n");
	}
}

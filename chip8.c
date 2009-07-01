#include "emul8r.h"

/* Create a new Chip-8 state */
CHIP8* chip8_new() {
	CHIP8* state = NULL;
	state = malloc(sizeof(CHIP8));
	if (state == NULL) {
		return NULL;
	}
	state->pc = 0;
	state->index = 0;
	state->sp = STACKADDR-2;
	state->delay_timer = 0;
	state->sound_timer = 0;
	for (int i = 0; i < 16; i++) {
		state->v[i] = 0;
	}
	state->memory = NULL;
	return state;
}

/* Move state ahead 1 cycle. Return value 1 means it's still running */
int chip8_cycle(CHIP8* state) {

	int x;
	unsigned char nn;

	/* Get current instruction in instruction register */
	int ir = (state->memory[state->pc] << 8) + state->memory[state->pc+1];
	state->pc += 2;
	
	/* Execute instruction by general opcode family */
	switch (ir>>12) {
	
		case 0x0:
			/* 00E0, clear screen */
			if (ir == 0x00e0) {
				int i;
				for (i = VIDEOADDR; i < 0x100; i++) {
					state->memory[i] = 0;
				}
			}
			/* 00EE, return from subroutine */
			else if (ir == 0x00ee) {
				state->pc = (state->memory[state->sp] << 8) + state->memory[state->sp+1];
				state->sp -= 2;
			}
			break;
			
		case 0x1:
			/* 1NNN, always a jump to 0xNNN */
			state->pc = ir & 0x0fff;
			break;
			
		case 0x2:
			/* 2NNN, calls subroutine at NNN */
			state->memory[++state->sp] = (unsigned char)((state->pc & 0xff00) >> 8);
			state->memory[++state->sp] = (unsigned char)(state->pc & 0x00ff);
			state->pc = ir & 0x0fff;
			break;
			
		case 0x3:
			/* 3XNN, Skip if VX == NN */
			x = (ir & 0x0f00) >> 8;
			nn = ir & 0x00ff;
			if (state->v[x] == nn) {
				state->pc += 2;
			}
			break;
			
		case 0x4:
			/* 4XNN, Skip if VX != NN */
			x = (ir & 0x0f00) >> 8;
			nn = ir & 0x00ff;
			if (state->v[x] != nn) {
				state->pc += 2;
			}
			break;
		
	}
	
	/* Somehow we've overrun our memory image */
	if (state->pc > ROMSIZE) {
		return 0;
	}
	
	return 1;
	
}
			

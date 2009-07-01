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

	int x, y;
	int carry = 0, borrow = 1;
	unsigned char n, nn;

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
			
		case 0x5:
			/* 5XY0, Skip if VX == VY */
			x = (ir & 0x0f00) >> 8;
			y = (ir & 0x00f0) >> 4;
			if (state->v[x] == state->v[y]) {
				state->pc += 2;
			}
			break;
			
		case 0x6:
			/* 6XNN, VX = NN */
			x = (ir & 0x0f00) >> 8;
			nn = ir & 0x00ff;
			state->v[x] = nn;
			break;
			
		case 0x7:
			/* 7XNN, VX = VX + NN, VF = carry? */
			x = (ir & 0x0f00) >> 8;
			nn = ir & 0x00ff;
			state->v[x] += nn;
			break;
			
		case 0x8:
			switch (ir & 0x000f) {
				case 0x0:
					/* 8XY0, VX = VY */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[x] = state->v[y];
					break;
				case 0x1:
					/* 8XY1, VX = VX | VY */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[x] |= state->v[y];
					break;
				case 0x2:
					/* 8XY2, VX = VX & VY */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[x] &= state->v[y];
					break;
				case 0x3:
					/* 8XY3, VX = VX ^ VY */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[x] ^= state->v[y];
					break;
				case 0x4:
					/* 8XY4, VX = VX + VY, VF = carry */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					if ((state->v[x] + state->v[y]) > 255) {
						carry = 1;
					}
					state->v[x] += state->v[y];
					state->v[0xf] = carry;
					break;
				case 0x5:
					/* 8XY5, VX = VX - VY, VY = not borrow */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					if (state->v[y] > state->v[x]) {
						borrow = 0;
					}
					state->v[x] -= state->v[y];
					state->v[0xf] = borrow;
					break;
				case 0x6:
					/* 8XY1, VX = VX >> 1, VF = carry */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[0xf] = state->v[x] & 0x01;
					state->v[x] >>= 1;
					break;
				case 0x7:
					/* 8XY1, VX = VY - VX, VF = not borrow */
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					if (state->v[x] > state->v[y]) {
						borrow = 0;
					}
					state->v[x] = state->v[y] - state->v[x];
					break;
				case 0x8:
					/* 8XY8, VX = VX << 1, VF = carry*/
					x = (ir & 0x0f00) >> 8;
					y = (ir & 0x00f0) >> 4;
					state->v[0xf] = state->v[x] >> 7;
					state->v[x] <<= 1;
					break;
			}
			break;
		
		case 0x9:
			if ((ir & 0x000f) == 0) {
				/* 9XY0, Skip if VX != VY */
				x = (ir & 0x0f00) >> 8;
				y = (ir & 0x00f0) >> 4;
				if (state->v[x] != state->v[y]) {
					state->pc += 2;
				}
			}
			break;
			
		case 0xa:
			/* ANNN, I = NNN */
			state->index = ir & 0x0fff;
			break;
			
		case 0xb:
			/* BNNN, Jump to NNN + V0 */
			state->pc = (ir & 0x0fff) + state->v[0];
			break;
		
		case 0xc:
			/* CXNN, VX = RANDOM & NN */
			x = (ir & 0x0f00) >> 8;
			nn = ir & 0x00ff;
			state->v[x] = genrand_int32() & nn;
			break;
			
		case 0xd:
			/* DXYN, Draw N-height sprite at VX, VY, collision in VF */
			x = ((ir & 0x0f00) >> 8) % 64;
			y = ((ir & 0x00f0) >> 4) % 32;
			n = (ir & 0x000f);
			unsigned char data;
			for (int line = 0; line < n; line++) {
				data = state->memory[state->index+line];
			}
			break;
		
	}
	
	/* How could this have happened... */
	if (state->pc > ROMSIZE) {
		return 0;
	}
	
	return 1;
	
}
			

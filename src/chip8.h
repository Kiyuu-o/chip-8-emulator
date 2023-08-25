#pragma once

#include <stdbool.h>

typedef struct {
    unsigned char gfx[64 * 32];
    unsigned char key[16];

    bool drawFlag;

    unsigned short opcode;

    unsigned char memory[4096];

    unsigned char V[16];

    unsigned short I;
    unsigned short pc;

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16];
    unsigned short sp;
} Chip8;

void initialize(Chip8* cpu);
void emulateCycle(Chip8* cpu);
bool loadApplication(Chip8* cpu, const char* filename);
void debugRender(Chip8* cpu);
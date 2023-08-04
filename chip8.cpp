#include "chip8.h"
#include <cstdio>
#include <cstring>

unsigned char chip8_fontset[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
        0x20, 0x60, 0x20, 0x20, 0x70,		// 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
        0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
        0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
        0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
        0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
        0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

chip8::chip8() = default;

chip8::~chip8() = default;

void chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // clear display
    memset(gfx, 0, sizeof(gfx));

    // clear stack
    memset(stack, 0, sizeof(stack));

    // clear registers
    memset(V, 0, sizeof(V));

    // clear memory
    memset(memory, 0, sizeof(memory));

    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // reset Timer
    delay_timer = 0;
    sound_timer = 0;

    drawFlag = false;
}

void chip8::emulateCycle() {
    opcode = memory[pc] << 8 | memory[pc + 1];

    // decode opcode
    // check the first character
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                case 0x0000: // 0x00E0: Clears the screen
                    // Execute opcode
                    memset(gfx, 0, sizeof(gfx));
                    break;

                case 0x000E: // 0x00EE: Returns from subroutine
                    // Execute opcode
                    --sp;
                    pc = stack[sp];
                    break;

                default:
                    printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0x1000: // 0x1NNN: jump to address NNN
            pc = opcode & 0x0FFF;
            break;
        case 0x2000: // 0x2NNN: calls subroutine at NNN
            stack[sp] = pc;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000: // 0x3XNN: skips the next instruction if VX equals NN
            if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x4000: // 0x4XNN: Skips the next instruction if VX does not equal NN
            if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 4;
            else
                pc += 2;
            break;
        case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY
            if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;
        case 0x6000: // 0x6XNN: Sets VX to NN.
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000: // 0x7XNN: Adds NN to VX.
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000: // 0x8XY0: Sets VX to the value of VY.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001: // 0x8XY1: Sets VX to VX or VY.
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002: // 0x8XY2: Sets VX to VX and VY.
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003: // 0x8XY3: Sets VX to VX xor VY.
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
                    if (V[(opcode & 0x0F00) >> 8] > 0xFF - V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
                    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
            }
            break;
            // more
    }

    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void chip8::setKeys() {

}

void chip8::loadGame(char* game) {

}
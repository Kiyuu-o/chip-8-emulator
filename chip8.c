#define _CRT_SECURE_NO_WARNINGS
#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif

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
        0xF0, 0x80, 0xF0, 0x80, 0x80		// Fstack
};

void initialize(Chip8* cpu) {
    cpu->pc = 0x200;
    cpu->opcode = 0;
    cpu->I = 0;
    cpu->sp = 0;

    // Clear display
    for (int i = 0; i < 2048; ++i)
        cpu->gfx[i] = 0;

    // Clear stack
    for (int i = 0; i < 16; ++i)
        cpu->stack[i] = 0;

    for (int i = 0; i < 16; ++i)
        cpu->key[i] = cpu->V[i] = 0;

    // Clear memory
    for (int i = 0; i < 4096; ++i)
        cpu->memory[i] = 0;

    // Load fontset
    for (int i = 0; i < 80; ++i)
        cpu->memory[i] = chip8_fontset[i];

    // reset Timer
    cpu->delay_timer = 0;
    cpu->sound_timer = 0;

    cpu->drawFlag = true;

    // for randomizing
    srand(time(NULL));
}

void emulateCycle(Chip8* cpu) {
    cpu->opcode = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

    printf("0x%x\n", cpu->opcode);

    // decode cpu->opcode
    // check the first character
    switch (cpu->opcode & 0xF000) {
        case 0x0000:
            switch (cpu->opcode & 0x000F) {
                // TODO: fix this
                case 0x0000: // 0x00E0: Clears the screen
                    // Execute cpu->opcode
                    for (int i = 0; i < 64 * 32; ++i)
                        cpu->gfx[i] = 0;
                    cpu->drawFlag = true;
                    cpu->pc += 2;
                    break;

                // TODO: fix this
                case 0x000E: // 0x00EE: Returns from subroutine
                    // Execute cpu->opcode
                    --cpu->sp;
                    cpu->pc = cpu->stack[cpu->sp];
                    cpu->pc += 2;
                    break;

                default:
                    printf("Unknown cpu->opcode [0x0000]: 0x%X\n", cpu->opcode);
            }
            break;
        case 0x1000: // 0x1NNN: jump to address NNN
            cpu->pc = cpu->opcode & 0x0FFF;
            break;
        case 0x2000: // 0x2NNN: calls subroutine at NNN
            cpu->stack[cpu->sp] = cpu->pc;
            ++cpu->sp;
            cpu->pc = cpu->opcode & 0x0FFF;
            break;
        case 0x3000: // 0x3XNN: skips the next instruction if VX equals NN
            if (cpu->V[(cpu->opcode & 0x0F00) >> 8] == (cpu->opcode & 0x00FF))
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;
        case 0x4000: // 0x4XNN: Skips the next instruction if VX does not equal NN
            if (cpu->V[(cpu->opcode & 0x0F00) >> 8] != (cpu->opcode & 0x00FF))
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;
        case 0x5000: // 0x5XY0: Skips the next instruction if VX equals VY
            if (cpu->V[(cpu->opcode & 0x0F00) >> 8] == cpu->V[(cpu->opcode & 0x00F0) >> 4])
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;
        case 0x6000: // 0x6XNN: Sets VX to NN.
            cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->opcode & 0x00FF;
            cpu->pc += 2;
            break;
        case 0x7000: // 0x7XNN: Adds NN to VX.
            cpu->V[(cpu->opcode & 0x0F00) >> 8] += cpu->opcode & 0x00FF;
            cpu->pc += 2;
            break;
        case 0x8000:
            switch(cpu->opcode & 0x000F) {
                case 0x0000: // 0x8XY0: Sets VX to the value of VY.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0001: // 0x8XY1: Sets VX to VX or VY.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] |= cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0002: // 0x8XY2: Sets VX to VX and VY.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] &= cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0003: // 0x8XY3: Sets VX to VX xor VY.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] ^= cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0004: // 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] += cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    if (cpu->V[(cpu->opcode & 0x0F00) >> 8] < cpu->V[(cpu->opcode & 0x00F0) >> 4])
                    //if (cpu->V[(cpu->opcode & 0x0F00) >> 8] > (0xFF - cpu->V[(cpu->opcode & 0x00F0) >> 4]))
                        cpu->V[0xF] = 1;
                    else
                        cpu->V[0xF] = 0;
                    //cpu->V[(cpu->opcode & 0x0F00) >> 8] += cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
                    if (cpu->V[(cpu->opcode & 0x00F0) >> 4] > cpu->V[(cpu->opcode & 0x0F00) >> 8]) // underflows
                        cpu->V[0xF] = 0;
                    else
                        cpu->V[0xF] = 1;
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] -= cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->pc += 2;
                    break;
                case 0x0006: // 0x8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->V[(cpu->opcode & 0x00F0) >> 4]; // different implementation in original CHIP-8
                    cpu->V[0XF] = cpu->V[(cpu->opcode & 0x0F00) >> 8] & 0x1;
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] >>= 1;
                    cpu->pc += 2;
                    break;
                case 0x0007: // 0x8XY7: sets VX to VY minus VX. set VF to 0 when there is a borrow and 1 when there's not.
                    if (cpu->V[(cpu->opcode & 0x0F00) >> 8] > cpu->V[(cpu->opcode & 0x00F0) >> 4]) // underflows
                        cpu->V[0xF] = 0;
                    else
                        cpu->V[0xF] = 1;
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->V[(cpu->opcode & 0x00F0) >> 4] - cpu->V[(cpu->opcode & 0x0F00) >> 8];
                    cpu->pc += 2;
                    break;
                case 0x000E: // 0x8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->V[(cpu->opcode & 0x00F0) >> 4];
                    cpu->V[0xF] = cpu->V[(cpu->opcode & 0x0F00) >> 8] & (1 << 8);
                    cpu->V[(cpu->opcode & 0x0F00) >> 8] <<= 1;
                    cpu->pc += 2;
                    break;
            }
            break;
        case 0x9000: // 0x9XY0: Skips the next instruction if VX does not equal VY.
            if (cpu->V[(cpu->opcode & 0x0F00) >> 8] != cpu->V[(cpu->opcode & 0x00F0) >> 4])
                cpu->pc += 4;
            else
                cpu->pc += 2;
            break;
        case 0xA000: // 0xANNN: sets I to the address NNN.
            cpu->I = cpu->opcode & 0x0FFF;
            cpu->pc += 2;
            break;
        case 0xB000: // 0xBNNN: jumps to the address NNN plus V0.
            cpu->pc = (cpu->opcode & 0x0FFF) + cpu->V[0x0];
            break;
        case 0xC000: // 0xCXNN: sets VX to the result of bitwise AND on a random number and NN.
            cpu->V[(cpu->opcode & 0x0F00) >> 8] = (rand() % 256) & (cpu->opcode & 0x00FF);
            cpu->pc += 2;
            break;
        case 0xD000: // 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
        {
            unsigned short x = cpu->V[(cpu->opcode & 0x0F00) >> 8];
            unsigned short y = cpu->V[(cpu->opcode & 0x00F0) >> 4];
            unsigned short height = cpu->opcode & 0x000F;
            unsigned short width = 8;
            unsigned short line;

            // VF will be set to 1 if any bit is flipped.
            cpu->V[0xF] = 0;

            // i is height here
            for (int i = 0; i < height; i++) {
                line = cpu->memory[cpu->I + i];
                for (int j = 0; j < width; j++) {
                    if ((line & (0x80 >> j)) != 0) {
                        if (cpu->gfx[j + x + (i + y) * 64] == 1)
                            cpu->V[0xF] = 1;
                        cpu->gfx[x + j + (y + i) * 64] ^= 1; // XOR
                    }
                }
            }

            cpu->drawFlag = true;
            cpu->pc += 2;
        }
        break;

            // skipped
        case 0xE000:
            switch (cpu->opcode & 0x000F) {
            case 0x000E: // 0xEX9E: Skips the next instruction if the key stored in VX is pressed
                if (cpu->key[cpu->V[(cpu->opcode & 0x0F00) >> 8]])
                    cpu->pc += 4;
                else
                    cpu->pc += 2;
                break;
            case 0x0001: // 0xExA1: Skips the next instruction if the key stored in VX is not pressed
                if (cpu->key[cpu->V[(cpu->opcode & 0x0F00) >> 8]])
                    cpu->pc += 2;
                else
                    cpu->pc += 4;
                break;
            }
            break;
        case 0xF000:
            switch (cpu->opcode & 0x00FF) {
            case 0x0007: // 0xFX07: set VX to the value of the delay timer.
                cpu->V[(cpu->opcode & 0x0F00) >> 8] = cpu->delay_timer;
                cpu->pc += 2;
                break;
            case 0x000A: // 0xFX0A: a key press is awaited, and then stored in VX.
            {
                bool keyPress = false;

                for (int i = 0; i < 16; i++) {
                    if (cpu->key[i]) {
                        cpu->V[(cpu->opcode & 0x0F00) >> 8] = i;
                        keyPress = true;
                    }
                }

                // if not received key press, run this cycle again.
                if (!keyPress)
                    return;

                cpu->pc += 2;
            }
                break;

            case 0x0015: // 0xFX15: set the delay timer to VX
                cpu->delay_timer = cpu->V[(cpu->opcode & 0x0F00) >> 8];
                cpu->pc += 2;
                break;
            case 0x0018: // 0xFX18: set the sound timer to VX
                cpu->sound_timer = cpu->V[(cpu->opcode & 0x0F00) >> 8];
                cpu->pc += 2;
                break;
            case 0x001E: // 0xFX1E: adds VX to I. VF is not affected (in common practice).
                cpu->I += cpu->V[(cpu->opcode & 0x0F00) >> 8];
                cpu->pc += 2;
                break;
            case 0x0029: // 0xFX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                cpu->I = cpu->V[(cpu->opcode & 0x0F00) >> 8] * 0x5;
                cpu->pc += 2;
                break;
            case 0x0033: // 0xFX33: Stores the binary-coded decimal representation of VX
                cpu->memory[cpu->I] = cpu->V[(cpu->opcode & 0x0F00) >> 8] / 100;
                cpu->memory[cpu->I + 1] = cpu->V[(cpu->opcode & 0x0F00) >> 8] % 100 / 10;
                cpu->memory[cpu->I + 2] = cpu->V[(cpu->opcode & 0x0F00) >> 8] % 10;
                cpu->pc += 2;
                break;
            case 0x0055: // 0xFX55: Stores from V0 to VX (including VX) in memory, starting at address I.
                for (int i = 0; i <= ((cpu->opcode & 0x0F00) >> 8); i++) {
                    cpu->memory[cpu->I + i] = cpu->V[i];
                }
                cpu->pc += 2;
                break;
            case 0x0065: // 0xFX65: Fills from V0 to VX (including VX) with values from memory, starting at address I.
                for (int i = 0; i <= ((cpu->opcode & 0x0F00) >> 8); i++) {
                    cpu->V[i] = cpu->memory[cpu->I + i];
                }
                cpu->pc += 2;
                break;
            }
            break;
        default:
            printf("Unknown cpu->opcode: 0x%X\n", cpu->opcode);
    }

    Sleep(1);

    if (cpu->delay_timer > 0)
        --cpu->delay_timer;

    if (cpu->sound_timer > 0) {
        if (cpu->sound_timer == 1)
            printf("BEEP!\n");
        --cpu->sound_timer;
    }
}

void debugRender(Chip8* cpu)
{
    printf("%Opcode: %x\n", cpu->opcode);
    // Draw
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (cpu->gfx[(y * 64) + x] == 0)
                printf("O");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

bool loadApplication(Chip8* cpu, const char* filename) {

    initialize(&cpu);
	printf("Loading: %s\n", filename);
		
    // Open file
    FILE* fptr = fopen(filename, "rb");
    if (NULL == fptr) {
        printf("Cannot open file");
        return false;
    }

    fseek(fptr, 0L, SEEK_END);
    long fileSize = ftell(fptr);
    rewind(fptr);
    printf("File Size: %d\n", (int)fileSize);

    // allocate memory to contain the whole file
    char* buffer = (char*)malloc(sizeof(char) * fileSize);
    if (NULL == buffer) {
        printf("Memory Error\n");
        return false;
    }

    // read the whole file into buffer
    size_t result = fread(buffer, sizeof(char), fileSize, fptr);
    if (result != fileSize) {
        printf("Reading Error\n");
        return false;
    }

    if ((4096 - 512) > fileSize) {
        for (int i = 0; i < fileSize; ++i) {
            cpu->memory[i + 512] = buffer[i];
        }
    }
    else {
        printf("Error: ROM too big for memory.\n");
        return false;
    }

    //for (int i = 0; i < fileSize; ++i) {
    //    printf("0x%2x 0x%2x\n", cpu->memory[i + 512], buffer[i]);
    //}

    // close file, free buffer
    fclose(fptr);
    free(buffer);

    return true;
}
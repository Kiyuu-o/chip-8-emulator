#define _CRT_SECURE_NO_WARNINGS
#include "chip8.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

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
        0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};

chip8::chip8() {

}

chip8::~chip8() {

}

void chip8::initialize() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    // Clear display
    for (int i = 0; i < 2048; ++i)
        gfx[i] = 0;

    // Clear stack
    for (int i = 0; i < 16; ++i)
        stack[i] = 0;

    for (int i = 0; i < 16; ++i)
        key[i] = V[i] = 0;

    // Clear memory
    for (int i = 0; i < 4096; ++i)
        memory[i] = 0;

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory[i] = chip8_fontset[i];

    // reset Timer
    delay_timer = 0;
    sound_timer = 0;

    drawFlag = true;

    // for randomizing
    srand(time(NULL));
}

void chip8::emulateCycle() {
    opcode = (memory[pc] << 8) | memory[pc + 1];

    printf("0x%x\n", opcode);

    // decode opcode
    // check the first character
    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode & 0x000F) {
                // TODO: fix this
                case 0x0000: // 0x00E0: Clears the screen
                    // Execute opcode
                    for (int i = 0; i < 64 * 32; ++i)
                        gfx[i] = 0;
                    drawFlag = true;
                    pc += 2;
                    break;

                // TODO: fix this
                case 0x000E: // 0x00EE: Returns from subroutine
                    // Execute opcode
                    --sp;
                    pc = stack[sp];
                    pc += 2;
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
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    if (V[(opcode & 0x0F00) >> 8] < V[(opcode & 0x00F0) >> 4])
                    //if (V[(opcode & 0x0F00) >> 8] > (0xFF - V[(opcode & 0x00F0) >> 4]))
                        V[0xF] = 1;
                    else
                        V[0xF] = 0;
                    //V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005: // 0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
                    if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) // underflows
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006: // 0x8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]; // different implementation in original CHIP-8
                    V[0XF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    pc += 2;
                    break;
                case 0x0007: // 0x8XY7: sets VX to VY minus VX. set VF to 0 when there is a borrow and 1 when there's not.
                    if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) // underflows
                        V[0xF] = 0;
                    else
                        V[0xF] = 1;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E: // 0x8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & (1 << 8);
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;
                    break;
            }
            break;
        case 0x9000: // 0x9XY0: Skips the next instruction if VX does not equal VY.
            if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 4;
            else
                pc += 2;
            break;
        case 0xA000: // 0xANNN: sets I to the address NNN.
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000: // 0xBNNN: jumps to the address NNN plus V0.
            pc = (opcode & 0x0FFF) + V[0x0];
            break;
        case 0xC000: // 0xCXNN: sets VX to the result of bitwise AND on a random number and NN.
            V[(opcode & 0x0F00) >> 8] = (rand() % 256) & (opcode & 0x00FF);
            pc += 2;
            break;
        case 0xD000: // 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
        {
            unsigned short x = V[(opcode & 0x0F00) >> 8];
            unsigned short y = V[(opcode & 0x00F0) >> 4];
            unsigned short height = opcode & 0x000F;
            unsigned short width = 8;
            unsigned short line;

            // VF will be set to 1 if any bit is flipped.
            V[0xF] = 0;

            // i is height here
            for (int i = 0; i < height; i++) {
                line = memory[I + i];
                for (int j = 0; j < width; j++) {
                    if ((line & (0x80 >> j)) != 0) {
                        if (gfx[j + x + (i + y) * 64] == 1)
                            V[0xF] = 1;
                        gfx[x + j + (y + i) * 64] ^= 1; // XOR
                    }
                }
            }

            drawFlag = true;
            pc += 2;
        }
        break;

            // skipped
        case 0xE000:
            switch (opcode & 0x000F) {
            case 0x000E: // 0xEX9E: Skips the next instruction if the key stored in VX is pressed
                if (key[V[(opcode & 0x0F00) >> 8]])
                    pc += 4;
                else
                    pc += 2;
                break;
            case 0x0001: // 0xExA1: Skips the next instruction if the key stored in VX is not pressed
                if (key[V[(opcode & 0x0F00) >> 8]])
                    pc += 2;
                else
                    pc += 4;
                break;
            }
            break;
        case 0xF000:
            switch (opcode & 0x00FF) {
            case 0x0007: // 0xFX07: set VX to the value of the delay timer.
                V[(opcode & 0x0F00) >> 8] = delay_timer;
                pc += 2;
                break;
            case 0x000A: // 0xFX0A: a key press is awaited, and then stored in VX.
            {
                bool keyPress = false;

                for (int i = 0; i < 16; i++) {
                    if (key[i]) {
                        V[(opcode & 0x0F00) >> 8] = i;
                        keyPress = true;
                    }
                }

                // if not received key press, run this cycle again.
                if (!keyPress)
                    return;

                pc += 2;
            }
                break;

            case 0x0015: // 0xFX15: set the delay timer to VX
                delay_timer = V[(opcode & 0x0F00) >> 8];
                pc += 2;
                break;
            case 0x0018: // 0xFX18: set the sound timer to VX
                sound_timer = V[(opcode & 0x0F00) >> 8];
                pc += 2;
                break;
            case 0x001E: // 0xFX1E: adds VX to I. VF is not affected (in common practice).
                I += V[(opcode & 0x0F00) >> 8];
                pc += 2;
                break;
            case 0x0029: // 0xFX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                I = V[(opcode & 0x0F00) >> 8] * 0x5;
                pc += 2;
                break;
            case 0x0033: // 0xFX33: Stores the binary-coded decimal representation of VX
                memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                memory[I + 1] = V[(opcode & 0x0F00) >> 8] % 100 / 10;
                memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
                pc += 2;
                break;
            case 0x0055: // 0xFX55: Stores from V0 to VX (including VX) in memory, starting at address I.
                for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                    memory[I + i] = V[i];
                }
                pc += 2;
                break;
            case 0x0065: // 0xFX65: Fills from V0 to VX (including VX) with values from memory, starting at address I.
                for (int i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
                    V[i] = memory[I + i];
                }
                pc += 2;
                break;
            }
            break;
        default:
            printf("Unknown opcode: 0x%X\n", opcode);
    }

    Sleep(1);

    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0) {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

void chip8::debugRender()
{
    printf("%Opcode: %x\n", opcode);
    // Draw
    for (int y = 0; y < 32; ++y)
    {
        for (int x = 0; x < 64; ++x)
        {
            if (gfx[(y * 64) + x] == 0)
                printf("O");
            else
                printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

bool chip8::loadApplication(const char* filename) {

    initialize();
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
            memory[i + 512] = buffer[i];
        }
    }
    else {
        printf("Error: ROM too big for memory.\n");
        return false;
    }

    //for (int i = 0; i < fileSize; ++i) {
    //    printf("0x%2x 0x%2x\n", memory[i + 512], buffer[i]);
    //}

    // close file, free buffer
    fclose(fptr);
    free(buffer);

    return true;
}
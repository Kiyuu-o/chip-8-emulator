#include <cstdio>
#include "chip8.h"

chip8 myChip8;

void drawGraphics();

int main(int argc, char **argv) {

//    setupGraphics();
//    setupInput();

    myChip8.initialize();
//    myChip8.loadGame("pong");

    for (;;) {
        myChip8.emulateCycle();

        if (myChip8.drawFlag)
            drawGraphics();

        myChip8.setKeys();
    }

    return 0;
}

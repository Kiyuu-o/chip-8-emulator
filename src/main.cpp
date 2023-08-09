#include <stdio.h>
//#include <stdlib.h>
#include "chip8.h"
#include <GL/glut.h>

//#define _LOCAL_DEBUG

const int WINDOW_WIDTH = 64;
const int WINDOW_HEIGHT = 32;

chip8 myChip8;
int modifier = 10;

int window_width = WINDOW_WIDTH * modifier;
int window_height = WINDOW_HEIGHT * modifier;

// key mapping for processing keyboard input.
char keyMap[] = "1234qwerasdfzxcv";

//void setupGraphics();
void display();
void drawPixel(int x, int y);
void updateQuads(const chip8& c8);
void keyboardDown(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void resizeFunc(int width, int height);

int main(int argc, char **argv) {

    if (argc < 2)
    {
        printf("Usage: myChip8.exe chip8application\n\n");
        return 1;
    }

    // Load game
    if (!myChip8.loadApplication(argv[1]))
        return 1;
    
    // setup graphics
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(window_width, window_height);
    glutInitWindowPosition(320, 320);
    glutCreateWindow("chip8 emulator created by Kevin Miao");
    
    glutReshapeFunc(resizeFunc);
    glutDisplayFunc(display);
    glutIdleFunc(display);

    // process keyboard input
    glutKeyboardFunc(keyboardDown);
    glutKeyboardUpFunc(keyboardUp);

    // main event loop for glut
    glutMainLoop();

    return 0;
}

void drawPixel(int x, int y)
{
        glVertex3f((x * modifier) + 0.0f, (y * modifier) + 0.0f, 0.0f);
        glVertex3f((x * modifier) + 0.0f, (y * modifier) + modifier, 0.0f);
        glVertex3f((x * modifier) + modifier, (y * modifier) + modifier, 0.0f);
        glVertex3f((x * modifier) + modifier, (y * modifier) + 0.0f, 0.0f);
}

void updateQuads(const chip8& c8)
{
    glBegin(GL_QUADS);
    // Draw
    for (int y = 0; y < 32; ++y)
        for (int x = 0; x < 64; ++x)
        {
            if (c8.gfx[(y * 64) + x] == 0)
                glColor3f(0.0f, 0.0f, 0.0f);
            else
                glColor3f(1.0f, 1.0f, 1.0f);

            drawPixel(x, y);
        }
    glEnd();
}

void display() {
    myChip8.emulateCycle();

#ifdef _LOCAL_DEBUG
    myChip8.debugRender();
#endif

    if (myChip8.drawFlag) {
        glClear(GL_COLOR_BUFFER_BIT);

        // draw something

        updateQuads(myChip8);

        glutSwapBuffers();

        myChip8.drawFlag = false;   // finished drawing
    }
}

void keyboardDown(unsigned char key, int x, int y) {
    // Esc
    if (key == 27)
        exit(0);

    for (int i = 0; i < 16; i++) {
        if (key == keyMap[i]) {
            myChip8.key[i] = 1;
            //printf("%c 0x%X\n", key, i); // for debugging purposes
        }
    }
    
}
void keyboardUp(unsigned char key, int x, int y) {

    for (int i = 0; i < 16; i++) {
        if (key == keyMap[i])
            myChip8.key[i] = 0;
    }
}

void resizeFunc(int width, int height) {
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);

    window_width = width;
    window_height = height;
}
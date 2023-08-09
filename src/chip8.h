class chip8 {
    public:
        chip8();
        ~chip8();
        void initialize();
        void emulateCycle();
        bool loadApplication(const char* filename);

        unsigned char gfx[64 * 32]; // graphics
        unsigned char key[16];  // HEX based keypad (0x0 to 0xF)

        bool drawFlag;

        void debugRender();

    private:
        unsigned short opcode;   // has the length of 2 bytes, (a char is only 1 byte)

        unsigned char memory[4096]; // 4k

        unsigned char V[16];    // registers

        unsigned short I;   // index register
        unsigned short pc;  // program counter

        // timer registers
        unsigned char delay_timer;
        unsigned char sound_timer;

        // stack and stack pointer
        unsigned short stack[16];
        unsigned short sp;

};
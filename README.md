# chip-8-emulator
My implementation of Chip-8 emulator with C++ and FreeGLUT.

<hr />

### Some Details

- When working with 0XYE and so on, make sure to get the X not only by bitmask AND, but also >> 8 to make get rid of the extra digits at the end.
- the image couldn't be displayed at first, make sure the correct callback function is written for reshapeWindow(): https://stackoverflow.com/questions/23214249/opengl-glut-cant-draw-on-it
- The rendering was really slow when I first implemented it. The solution to this problem: https://community.khronos.org/t/slow-rendering-using-basic-code/77232/2



<hr />

### Resources

- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) - System information
- [Cowgod’s Chip-8 Technical Reference v1.0](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM) - Recommended
- [Chip8 tutorial](https://multigesture.net/wp-content/uploads/mirror/goldroad/chip8.shtml) - Goldroad.co.uk (mirror)
- [(S)Chip 8 instruction set](https://multigesture.net/wp-content/uploads/mirror/goldroad/chip8_instruction_set.shtml) - Goldroad.co.uk (mirror)
- [David Winter’s CHIP-8 emulation page](http://www.pong-story.com/chip8/) - Contains some games
- [(S)CHIP-8 instruction set overview](http://devernay.free.fr/hacks/chip8/schip.txt) - (Erik Bryntse)
- [Chip8 emulator topic on Emutalk](http://www.emutalk.net/threads/19894-Chip-8)
- [Chip8 emulator topic on NGemu](http://forums.ngemu.com/web-development-programming/114578-chip8-thread.html)

### Advanced emulator resources

- Zilmar’s Emubook
	- [Emubook CPU emulation chapter](http://emubook.emulation64.com/cpu.htm)
	- [Emubook memory emulation chapter](http://emubook.emulation64.com/memory.htm)
- Zenogais Emulation Tutorials - (Offline, but available through Archive.org)
- Zenogais’ Emulation Tutorials - (mirror)
    - [Dynamic Recompiler](https://multigesture.net/wp-content/uploads/mirror/zenogais/Dynamic Recompiler.html) - (mirror)
    - [Array of Function Pointers](https://multigesture.net/wp-content/uploads/mirror/zenogais/FunctionPointers.htm) - (mirror)
    - [Introduction to Emulation Part 1](https://multigesture.net/wp-content/uploads/mirror/zenogais/EmuDoc1.htm) - (mirror)
    - [Introduction to Emulation Part 2](https://multigesture.net/wp-content/uploads/mirror/zenogais/EmuDoc2.html) - (mirror)
    - [Laying the Ground For An Emulator](https://multigesture.net/wp-content/uploads/mirror/zenogais/GroundWork.html) - (mirror)
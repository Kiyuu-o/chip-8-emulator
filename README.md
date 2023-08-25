# chip-8-emulator

My implementation of a CHIP-8 emulator written in C. Compiles on Linux. Depends on GLUT.

## Building and Running:

```
sudo apt-get install freeglut3 freeglut3-dev cmake gcc
cd ..
mkdir build
cd build
cmake ../chip-8-emulator
cmake --build .
```

## References

- FreeGLUT: https://freeglut.sourceforge.net/
- CMake: https://cmake.org/

Tutorials:
- https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
- [CHIP-8 on Wikipedia](https://en.wikipedia.org/wiki/CHIP-8) - System information
- https://tobiasvl.github.io/blog/write-a-chip-8-emulator/

## License

MIT
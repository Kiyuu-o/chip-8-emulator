CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
project(chip8 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(${GLUT_INCLUDE_DIR})

add_executable(chip8 main.c)
target_link_libraries(chip8 
PRIVATE
    ${GLUT_LIBRARIES}
)
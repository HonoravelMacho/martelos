cmake_minimum_required(VERSION 3.10)
project(martelos C)

set(CMAKE_C_STANDARD 11)

# Procura a biblioteca GMP
find_library(GMP_LIB gmp)

# Define os arquivos fonte
set(SOURCES
    main.c
    alfabeto.c
    motor.c
    io.c
    crypto.c
    parser.c
    binario.c
)

add_executable(martelos ${SOURCES})

# LIGA AS BIBLIOTECAS: gmp para os números e m para a matemática (sin/cos)
target_link_libraries(martelos gmp m)

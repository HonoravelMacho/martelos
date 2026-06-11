cmake_minimum_required(VERSION 3.10)
project(Martelos C)

set(CMAKE_C_STANDARD 11)

find_library(GMP_LIB gmp)
if(NOT GMP_LIB)
    message(FATAL_ERROR "Biblioteca GMP não encontrada!")
endif()

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
target_link_libraries(martelos ${GMP_LIB})
install(TARGETS martelos DESTINATION /usr/local/bin)

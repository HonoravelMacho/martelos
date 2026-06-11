cmake_minimum_required(VERSION 3.10)
project(Martelos C)

# Define o padrão C11
set(CMAKE_C_STANDARD 11)

# Procura a biblioteca GMP (obrigatória)
find_library(GMP_LIB gmp)
if(NOT GMP_LIB)
    message(FATAL_ERROR "Biblioteca GMP não encontrada! Instale-a com sudo apt install libgmp-dev")
endif()

# Lista de arquivos do projeto
set(SOURCES
    main.c
    alfabeto.c
    motor.c
    io.c
    crypto.c
    parser.c
)

# Cria o executável chamado 'martelos'
add_executable(martelos ${SOURCES})

# Linka a biblioteca GMP ao executável
target_link_libraries(martelos ${GMP_LIB})

# Comando de Instalação: permite que 'sudo make install' coloque o binário no sistema
install(TARGETS martelos DESTINATION /usr/local/bin)

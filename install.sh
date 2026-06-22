#!/bin/bash
set -e

# 1. Detecção de Ambiente e Definição de Caminhos
if [ -d "/data/data/com.termux/files/usr/bin" ]; then
    OS="Android-Termux"
    BIN_DIR="/data/data/com.termux/files/usr/bin"
    PKG_MGR="pkg install -y"
    SUDO=""
else
    OS="Linux-Desktop"
    BIN_DIR="/usr/local/bin"
    PKG_MGR="sudo apt install -y"
    SUDO="sudo"
fi

echo "🔨 Instalando M.A.R.T.E.L.O.S. v6.1.2 no $OS..."

# 2. Instalação de Dependências
echo "📦 Verificando dependências..."
if [ "$OS" = "Linux-Desktop" ]; then
    sudo apt update
    $PKG_MGR libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    $PKG_MGR libgmp cmake clang git ffmpeg
fi

# 3. Compilação de Alta Performance
echo "⚙️  Compilando Motor Algébrico (-O3 Optimization)..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 4. Instalação Global do Binário
echo "🚀 Movendo binário para $BIN_DIR..."
$SUDO mv martelos $BIN_DIR/
$SUDO chmod +x $BIN_DIR/martelos

# 5. Configuração da Bancada de Trabalho
echo "📁 Organizando pastas em ~/Downloads/martelos/..."
BASE_DIR="$HOME/Downloads/martelos"
mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
mkdir -p "$BASE_DIR/textos_salvos"

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. v6.1.2 INSTALADO COM SUCESSO!"
echo "================================================="
echo "Comandos disponíveis:"
echo "-> 'martelos' (Menu Interativo)"
echo "-> 'martelos --help' (Manual de Flags)"
echo "================================================="

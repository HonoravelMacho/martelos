#!/bin/bash
set -e

# Detectar ambiente
if [ -d "/data/data/com.termux/files/usr/bin" ]; then
    OS="Android-Termux"
    PREFIX="/data/data/com.termux/files/usr"
    PKG_MGR="pkg install -y"
else
    OS="Linux"
    PREFIX="/usr/local"
    PKG_MGR="sudo apt install -y"
fi

echo "🔨 Instalando M.A.R.T.E.L.O.S. v5.7.1 no $OS..."

# 1. Instalar Dependências
if [ "$OS" = "Linux" ]; then
    sudo apt update
    $PKG_MGR libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    $PKG_MGR libgmp cmake clang git ffmpeg
fi

# 2. Compilar
echo "⚙️ Compilando Motor Algébrico..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 3. Instalar Binário
echo "🚀 Movendo para o PATH..."
if [ "$OS" = "Linux" ]; then
    sudo mv martelos $PREFIX/bin/
else
    mv martelos $PREFIX/bin/
fi

# 4. Criar Pastas de Trabalho
echo "📁 Configurando Bancada de Trabalho..."
BASE_DIR="$HOME/Downloads/martelos"
mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
mkdir -p "$BASE_DIR/textos_salvos"

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. INSTALADO COM SUCESSO!"
echo "Digite 'martelos' em qualquer lugar para rodar."
echo "================================================="

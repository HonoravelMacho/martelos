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

echo "🔨 Instalando M.A.R.T.E.L.O.S. v5.8.0 no $OS..."

# 1. Instalar Dependências
echo "📦 Atualizando repositórios e instalando dependências..."
if [ "$OS" = "Linux" ]; then
    sudo apt update
    sudo apt install -y libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    pkg install -y libgmp cmake clang git ffmpeg
fi

# 2. Compilar
echo "⚙️ Compilando Motor Algébrico com Otimização Máxima (-O3)..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 3. Instalar Binário no PATH para uso global
echo "🚀 Disponibilizando comando global 'martelos'..."
if [ "$OS" = "Linux" ]; then
    sudo mv martelos $PREFIX/bin/
else
    mv martelos $PREFIX/bin/
fi

# 4. Criar Pastas de Trabalho
echo "📁 Configurando diretórios de soberania em ~/Downloads/martelos/..."
BASE_DIR="$HOME/Downloads/martelos"
mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
mkdir -p "$BASE_DIR/textos_salvos"

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. v5.8.0 INSTALADO COM SUCESSO!"
echo "Digite 'martelos' em qualquer lugar para operar."
echo "================================================="

#!/bin/bash
set -e

# 1. Detecção de Ambiente
if [ -d "/data/data/com.termux/files/usr/bin" ]; then
    OS="Android-Termux"
    BIN_DIR="/data/data/com.termux/files/usr/bin"
    PKG_MGR="pkg install -y"
    SUDO=""
    # Configuração de Armazenamento para Android
    if [ ! -d "$HOME/storage" ]; then
        echo "⚠️  AVISO: O Termux nao tem acesso ao armazenamento do celular."
        echo "Por favor, digite 'termux-setup-storage' e autorize antes de continuar."
        exit 1
    fi
    BASE_DIR="$HOME/storage/downloads/martelos"
else
    OS="Linux-Desktop"
    BIN_DIR="/usr/local/bin"
    PKG_MGR="sudo apt install -y"
    SUDO="sudo"
    BASE_DIR="$HOME/Downloads/martelos"
fi

echo "🔨 Instalando M.A.R.T.E.L.O.S. v6.2.6 no $OS..."

# 2. Instalação de Dependências
echo "📦 Verificando dependencias..."
if [ "$OS" = "Linux-Desktop" ]; then
    sudo apt update
    $PKG_MGR libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    $PKG_MGR libgmp cmake clang git ffmpeg
fi

# 3. Compilação de Alta Performance
echo "⚙️  Compilando Motor Algebrico (-O3 Optimization)..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 4. Instalação Global do Binário
echo "🚀 Movendo binario para $BIN_DIR..."
$SUDO mv martelos $BIN_DIR/
$SUDO chmod +x $BIN_DIR/martelos

# 5. Configuração da Bancada de Trabalho Pública
echo "📁 Organizando pastas em $BASE_DIR..."
mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
mkdir -p "$BASE_DIR/textos_salvos"

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. INSTALADO COM SUCESSO!"
echo "================================================="
echo "Caminho da Bancada: $BASE_DIR"
echo "Como operar:"
echo "-> 'martelos' (Menu Interativo)"
echo "-> 'martelos --help' (Manual de Flags)"
echo "================================================="

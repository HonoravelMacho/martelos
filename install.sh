#!/bin/bash
set -e

# Detectar ambiente (Linux Desktop ou Android Termux)
if [ -d "/data/data/com.termux/files/usr/bin" ]; then
    OS="Android-Termux"
    PREFIX="/data/data/com.termux/files/usr"
    PKG_MGR="pkg install -y"
else
    OS="Linux"
    PREFIX="/usr/local"
    PKG_MGR="sudo apt install -y"
fi

echo "🔨 Iniciando Instalação M.A.R.T.E.L.O.S. v6.0.4..."
echo "🌐 Detectado: $OS"

# 1. Instalar Dependências (Agora incluindo FFmpeg como vital)
echo "📦 Atualizando dependências e motor de mídia..."
if [ "$OS" = "Linux" ]; then
    sudo apt update
    sudo apt install -y libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    pkg install -y libgmp cmake clang git ffmpeg
fi

# 2. Compilação de Alta Performance
echo "⚙️  Compilando Motor Algébrico v6.0.4 (-O3 Optimization)..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 3. Registro Global do Comando (A mágica para usar as flags em qualquer lugar)
echo "🚀 Instalando binário no sistema..."
if [ "$OS" = "Linux" ]; then
    sudo mv martelos $PREFIX/bin/
    sudo chmod +x $PREFIX/bin/martelos
else
    mv martelos $PREFIX/bin/
    chmod +x $PREFIX/bin/martelos
fi

# 4. Criação da Estrutura de Bancada (Modo Interativo)
echo "📁 Organizando pastas de soberania..."
BASE_DIR="$HOME/Downloads/martelos"
mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
mkdir -p "$BASE_DIR/textos_salvos"

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. v6.0.4 INSTALADO COM SUCESSO!"
echo "================================================="
echo "Como operar:"
echo "1. Menu: Digite 'martelos'"
echo "2. CLI:  Digite 'martelos -c -s \"#texto#\" ...'"
echo "3. Ajuda: Digite 'martelos --help'"
echo "================================================="

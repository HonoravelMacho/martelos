#!/bin/bash
set -e

# 1. Detecção de Ambiente
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

echo "🔨 Instalando M.A.R.T.E.L.O.S. v6.2.7 no $OS..."

# 2. Instalação de Dependências
if [ "$OS" = "Linux-Desktop" ]; then
    sudo apt update
    $PKG_MGR libgmp-dev cmake build-essential git ffmpeg
else
    pkg update
    $PKG_MGR libgmp cmake clang git ffmpeg
fi

# 3. Compilação de Alta Performance
echo "⚙️ Compilando Motor Algébrico..."
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm || clang -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm

# 4. Instalação Global
echo "🚀 Movendo binário para $BIN_DIR..."
$SUDO mv martelos $BIN_DIR/
$SUDO chmod +x $BIN_DIR/martelos

# 5. Configuração da Bancada Pública (Foco Android)
if [ "$OS" = "Android-Termux" ]; then
    echo "🔗 Criando ponte para pasta pública 'Download/martelos'..."
    # Cria a pasta física no storage real do Android
    mkdir -p ~/storage/downloads/martelos/{entrada,saida,textos_salvos}
    # Cria o link simbólico que engana o código C
    ln -sf ~/storage/downloads ~/Downloads
else
    mkdir -p ~/Downloads/martelos/{entrada,saida,textos_salvos}
fi

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. v6.2.7 INSTALADO COM SUCESSO!"
echo "Operação transparente: Downloads/martelos ativa."
echo "================================================="

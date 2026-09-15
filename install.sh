#!/bin/bash
set -e

# 1. Detecção de Ambiente
if [ -d "/data/data/com.termux/files/usr/bin" ] || [ -n "$TERMUX_VERSION" ]; then
    OS="Android-Termux"
    BIN_DIR="${PREFIX:-/data/data/com.termux/files/usr}/bin"
    PKG_MGR="pkg install -y"
    SUDO=""
else
    OS="Linux-Desktop"
    BIN_DIR="/usr/local/bin"
    PKG_MGR="sudo apt install -y"
    SUDO="sudo"
fi

echo "🔨 Instalando M.A.R.T.E.L.O.S. v6.3.2 no $OS..."

# 2. Instalação de Dependências
# CORREÇÃO v6.3.2: no Termux o pacote é "libgmp" (não "gmp") e o ffprobe
# acompanha o pacote "ffmpeg". Sem isso o instalador quebrava no Android.
if [ "$OS" = "Linux-Desktop" ]; then
    sudo apt update
    $PKG_MGR libgmp-dev cmake build-essential git ffmpeg
else
    pkg update -y
    $PKG_MGR libgmp clang git ffmpeg
    command -v ffmpeg >/dev/null || { echo "[❌] ffmpeg não instalado. Rode: pkg install -y ffmpeg"; exit 1; }
    command -v ffprobe >/dev/null && echo "✅ ffprobe OK." || echo "⚠️ ffprobe ausente — validação de mídia simplificada."
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

# 6. Ícone e tipo MIME dos arquivos .martelos (2 martelos cruzados)
# No Termux não há /usr/share nem sudo: pula esta etapa (o .martelos segue funcional).
if [ "$OS" = "Linux-Desktop" ]; then
echo "🎨 Registrando ícone dos arquivos .martelos..."
$SUDO cp martelos-mime.xml /usr/share/mime/packages/martelos.xml
$SUDO mkdir -p /usr/share/icons/hicolor/scalable/mimetypes /usr/share/icons/hicolor/48x48/mimetypes
$SUDO cp martelos-icon.svg /usr/share/icons/hicolor/scalable/mimetypes/application-x-martelos.svg
$SUDO cp martelos-icon-48.png /usr/share/icons/hicolor/48x48/mimetypes/application-x-martelos.png
$SUDO update-mime-database /usr/share/mime
$SUDO gtk-update-icon-cache -f /usr/share/icons/hicolor > /dev/null 2>&1 || true
else
echo "ℹ️ Termux: registro de ícone/MIME ignorado (sem /usr/share no Android)."
fi

echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. v6.3.2 INSTALADO COM SUCESSO!"
echo "Operação transparente: Downloads/martelos ativa."
echo "================================================="

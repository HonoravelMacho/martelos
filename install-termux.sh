#!/bin/bash
# Instalador do M.A.R.T.E.L.O.S. para Android/Termux.
# Uso no Termux: bash install-termux.sh
set -e
echo "🔨 Instalando M.A.R.T.E.L.O.S. no Termux..."
pkg update
pkg install -y clang gmp ffmpeg git
cd ~
rm -rf ~/martelos_temp
git clone --depth 1 https://github.com/HonoravelMacho/martelos.git ~/martelos_temp
cd ~/martelos_temp
clang -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm
cp martelos "$PREFIX/bin/" && chmod +x "$PREFIX/bin/martelos"
mkdir -p ~/Downloads/martelos/textos_salvos
hash -r
echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. INSTALADO NO TERMUX!"
echo "O modo por números usa a pasta onde o terminal está aberto."
echo "🚀 Digite: martelos"
echo "================================================="

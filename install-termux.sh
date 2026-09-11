#!/bin/bash
# Instalador/ATUALIZADOR do M.A.R.T.E.L.O.S. para Android/Termux.
# Um único comando instala a versão mais recente do GitHub ou atualiza a antiga:
#   curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install-termux.sh | bash
set -e
echo "🔨 M.A.R.T.E.L.O.S. no Termux (instalação/atualização)..."
pkg update
pkg install -y clang gmp ffmpeg git
cd ~
rm -rf ~/martelos_temp
git clone --depth 1 https://github.com/HonoravelMacho/martelos.git ~/martelos_temp
cd ~/martelos_temp
clang -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm
echo "🧪 Validando o binário recém-compilado..."
./martelos -c -s "#OK#" -k 1 "#S#" -f pi -print | grep -q "RESULTADO" || { echo "[❌] Falha no teste do motor."; exit 1; }
rm -f "$PREFIX/bin/martelos"
cp martelos "$PREFIX/bin/" && chmod +x "$PREFIX/bin/martelos"
mkdir -p ~/Downloads/martelos/textos_salvos
hash -r
echo "🔍 Validando o comando global 'martelos'..."
command -v martelos > /dev/null || { echo "[❌] 'martelos' não entrou no PATH."; exit 1; }
martelos --help > /dev/null || { echo "[❌] 'martelos' instalado mas não executa."; exit 1; }
echo "================================================="
echo "✅ M.A.R.T.E.L.O.S. ATUALIZADO NO TERMUX!"
echo "O modo por números usa a pasta onde o terminal está aberto."
echo "🚀 Digite em qualquer pasta: martelos"
echo "================================================="

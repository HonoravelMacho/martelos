#!/bin/bash
set -e  # Isso faz o script parar imediatamente se qualquer comando der erro

# Detectar ambiente
if [ -d "/data/data/com.termux/files/usr/bin" ]; then
    OS="Android"
    BIN_DIR="/data/data/com.termux/files/usr/bin"
    PKG_MGR="pkg install -y"
else
    OS="Linux"
    BIN_DIR="/usr/local/bin"
    PKG_MGR="sudo apt install -y"
fi

echo "🔨 Instalando/Atualizando M.A.R.T.E.L.O.S. no $OS..."

# 1. Instalar dependências
if [ "$OS" = "Linux" ]; then
    $PKG_MGR libgmp-dev cmake build-essential git
else
    $PKG_MGR libgmp cmake clang git
fi

# 2. Atualizar código
if [ -d "martelos_temp" ]; then rm -rf martelos_temp; fi
git clone --depth 1 https://github.com/HonoravelMacho/martelos.git martelos_temp
cd martelos_temp

# 3. Compilar
mkdir build && cd build
cmake ..
make

# 4. Instalar
if [ "$OS" = "Linux" ]; then
    sudo cp martelos $BIN_DIR/
else
    cp martelos $BIN_DIR/
fi

# --- CRIAÇÃO DAS PASTAS DE TRABALHO M.A.R.T.E.L.O.S. ---
echo "🔨 Configurando bancada de trabalho..."
if [ -d "$HOME/storage/downloads" ]; then
    # Android Termux
    BASE_DIR="$HOME/storage/downloads/martelos"
else
    # Linux Desktop
    BASE_DIR="$HOME/Downloads/martelos"
fi

mkdir -p "$BASE_DIR/entrada"
mkdir -p "$BASE_DIR/saida"
echo "✅ Pastas criadas com sucesso em: $BASE_DIR"

echo "✅ M.A.R.T.E.L.O.S. pronto! Digite 'martelos' para rodar."
cd ../.. && rm -rf martelos_temp

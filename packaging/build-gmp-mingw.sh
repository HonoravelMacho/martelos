#!/bin/bash
# Compila o GMP estático para Windows (MinGW-w64 x86_64).
# Uso: build-gmp-mingw.sh <prefixo> [jobs]
set -e
PREFIX="$1"
JOBS="${2:-$(nproc)}"
GMP_VER="6.3.0"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cd "$WORK"
curl -sSL "https://ftp.gnu.org/gnu/gmp/gmp-${GMP_VER}.tar.xz" -o gmp.tar.xz
tar xf gmp.tar.xz
cd "gmp-${GMP_VER}"
./configure --host=x86_64-w64-mingw32 --disable-shared --enable-static \
  --prefix="$PREFIX" --disable-assembly || \
./configure --host=x86_64-w64-mingw32 --disable-shared --enable-static \
  --prefix="$PREFIX"
make -j"$JOBS"
make install
echo "GMP mingw instalado em $PREFIX"

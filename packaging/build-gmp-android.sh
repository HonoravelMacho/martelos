#!/bin/bash
# Compila o GMP estático para Android ARM64 usando o NDK.
# Uso: build-gmp-android.sh <ndk_dir> <api> <prefixo> [jobs]
# Ex: build-gmp-android.sh $HOME/android-sdk/ndk/27.0.12077973 24 /tmp/gmp-android
set -e
NDK="$1"
API="$2"
PREFIX="$3"
JOBS="${4:-$(nproc)}"
GMP_VER="6.3.0"
TOOLCHAIN="$NDK/toolchains/llvm/prebuilt/linux-x86_64"
export CC="$TOOLCHAIN/bin/aarch64-linux-android${API}-clang"
export AR="$TOOLCHAIN/bin/llvm-ar"
export RANLIB="$TOOLCHAIN/bin/llvm-ranlib"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
cd "$WORK"
curl -sSL "https://ftp.gnu.org/gnu/gmp/gmp-${GMP_VER}.tar.xz" -o gmp.tar.xz
tar xf gmp.tar.xz
cd "gmp-${GMP_VER}"
./configure --host=aarch64-linux-android --disable-shared --enable-static \
  --prefix="$PREFIX" --with-sysroot="$TOOLCHAIN/sysroot" || \
./configure --host=aarch64-linux-android --disable-shared --enable-static \
  --prefix="$PREFIX"
make -j"$JOBS"
make install
echo "GMP android-arm64 instalado em $PREFIX"

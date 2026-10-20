#!/bin/bash
# Teste de COR de vídeo do M.A.R.T.E.L.O.S. (v6.3.7+).
# Um comando, zero dependência de arquivos do usuário: usa fonte sintética (testsrc2).
# Fluxo fiel ao original:
#   encrypt: cria amostra_glitch.avi (AVI cru caótico = a cifra visível)
#   decrypt: devolve amostra_glitch.avi_fix.mp4 (cor CORRETA garantida)
# Copia tudo para ~/storage/downloads para conferir na Galeria/VLC.
# OBS: no VLC Android o *.glitch.avi cru pode exibir avermelhado — limitação
# conhecida do player móvel com AVI sem compressão (o PC exibe o caçõs cinza).
# Uso: curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/teste-cor-video.sh | bash
set -e
echo "🔨 Teste de cor do M.A.R.T.E.L.O.S. no Termux..."
command -v martelos >/dev/null || { echo "[❌] martelos não está no PATH (rode primeiro o instalador)."; exit 1; }
command -v ffmpeg   >/dev/null || { echo "[❌] ffmpeg ausente (pkg install -y ffmpeg)."; exit 1; }
cd ~
rm -f amostra.mp4 amostra_glitch.avi amostra_glitch.avi_fix.mp4 amostra_quadro.png
echo "✂️  Gerando amostra sintética de 2s (testsrc2, 320p)..."
if ! ffmpeg -v error -f lavfi -i testsrc2=size=320x240:rate=25:duration=2 \
            -f lavfi -i sine=frequency=440:duration=2 \
            -c:v libx264 -crf 23 -c:a aac -pix_fmt yuv420p -y amostra.mp4; then
  ffmpeg -v error -f lavfi -i testsrc2=size=320x240:rate=25:duration=2 \
         -f lavfi -i sine=frequency=440:duration=2 \
         -c:v mpeg4 -q:v 6 -c:a aac -pix_fmt yuv420p -y amostra.mp4
fi
ls -l amostra.mp4
echo "🔒 Criptografando com o martelos instalado..."
martelos -c -v amostra.mp4 -k 1 "#teste#" -f pi
vcodec=$(ffprobe -v error -select_streams v:0 -show_entries stream=codec_name -of default=noprint_wrappers=1:nokey=1 amostra_glitch.avi 2>/dev/null)
echo "vídeo do glitch: $vcodec"
if [ -n "$vcodec" ]; then
  echo "✅ Criptografia OK: o *_glitch.avi é o AVI cru caótico (a cifra visível)."
else
  echo "================================================="
  echo "[❌] PROBLEMA: o *_glitch.avi não foi criado direito."
  echo "    Envie esta saída inteira."
  echo "================================================="
  exit 1
fi
echo "🔓 Descriptografando (deve criar o *_fix.mp4 com a cor certa)..."
martelos -d -v amostra_glitch.avi -k 1 "#teste#" -f pi
FIX=$(find ~ -maxdepth 1 -name 'amostra_glitch*fix.mp4' 2>/dev/null | head -1)
if [ -n "$FIX" ]; then
  ffmpeg -v error -i "$FIX" -frames:v 1 -y fix_quadro.png
  echo "✅ Restauração em MP4 criada: $FIX"
else
  echo "[⚠️] O arquivo *_fix.mp4 não apareceu (binário antigo? rode o instalador primeiro)."
fi
ffmpeg -v error -i amostra_glitch.avi -frames:v 1 -y amostra_quadro.png
cp -f amostra_quadro.png fix_quadro.png "$FIX" ~/storage/downloads/ 2>/dev/null || true
cp -f amostra_glitch.avi ~/storage/downloads/ 2>/dev/null || true
echo ""
echo "================================================="
echo "📝 COMO CONFERIR (tudo em ~/storage/downloads):"
echo "  1) 'amostra_glitch.avi' no PC (VLC Linux) → caos cinza fiel ao original."
echo "     No VLC Android pode exibir avermelhado/preto — limitação conhecida"
echo "     do player móvel com AVI cru (a estética real está no arquivo)."
echo "  2) 'amostra_glitch.avi_fix.mp4' no VLC → deve mostrar a IMAGEM-TESTE com"
echo "     a cor CERTA. Toda descriptografia de vídeo sai .mp4 assim."
echo "================================================="

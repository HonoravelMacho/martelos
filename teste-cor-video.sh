#!/bin/bash
# Teste de COR de vídeo do M.A.R.T.E.L.O.S. no Termux (v6.3.5+).
# Um comando, zero dependência de arquivos do usuário: usa uma fonte
# sintética (testsrc2) do próprio ffmpeg. Criptografa, descriptografa
# (gerando o *_fix.mp4 universal) e copia os resultados para Downloads:
#   amostra_glitch.avi          (criptografado — formato AVI cru)
#   amostra_glitch.avi_fix.mp4  (re-codificado — cor CORRETA em qualquer player)
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
martelos -c -v amostra.mp4 -k 1 "#teste#" -f pi -o amostra_glitch.avi
fmt=$(ffprobe -v error -select_streams v:0 -show_entries stream=pix_fmt -of default=noprint_wrappers=1:nokey=1 amostra_glitch.avi 2>/dev/null)
echo "pix_fmt do glitch: $fmt"
if [ "$fmt" = "bgr24" ]; then
  echo "✅ Criptografia OK (bgr24 = caos CINZA correto)."
else
  echo "================================================="
  echo "[❌] PROBLEMA: glitch saiu em '$fmt' (esperado: bgr24)."
  echo "    Envie esta saída inteira."
  echo "================================================="
  exit 1
fi
echo "🔓 Descriptografando (deve criar o *_fix.mp4 universal)..."
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
echo ""
echo "================================================="
echo "📝 COMO CONFERIR (tudo em ~/storage/downloads):"
echo "  1) 'amostra_quadro.png' na GALERIA → deve ser estática cinza puro."
echo "  2) 'amostra_glitch.avi' no VLC → é o AVI cru: alguns players Android"
echo "     exibem com tintura avermelhada (limitação de exibição, não é o arquivo)."
echo "  3) TESTE QUE IMPORTA: 'amostra_glitch.avi_fix.mp4' no VLC → deve exibir"
echo "     a IMAGEM-TESTE colorida com a COR CERTA. A partir de agora TODA"
echo "     descriptografia de vídeo sai .mp4 assim."
echo "================================================="

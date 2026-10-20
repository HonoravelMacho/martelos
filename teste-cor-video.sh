#!/bin/bash
# Teste de COR de vídeo do M.A.R.T.E.L.O.S. no Termux (v6.3.5+).
# Um comando: cria amostra de 2s do maior vídeo da home, criptografa,
# descriptografa (que agora gera *_fix.mp4 universal) e copia tudo
# para a pasta Downloads para comparar no VLC:
#   amostra_glitch.avi          (criptografado — formato AVI cru)
#   amostra_glitch.avi_fix.mp4  (re-codificado — cor CORRIGIDA em qualquer player)
# Uso: curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/teste-cor-video.sh | bash
set -e
echo "🔨 Teste de cor do M.A.R.T.E.L.O.S. no Termux..."
command -v martelos >/dev/null || { echo "[❌] martelos não está no PATH (rode primeiro o instalador)."; exit 1; }
command -v ffmpeg   >/dev/null || { echo "[❌] ffmpeg ausente (pkg install -y ffmpeg)."; exit 1; }
FONTE=$(find ~ -maxdepth 1 -type f \( -iname '*.mp4' -o -iname '*.mkv' -o -iname '*.avi' -o -iname '*.mov' -o -iname '*.webm' \) 2>/dev/null | head -1)
if [ -z "$FONTE" ]; then
  echo "[⚠️] Nenhum vídeo na home do Termux (~). Copie um e rode de novo, ex.:"
  echo "    cp ~/storage/downloads/filme.mp4 ~/Filme.mp4"
  exit 1
fi
echo "🎬 Fonte: $FONTE"
cd ~
rm -f amostra.mp4 amostra_glitch.avi amostra_glitch.avi_fix.mp4 amostra_quadro.png
echo "✂️  Montando amostra de 2s em 320p..."
ffmpeg -v error -ss 60 -t 2 -i "$FONTE" -vf scale=320:-2 -c:v libx264 -crf 23 -c:a aac -pix_fmt yuv420p -y amostra.mp4 || \
ffmpeg -v error -ss 60 -t 2 -i "$FONTE" -vf scale=320:-2 -c:v mpeg4 -q:v 6 -c:a aac -pix_fmt yuv420p -y amostra.mp4
echo "🔒 Criptografando com o martelos instalado..."
martelos -c -v amostra.mp4 -k 1 "#teste#" -f pi -o amostra_glitch.avi
fmt=$(ffprobe -v error -select_streams v:0 -show_entries stream=pix_fmt -of default=noprint_wrappers=1:nokey=1 amostra_glitch.avi 2>/dev/null)
echo "pix_fmt do glitch: $fmt"
if [ "$fmt" = "bgr24" ]; then
  echo "✅ Arquivo de criptografia OK (bgr24 = caos CINZA correto)."
else
  echo "================================================="
  echo "[❌] PROBLEMA: glitch saiu em '$fmt' (esperado: bgr24)."
  echo "    Envie esta saída inteira."
  echo "================================================="
  exit 1
fi
echo "🔓 Descriptografando (deve criar o *.mp4 universal)..."
martelos -d -v amostra_glitch.avi -k 1 "#teste#" -f pi
FIX=$(find ~ -maxdepth 1 -name 'amostra_glitch*fix.mp4' 2>/dev/null | head -1)
if [ -n "$FIX" ]; then
  echo "✅ Restauração em MP4 criada: $FIX"
else
  echo "[⚠️] O arquivo *_fix.mp4 não apareceu (binário antigo? instale primeiro)."
fi
ffmpeg -v error -i amostra_glitch.avi -frames:v 1 -y amostra_quadro.png
cp -f amostra_quadro.png amostra_glitch.avi ~/storage/downloads/ 2>/dev/null || true
[ -n "$FIX" ] && cp -f "$FIX" ~/storage/downloads/ 2>/dev/null || true
echo ""
echo "================================================="
echo "📝 COMO CONFERIR (tudo já está em ~/storage/downloads):"
echo "  1) quadro_teste: 'amostra_quadro.png' na GALERIA → deve ser cinza puro."
echo "  2) 'amostra_glitch.avi' no VLC → é o AVI cru (pode mostrar rosa/vermelho;"
echo "     é limitação do player Android com AVI não comprimido)."
echo "  3) 'amostra_glitch.avi_fix.mp4' no VLC → deve ter a COR CORRETA do vídeo"
echo "     (a partir de agora TODA descriptografia de vídeo gera .mp4)."
echo "================================================="

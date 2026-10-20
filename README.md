# 🔨 Projeto M.A.R.T.E.L.O.S.
**Motor Algébrico Ruptivo Transformacional Experimental Livre e Open Source**

![Status](https://img.shields.io/badge/Status-Estável-green)
![Versão](https://img.shields.io/badge/Versão-v6.3.7-blue)
![Licença](https://img.shields.io/badge/Licença-Apache%202.0-yellow)

M.A.R.T.E.L.O.S. é um manifesto técnico pela **Soberania Algébrica**. Em um mundo de vigilância constante, o projeto foca em segurança **Off-Grid**, autonomia total e na entropia pura extraída de constantes transcendentais do universo.

---

## 🚀 Como Instalar (Comando Único)

O script de instalação detecta automaticamente seu sistema, instala as dependências (libgmp, ffmpeg), compila o motor com otimização máxima e configura as pastas de trabalho públicas.

### 🐧 Linux (Ubuntu/Debian/Pop_OS)
`curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash`
Outras distros: use o pacote `.deb`/`.rpm` ou o tarball estático da [página de Releases](https://github.com/HonoravelMacho/martelos/releases).

### 🍎 macOS (via Homebrew)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"  # só na 1ª vez, se não tiver o brew
curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash
```
O instalador detecta o macOS, instala `gmp` e `ffmpeg` pelo brew e compila com clang. Requer `ffmpeg` no PATH para mídia (o instalador cuida disso).

### 🪟 Windows (dois caminhos)
**Caminho 1 — Binário pronto:** baixe o `martelos-*.exe` na [página de Releases](https://github.com/HonoravelMacho/martelos/releases) e deixe-o numa pasta do PATH. Para criptografar mídia, instale o **ffmpeg** (ex.: winget: `winget install ffmpeg`, ou baixe em ffmpeg.org), o martelos usa o `ffmpeg` e `ffprobe` do PATH.

**Caminho 2 — Compilar (MSYS2, [msys2.org](https://www.msys2.org)):**
```bash
pacman -S --noconfirm mingw-w64-x86_64-gcc mingw-w64-x86_64-gmp mingw-w64-x86_64-ffmpeg git
git clone https://github.com/HonoravelMacho/martelos.git
cd martelos
gcc -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos.exe -lgmp -lm
```

### 📱 Android (Termux) — 3 caminhos, do mais fácil ao manual

**Caminho 1 — Script oficial (instala OU atualiza, com validação):**
```bash
curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install-termux.sh | bash
```
Um comando só: baixa a versão mais recente do GitHub, compila, testa o motor, instala no `$PREFIX/bin` e valida que `martelos` funciona em qualquer pasta. Pode rodar de novo quando sair versão nova.

**Caminho 2 — Binário pronto (sem compilar):**
Baixe o `martelos-*-android-arm64.tar.gz` na [página de Releases](https://github.com/HonoravelMacho/martelos/releases), extraia no Termux e mova para o PATH:
```bash
tar xzf martelos-*-android-arm64.tar.gz
cp martelos $PREFIX/bin/ && chmod +x $PREFIX/bin/martelos
```

**Caminho 3 — Manual:**
```bash
termux-setup-storage
pkg update && pkg install -y clang libgmp ffmpeg git
git clone --depth 1 https://github.com/HonoravelMacho/martelos.git ~/martelos_temp
cd ~/martelos_temp
clang -O3 main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c -o martelos -lgmp -lm
cp martelos $PREFIX/bin/ && chmod +x $PREFIX/bin/martelos
```
> **Nota v6.3.3 (Termux):** o pacote correto é `libgmp` (o antigo `gmp` não existe e quebrava a instalação). O `ffprobe` já vem junto com o pacote `ffmpeg`. Se a mídia falhar, rode `ffmpeg -version` e `ffprobe -version` para conferir; o martelos agora mostra o log real do ffmpeg em vez de um erro genérico. O log usa `$TMPDIR` (`$PREFIX/tmp` no Termux) porque o `/tmp` do Android não tem escrita.

> **Nota v6.3.7 (vídeo, todos os sistemas): voltou a PUREZA.** O VÍDEO criptografado é **`*_glitch.avi` cru** — a cifra visível (quadros cinza embaralhados + áudio cru), como no `.bmp` e no `.wav`. **Aviso honesto:** no PC o caos aparece perfeito; alguns players móveis exibem esses AVIs sem compressão avermelhado/preto — ** teste players alternativos da Play Store: executam o `*_glitch.avi` perfeitamente (comprovado)**; o VLC Android é a exceção conhecida (limitação dele, não da criptografia). Na descriptografia o martelos restaura e converte o vídeo para `*_fix.mp4` (H.264 + yuv420p) — cor correta em qualquer player — e ainda aceita descriptografar os `*_glitch.mp4` do legado 6.3.5/6.3.6. Para diagnosticar no Termux: `curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/teste-cor-video.sh | bash`

**Pré-requisito (só na 1ª vez):** autorize o acesso à memória com `termux-setup-storage`.

---
### 📱 Dica de Ouro para Android (Termux)
O modo por números do M.A.R.T.E.L.O.S. opera **na pasta onde o terminal foi aberto** — entre na pasta dos seus arquivos com `cd` antes de digitar `martelos`. Exemplo com a pasta pública de Downloads:

1. Se você não encontrar os arquivos, execute: `termux-setup-storage`
2. Navegue até eles: `cd ~/storage/downloads`
3. Rode `martelos` ali — cripto e descripto nascem na mesma pasta, sem cópia manual.

## 🛠️ Manual de Operação CLI (Flags)

No modo CLI (Flags), os arquivos são processados na pasta local `./` onde o terminal está aberto, facilitando a integração com scripts.

**REGRA DE OURO:** No terminal, envolva textos e chaves que contenham espaços em aspas e cerquilhas: `" #texto aqui# "`

| Flag | Função |
| :--- | :--- |
| `-c / -d` | Criptografar / Descriptografar |
| `-s` | Alvo: **String/Texto** (Envolva em "#...#") |
| `-i` | Alvo: **Imagem** (Transmuta para .bmp) |
| `-a` | Alvo: **Áudio** (Transmuta para .wav) |
| `-v` | Alvo: **Vídeo** (Transmuta para AVI cru caótico: `*_glitch.avi`) |
| `-m` | Alvo: **Qualquer arquivo** (gera `.martelos` com extensão guardada) |
| `-o` | **Saída**: Nome do arquivo gerado (padrão: `*_glitch.*`) |
| `-save` | Salvar resultado de texto em `.txt` |
| `-k` | **Chaves**: Informe a Qtd e as frases entre "#...#" |
| `-f` | **Fórmula**: Constante (pi, catalan, etc) ou fórmula customizada |

---

## ♾️ Constantes e Fórmulas (Motor BBP)

O motor utiliza a biblioteca **GNU MP (GMP)** para gerar milhões de dígitos irracionais que servem como esteira de deslocamento dinâmico.

### Constantes Nativas (`-f`):
1. **pi**: A clássica constante circular (A mais rápida e otimizada).
2. **catalan**: Constante de Catalan (Excelente para dispersão de bits).
3. **apery**: Zeta de 3 de Apéry (Alta complexidade irracional).
4. **clausen**: Função de Clausen (Ideal para transmutações de áudio).
5. **arctan**: Arco-tangente de 1 (Base Pi/4).
6. **log3**: Logaritmo natural de 3 (Entropia densa).

### Fórmulas Personalizadas (C-Style Syntax):
O Martelos aceita expressões complexas. Exemplos:
* `sqrt(pi) * 2`
* `sin(pi/2) + log(3)`
* `pi^2 / 6`
* `e + pi`

## 📦 Pacotes Prontos (Releases)

Cada tag `v*` gera automaticamente na [página de Releases](https://github.com/HonoravelMacho/martelos/releases):

| Arquivo | Plataforma |
| :--- | :--- |
| `.deb` | Debian/Ubuntu (`sudo dpkg -i martelos_*.deb`) |
| `.rpm` | Fedora/RHEL/openSUSE (`sudo rpm -i martelos-*.rpm`) |
| `.exe` | Windows (requer `ffmpeg` no PATH para mídia) |
| `-android-arm64.tar.gz` | Termux (ver Caminho 2 acima) |
| `-linux-x86_64-static.tar.gz` | Linux genérico, sem dependências |
| `.apk` | App companheiro Android (motor completo via Termux) |

---

## ⚖️ Matrix de Soberania: Martelos vs. Padrões Industriais

<table width="100%">
  <thead>
    <tr bgcolor="#222">
      <th align="center">💻 Algoritmo</th>
      <th align="left">⚡ Foco Principal</th>
      <th align="left">🛡️ Vantagem Martelos</th>
      <th align="left">⚠️  Desvantagem do Martelos</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td align="center"><b>AES-256</b></td>
      <td>Bloco / Militar</td>
      <td><b>Chaves dinâmicas sem IVs fixos</b></td>
      <td>AES possui aceleração em hardware</td>
    </tr>
    <tr>
      <td align="center"><b>RSA</b></td>
      <td>Chaves Públicas</td>
      <td><b>Imune a fatoração simples</b></td>
      <td>Exige canal seguro de troca manual</td>
    </tr>
    <tr>
      <td align="center"><b>ChaCha20</b></td>
      <td>Fluxo em Software</td>
      <td><b>N-Camadas auto-mutáveis</b></td>
      <td>Maior adoção de mercado</td>
    </tr>
    <tr>
      <td align="center"><b>PGP / GPG</b></td>
      <td>E-mails</td>
      <td><b>Transmutação e Glitch Art</b></td>
      <td>Ecossistema de e-mail pronto</td>
    </tr>
    <tr>
      <td align="center"><b>OTP</b></td>
      <td>Segurança Absoluta</td>
      <td><b>Entropia de chaves curtas</b></td>
      <td>Exige chave do tamanho do dado</td>
    </tr>
    <tr>
      <td align="center"><b>Twofish</b></td>
      <td>Flexibilidade</td>
      <td><b>Zero metadados em arquivos</b></td>
      <td>Documentação acadêmica vasta</td>
    </tr>
  </tbody>
</table>

---

## 📜 Licença e Filosofia
Distribuído sob a **Apache Software License 2.0**. 

O M.A.R.T.E.L.O.S. é focado em **Segurança de Resistência**. Assume-se que o operador não confia em infraestruturas centralizadas e utiliza a matemática imutável do universo como escudo definitivo.

## 🏛️ Filosofia: A Cifra Exposta — Quiasmos, Parábolas e o Oceano de Cantor

### O quiasmo: ensinar ocultando, ocultar revelando
Um **quiasmo** é uma estrutura em espelho (A-B-C-C-B-A) — organização que esconde a ordem dentro de si para quem não sabe olhar, e a revela para quem tem a chave de leitura. As Escrituras falam por quiasmos: **Apocalipse**, **Daniel**, **Lamentações de Jeremias** e outros são construídos em espelhos literários que atravessam milênios sem serem percebidos por quem lê a seco. Jesus ensinava por **parábolas** — e quando perguntaram por que Ele falava assim, respondeu citando **Isaías 6:9-10**: *"Vendo, não veem; ouvindo, não entendem"* (Mateus 13:13-14, Marcos 4:12).

O Martelos é esse princípio transformado em código: **o arquivo criptografado fica em plena luz**. Nada é escondido — o texto embaralhado, o BMP de chiado, o WAV de caos, o AVI cru: tudo exposto, desafiando qualquer um a tentar entender. Mas **só quem tem as chaves e a constante** faz o arquivo voltar a ser o que era. A quem não tem a chave, o conteúdo fala como parábola: visível, contudo ilegível; audível, contudo ruidoso. Não é segurança por obscuridade — é **segurança por soberania**: o segredo não depende do esconderijo, depende da chave.

### O oceano de Cantor
George Cantor provou que os irracionais são **incontáveis**: há mais números num pedaço da reta real do que em todos os racionais do universo. Os racionais — aquilo que contamos, tabulamos, prevemos — são uma **gota**; o resto é um oceano que nenhuma tabela jamais drenará.

O Martelos bebe do oceano. A esteira numérica (BBP) extraída de π, Catalan, Apery, Clausen, arctan, log3 — ou da **fórmula que você mesmo escrever** — busca o caos justamente no que **não é racional**: dígitos que não se repetem em ciclo, não se tabulam, não acabam. É por isso que cada par *chaves + constante* abre uma cifra diferente: **cada indivíduo do planeta pode ter infinitas criptografias próprias** — todas nascendo da mesma matemática imutável, todas reversíveis, todas suas.

   > *"O infinito dos irracionais é maior que o dos racionais. O Martelos tira o caos do resto do oceano."*

### Cifra em três camadas (o triplo selo)
1. **A esteira** — milhões de dígitos irracionais: a areia do oceano: possuída por ninguém, visível por todos, decorável por ninguém.
2. **O quiasmo dinâmico** — seu passo na esteira é definido pela SUA frase (via casa do quiasmo) e pelo PRÓPRIO byte anterior: a rota muda enquanto caminha, como a leitura de uma parábola que muda conforme quem lê.
3. **A exposição** — o artefato fica público e esteticamente caótico (glitch art real, reversível). Quem tem olhos, vê; quem tem chaves, lê.

### Classificação honesta
O Martelos é **criptografia experimental**: é arte reversível, não um padrão auditado. Nenhuma cifra aqui promete competir com AES em prova formal — promete outra coisa: **propriedade**. Sua cifra é sua porque nasce da sua frase, da sua constante, da sua fixação pelo infinito. Estude-a, ataque-a, melhore-a — ela é sua.

> *"A matemática é a única língua que ninguém pode proibir."*

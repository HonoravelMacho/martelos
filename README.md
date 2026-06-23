# 🔨 Projeto M.A.R.T.E.L.O.S.
**Motor Algébrico Ruptivo Transformacional Experimental Livre e Open Source**

![Status](https://img.shields.io/badge/Status-Est%C3%A1vel-green)
![Versão](https://img.shields.io/badge/Vers%C3%A3o-v6.2.6-blue)
![Licença](https://img.shields.io/badge/Licen%C3%A7a-Apache%202.0-yellow)

M.A.R.T.E.L.O.S. é um manifesto técnico pela **Soberania Algébrica**. Em um mundo de vigilância constante, o projeto foca em segurança **Off-Grid**, autonomia total e na entropia pura extraída de constantes transcendentais do universo.

---

## 🚀 Como Instalar (Comando Único)

O script de instalação detecta automaticamente seu sistema, instala as dependências (libgmp, ffmpeg), compila o motor com otimização máxima e configura as pastas de trabalho públicas.

### 🐧 Linux (Ubuntu/Debian/Pop_OS)
`curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash`

### 📱 Android (Termux)
**1. Autorize o acesso à memória:**
`termux-setup-storage`

**2. Execute a instalação:**
`curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash`

---

## 🛠️ Manual de Operação CLI (Flags)

O M.A.R.T.E.L.O.S. possui um motor híbrido. No modo CLI (Flags), os arquivos são processados na pasta local onde o terminal está aberto, facilitando a integração com scripts e automações.

**REGRA DE OURO:** No terminal, sempre envolva textos e chaves que contenham espaços em aspas e cerquilhas: `" #texto aqui# "`

| Flag | Função |
| :--- | :--- |
| `-c` | **Criptografar** |
| `-d` | **Descriptografar** |
| `-s` | Alvo: **String/Texto** (Envolva em "#...#") |
| `-i` | Alvo: **Imagem** (Auto-transmutação para .bmp) |
| `-a` | Alvo: **Áudio** (Auto-transmutação para .wav) |
| `-v` | Alvo: **Vídeo** (Transmuta para AVI Raw - Chiado v2.7) |
| `-m` | Alvo: **Qualquer Arquivo** (Gera extensão .martelos) |
| `-k` | **Chaves**: Informe a Qtd e as Chaves entre "#...#" |
| `-b` | **Base**: Atualmente fixo em **10** (Decimal) |
| `-f` | **Fórmula/Constante**: Constantes nativas ou fórmula customizada |

### EXEMPLO PRÁTICO:
```bash
# Criptografar um texto e exibir o resultado no terminal
martelos -c -s "#Ataque ao amanhecer#" -k 2 "#Senha1#" "#Senha2#" -b 10 -f pi -print

♾️ Constantes e Fórmulas (Motor BBP)
O motor utiliza a biblioteca GNU MP (GMP) para gerar milhões de dígitos irracionais que servem como uma esteira de deslocamento dinâmico e assimétrico.
Constantes Nativas (-f):
pi: A clássica constante circular (A mais rápida e otimizada).
catalan: Constante de Catalan (Excelente para dispersão de bits).
apery: Zeta de 3 de Apéry (Alta complexidade irracional).
clausen: Função de Clausen (Ideal para transmutações de áudio).
arctan: Arco-tangente de 1 (Base Pi/4).
log3: Logaritmo natural de 3 (Entropia densa).
Fórmulas Personalizadas (C-Style Syntax):
O M.A.R.T.E.L.O.S. aceita expressões matemáticas complexas via Parser. Exemplos de uso via flag -f "#fórmula#":
sqrt(pi) * 2
sin(pi/2) + log(3)
pi^2 / 6
e + pi
sqrt(2) * 1.4142

⚖️ Matrix de Soberania: Martelos vs. Padrões Industriais
O M.A.R.T.E.L.O.S. não busca competir em velocidade bruta, mas sim em resiliência matemática, privacidade offline e independência de autoridades certificadoras.
<table width="100%">
<thead>
<tr bgcolor="#222">
<th align="center">💻 Algoritmo</th>
<th align="left">⚡ Foco Principal</th>
<th align="left">🛡️ Vantagem Martelos</th>
<th align="left">⚠️ Trade-off</th>
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
<td>Maior adoção comercial</td>
</tr>
<tr>
<td align="center"><b>PGP / GPG</b></td>
<td>E-mails / Assinatura</td>
<td><b>Transmutação Multimídia Nativa</b></td>
<td>Ecossistema de e-mail pronto</td>
</tr>
<tr>
<td align="center"><b>OTP (One-Time)</b></td>
<td>Segurança Absoluta</td>
<td><b>Entropia de chaves curtas</b></td>
<td>Exige chave do tamanho do dado</td>
</tr>
<tr>
<td align="center"><b>Twofish</b></td>
<td>Flexibilidade</td>
<td><b>Zero metadados em modo .martelos</b></td>
<td>Documentação acadêmica vasta</td>
</tr>
</tbody>
</table>

📜 Licença e Filosofia
Distribuído sob a Apache Software License 2.0.
O M.A.R.T.E.L.O.S. é focado em Segurança de Resistência. Assume-se que o operador não confia em infraestruturas centralizadas e utiliza a matemática imutável do universo como escudo definitivo contra a observação indesejada.
"A matemática é a única língua que ninguém pode proibir."

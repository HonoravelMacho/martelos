# 🔨 Projeto M.A.R.T.E.L.O.S.
**Motor Algébrico Ruptivo Transformacional Experimental Livre e Open Source**

![Status](https://img.shields.io/badge/Status-Estável-green)
![Versão](https://img.shields.io/badge/Versão-v6.2.6-blue)
![Licença](https://img.shields.io/badge/Licença-Apache%202.0-yellow)

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

No modo CLI (Flags), os arquivos são processados na pasta local `./` onde o terminal está aberto, facilitando a integração com scripts.

**REGRA DE OURO:** No terminal, envolva textos e chaves que contenham espaços em aspas e cerquilhas: `" #texto aqui# "`

| Flag | Função |
| :--- | :--- |
| `-c / -d` | Criptografar / Descriptografar |
| `-s` | Alvo: **String/Texto** (Envolva em "#...#") |
| `-i` | Alvo: **Imagem** (Transmuta para .bmp) |
| `-a` | Alvo: **Áudio** (Transmuta para .wav) |
| `-v` | Alvo: **Vídeo** (Transmuta para AVI Raw - Chiado v2.7) |
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

---

## ⚖️ Matrix de Soberania: Martelos vs. Padrões Industriais

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

> *"A matemática é a única língua que ninguém pode proibir."*

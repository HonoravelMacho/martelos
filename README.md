# 🔨 Projeto M.A.R.T.E.L.O.S.
**Motor Algébrico Ruptivo Transformacional Experimental Livre e Open Source**

![Status](https://img.shields.io/badge/Status-Est%C3%A1vel-green)
![Versão](https://img.shields.io/badge/Vers%C3%A3o-v6.2.6-blue)
![Licença](https://img.shields.io/badge/Licen%C3%A7a-Apache%202.0-yellow)

M.A.R.T.E.L.O.S. não é apenas um software de criptografia; é um manifesto técnico pela **Soberania Algébrica**. Em um mundo de vigilância constante, o projeto foca em segurança **Off-Grid**, autonomia total e na entropia pura extraída de constantes transcendentais.

---

## 🚀 Como Instalar (Comando Único)

O script de instalação detecta automaticamente seu sistema, instala as dependências (libgmp, ffmpeg), compila o motor e configura as pastas de trabalho.

### 🐧 Linux (Ubuntu/Debian/Pop_OS)
```bash
curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash

📱 Android (Termux)
Primeiro, autorize o acesso à memória do celular para que as pastas fiquem na sua pasta de Downloads pública:

termux-setup-storage

Depois, execute a instalação:

curl -sSL https://raw.githubusercontent.com/HonoravelMacho/martelos/main/install.sh | bash

🛠️ Manual de Operação CLI (Flags)
O M.A.R.T.E.L.O.S. v6.2.x possui um motor híbrido. No modo CLI, os arquivos são processados na pasta local ./ onde o terminal está aberto.
REGRA DE OURO: No terminal, sempre envolva textos e chaves que contenham espaços em aspas e cerquilhas: " #texto aqui# "
SINTAXE:

martelos [operação] [alvo] [entrada] [parâmetros]

FLAGS PRINCIPAIS:

Flag	Função
-c	Criptografar
-d	Descriptografar
-s	Alvo: String/Texto (Use "#...#")
-i	Alvo: Imagem (Auto-transmutação para .bmp)
-a	Alvo: Áudio (Auto-transmutação para .wav)
-v	Alvo: Vídeo (Transmuta para AVI Raw - Chiado v2.7)
-m	Alvo: Qualquer Arquivo (Gera extensão .martelos)
-k	Chaves: Informe a Qtd e as Chaves entre "#...#"
-b	Base: Atualmente fixo em 10 (Decimal)
-f	Fórmula/Constante: pi, catalan, apery, clausen, arctan, log3
EXEMPLO PRÁTICO:

# Criptografar um texto e exibir o resultado no terminal
martelos -c -s "#Ataque ao amanhecer#" -k 2 "#Senha1#" "#Senha2#" -b 10 -f pi -print


♾️ Constantes e Fórmulas (Motor BBP)
O motor utiliza a biblioteca GNU MP (GMP) para gerar milhões de dígitos irracionais que servem como esteira de deslocamento dinâmico.
Constantes Nativas (-f):
pi: A clássica constante circular.
catalan: Constante de Catalan (Soma de séries).
apery: Zeta de 3 de Apéry.
clausen: Função de Clausen.
arctan: Arco-tangente (Base Pi/4).
log3: Logaritmo natural de 3.
Fórmulas Personalizadas (C-Style):
O Martelos aceita expressões complexas. Exemplos:
sqrt(pi) * 2
sin(pi/2) + log(3)
pi^2 / 6
e + pi


## ⚖️ Matrix de Soberania: Martelos vs. Padrões Industriais

O M.A.R.T.E.L.O.S. não busca competir em velocidade de servidor, mas em **resiliência matemática e autonomia**.

| Algoritmo | Força Principal | Vantagem do Martelos | Ponto de Atenção |
| :--- | :--- | :--- | :--- |
| **AES-256** | Velocidade Hardware | Sem vetores de inicialização fixos | AES é mais rápido para Big Data |
| **RSA** | Chaves Públicas | Imune a ataques de fatoração simples | Martelos exige troca manual de chaves |
| **ChaCha20** | Fluxo de Software | Complexidade N-Camadas Dinâmicas | Popularidade mundial |
| **PGP** | Emails | Transmutação Multimídia (Glitch Art) | Facilidade para usuários leigos |
| **OTP** | Inquebrável | Chaves curtas geram entropia transcendental | OTP exige chave do tamanho do dado |
| **Twofish** | Flexibilidade | Zero metadados em arquivos .martelos | Documentação acadêmica vasta |

📜 Licença e Filosofia
Distribuído sob a Apache Software License 2.0.
O M.A.R.T.E.L.O.S. é focado em Segurança de Resistência. Assume-se que o operador não confia em infraestruturas centralizadas e utiliza a matemática imutável do universo como escudo.
"A matemática é a única língua que ninguém pode proibir."


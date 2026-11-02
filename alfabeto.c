#include "martelos.h"

// ============================================================================
// FONTE LEGADA (v6): 34 símbolos. Somente os caminhos legados a usam —
// comportamento congelado pelos vetores-ouro do teste.c. NÃO ALTERAR.
// ============================================================================
const char ALFABETO_LEGADO[MODULO_LEGADO] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ','\n','.',',',';','?','!','-'};
int caractere_para_num_legado(char c) {
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 1);
    switch(c) { case ' ': return 27; case '\n': return 28; case '.': return 29; case ',': return 30; case ';': return 31; case '?': return 32; case '!': return 33; case '-': return 34; default: return -1; }
}
char num_para_caractere_legado(int num) {
    int pos = (num - 1) % MODULO_LEGADO; while (pos < 0) pos += MODULO_LEGADO; return ALFABETO_LEGADO[pos % MODULO_LEGADO];
}

// Normalização legada (v6): tudo para CAIXA ALTA; dígitos/símbolos viram '-'.
char* remover_acentos_e_upper(const char* texto) {
    long len = strlen(texto); char* res = malloc(len * 2 + 1); long r = 0;
    for (long i = 0; i < len; i++) {
        unsigned char c = (unsigned char)texto[i];
        if (c == 0xC3 && i+1 < len) {
            i++; unsigned char c2 = (unsigned char)texto[i];
            if ((c2 >= 0x80 && c2 <= 0x85) || (c2 >= 0xA0 && c2 <= 0xA5)) res[r++] = 'A';
            else if ((c2 >= 0x88 && c2 <= 0x8B) || (c2 >= 0xA8 && c2 <= 0xAB)) res[r++] = 'E';
            else if ((c2 >= 0x8C && c2 <= 0x8F) || (c2 >= 0xAC && c2 <= 0xAF)) res[r++] = 'I';
            else if ((c2 >= 0x92 && c2 <= 0x96) || (c2 >= 0xB2 && c2 <= 0xB6)) res[r++] = 'O';
            else if ((c2 >= 0x99 && c2 <= 0x9C) || (c2 >= 0xB9 && c2 <= 0xBC)) res[r++] = 'U';
            else if (c2 == 0x87 || c2 == 0xA7) res[r++] = 'C';
            else if (c2 == 0x83 || c2 == 0xA3) res[r++] = 'A';
            else if (c2 == 0x91 || c2 == 0xB1) res[r++] = 'N';
        } else {
            char up = toupper(c);
            if (caractere_para_num_legado(up) != -1) res[r++] = up;
            else if (isspace(c)) res[r++] = ' ';
            else res[r++] = '-';
        }
    } res[r] = '\0'; return res;
}

// ============================================================================
// FONTE v7 — SALTO QUÂNTICO: teclado completo (95 símbolos ASCII printáveis,
// ordem de código). Toda tecla tem valor próprio:
//   espaço=1 … '!'=2 … '0'=17 … '9'=26 … 'A'=34 … 'Z'=59 … 'a'=66 … 'z'=91 … '~'=95
// Duas linhas de matemática — à prova de re-interpretação da IA de plantão.
// ============================================================================
int fonte95_para_num(char c) {
    return (c >= 32 && c <= 126) ? (int)c - 31 : -1;
}

char fonte95_num_para_char(int num) {
    long pos = (num - 1) % MODULO; while (pos < 0) pos += MODULO;
    return (char)(32 + pos);
}

// Normalização v7: translitera acentos LATIN-1 MANTENDO A CAIXA
// (á->a, Á->A, ç->c, Ç->C, Ý->Y, ß->s); ASCII printável passa igual;
// tab/quebra/controles viram espaço; nada vira '-': '-' é letra de verdade.
char* normalizar_fonte95(const char* texto) {
    long len = strlen(texto); char* res = malloc(len * 2 + 1); long r = 0;
    for (long i = 0; i < len; i++) {
        unsigned char c = (unsigned char)texto[i];
        if (c == 0xC3 && i + 1 < len) {
            // Latin-1 Supplement (UTF-8 C3 xx): maiúsculas em xx<0xA0, minúsculas em xx>=0xA0
            i++; unsigned char x = (unsigned char)texto[i];
            int minusculo = (x >= 0xA0);
            unsigned char b = x & 0x1F;
            char base = 0;
            if (b <= 0x05) base = 'A';            // Á Â Ã Ä Å à â ã ä å
            else if (b == 0x07) base = 'C';       // Ç ç
            else if (b >= 0x08 && b <= 0x0B) base = 'E'; // É Ê Ë é ê ë
            else if (b >= 0x0C && b <= 0x0F) base = 'I'; // Í Î Ï í î ï
            else if (b == 0x11) base = 'N';       // Ñ ñ
            else if (b >= 0x12 && b <= 0x16) base = 'O'; // Ó Ô Õ Ö ó ô õ ö
            else if (b >= 0x19 && b <= 0x1C) base = 'U'; // Ú Û Ü ú û ü
            else if (b == 0x1D) base = 'Y';       // Ý ý
            else if (b == 0x1F) base = 's';       // ß (ess-zett)
            // Æ/æ, Ð/ð, ×, ÷, Ø/ø, Þ/þ (b = 0x06,0x10,0x17,0x18,0x1E): sem
            // caixa-português — caem como espaço (fora da fonte).
            if (base) {
                char letra = base;
                if (minusculo && base != 's') letra = (char)(base - 'A' + 'a');
                else if (base == 's') letra = 's';
                res[r++] = letra;
            } else res[r++] = ' ';
        } else if (c >= 0x80) {
            // Multibyte que não é Latin-1 (emoji, CJK, símbolos C2, Latin
            // Extended…): consome a sequência inteira e vira UM espaço —
            // nada vira '-': o desconhecido não é mais um símbolo de confusão.
            i++;
            while (i < len && ((unsigned char)texto[i] & 0xC0) == 0x80) i++;
            i--; res[r++] = ' ';
        } else if (c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f') {
            res[r++] = ' ';
        } else if (c >= 32 && c <= 126) {
            res[r++] = (char)c; // minúsculas, dígitos, pontuação: preservados como são
        } else {
            res[r++] = ' '; // demais controles (0x00..0x1F, 0x7F)
        }
    } res[r] = '\0'; return res;
}

// ============================================================================
// MALHA UTF-8 (v7.1) — a memória byte a byte da decifra.
// A fonte continua 95 (cópia honesta da tela), mas nenhum byte se perde:
// tudo que está FORA da fonte (acento multibyte, quebra de linha, tab,
// controle > 0x7E) viaja como TRIO de glifos da própria fonte — '=' + dois
// hexadecimais MAIÚSCULOS (quoted-printable, RFC 2045):
//   'é' (=C3=A9 em bytes) -> "=C3=A9"   '\n' -> "=0A"   '=' -> "=3D"
// A decodificação é CANÔNICA: decodifica, re-encoda e só aceita a leitura
// escaped se re-encodar reproduzir a corda original byte a byte. Textos
// escritos à mão que por acaso contenham "=XX" voltam LITERAIS — sem
// ambiguidade, sem perda. (Limitação honesta herdada do v7.0.0: um texto
// antigo contendo LITERALMENTE "=3D" é lido como '='. Corrige recifrando.)
// ============================================================================
static int malha_hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

char* malha_utf8_encode(const char* brutos) {
    long L = strlen(brutos);
    char* r = malloc((size_t)L * 3 + 4); // pior caso: todo byte vira 3 glifos
    if (!r) return NULL;
    static const char HEX[] = "0123456789ABCDEF";
    long o = 0;
    for (long i = 0; i < L; i++) {
        unsigned char b = (unsigned char)brutos[i];
        if (b == '=') { r[o++] = '='; r[o++] = '3'; r[o++] = 'D'; }
        else if (b >= 0x20 && b <= 0x7E) r[o++] = (char)b; // dentro da fonte: passa
        else { r[o++] = '='; r[o++] = HEX[b >> 4]; r[o++] = HEX[b & 0x0F]; }
    }
    r[o] = '\0'; return r;
}

char* malha_utf8_decode(const char* s) {
    long L = strlen(s);
    char* dec = malloc((size_t)L + 2);
    if (!dec) return NULL;
    long o = 0;
    for (long i = 0; i < L; ) {
        if (s[i] == '=' && i + 2 < L) {
            int h1 = malha_hexval(s[i + 1]), h2 = malha_hexval(s[i + 2]);
            if (h1 >= 0 && h2 >= 0) { // escape legítimo: 3 glifos -> UM byte
                dec[o++] = (char)(h1 * 16 + h2); i += 3; continue;
            }
        }
        dec[o++] = s[i]; i++; // '=' solitário ou sem par hex: literal
    }
    dec[o] = '\0';
    // CANÔNICO: decodifica -> re-encoda -> compara. Só aceita a leitura
    // escaped quando o ciclo fecha; senão devolve a corda como veio.
    char* re = malha_utf8_encode(dec);
    if (re && strcmp(re, s) != 0) {
        // roundtrip não fecha: texto cru (v7.0.0 anterior, ou '=XX' à mão)
        free(re); free(dec);
        char* literal = malloc((size_t)L + 1);
        if (literal) { memcpy(literal, s, (size_t)L + 1); }
        return literal;
    }
    free(re);
    return dec; // byte a byte — o acento e a quebra de linha voltam de pé
}

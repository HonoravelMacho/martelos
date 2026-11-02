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

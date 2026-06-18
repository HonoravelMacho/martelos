#include "martelos.h"
const char ALFABETO[MODULO] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' ','\n','.',',',';','?','!','-'};
int caractere_para_num(char c) {
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 1);
    switch(c) { case ' ': return 27; case '\n': return 28; case '.': return 29; case ',': return 30; case ';': return 31; case '?': return 32; case '!': return 33; case '-': return 34; default: return -1; }
}
char num_para_caractere(int num) {
    int pos = (num - 1) % MODULO; while (pos < 0) pos += MODULO; return ALFABETO[pos % MODULO];
}
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
            if (caractere_para_num(up) != -1) res[r++] = up;
            else if (isspace(c)) res[r++] = ' ';
            else res[r++] = '-';
        }
    } res[r] = '\0'; return res;
}

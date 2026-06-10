#include "martelos.h"

const char ALFABETO[MODULO] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    ' ', '\n', '.', ',', ';', '?', '!', '-'
};

int caractere_para_num(char c) {
    if (c >= 'A' && c <= 'Z') return (c - 'A' + 1);
    switch(c) {
        case ' ': return 27; case '\n': return 28; case '.': return 29;
        case ',': return 30; case ';': return 31; case '?': return 32;
        case '!': return 33; case '-': return 34;
        default: return -1;
    }
}

char num_para_caractere(int num) {
    int posicao = (num - 1) % MODULO;
    if (posicao < 0) posicao += MODULO;
    return ALFABETO[posicao];
}

char* remover_acentos_e_upper(const char* texto) {
    long len = strlen(texto);
    // Alocamos o dobro por segurança (caracteres multibyte)
    char* resultado = malloc((len * 2 + 1) * sizeof(char));
    long r_idx = 0;

    for (long i = 0; i < len; i++) {
        unsigned char c = (unsigned char)texto[i];

        // Tratamento de acentuação UTF-8 (Caracteres que começam com 0xC3)
        if (c == 0xC3 && i + 1 < len) {
            i++;
            unsigned char c2 = (unsigned char)texto[i];
            switch(c2) {
                // Letra A
                case 0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85:
                case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5:
                    resultado[r_idx++] = 'A'; break;
                // Letra E
                case 0x88: case 0x89: case 0x8A: case 0x8B:
                case 0xA8: case 0xA9: case 0xAA: case 0xAB:
                    resultado[r_idx++] = 'E'; break;
                // Letra I
                case 0x8C: case 0x8D: case 0x8E: case 0x8F:
                case 0xAC: case 0xAD: case 0xAE: case 0xAF:
                    resultado[r_idx++] = 'I'; break;
                // Letra O
                case 0x92: case 0x93: case 0x94: case 0x95: case 0x96:
                case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6:
                    resultado[r_idx++] = 'O'; break;
                // Letra U
                case 0x99: case 0x9A: case 0x9B: case 0x9C:
                case 0xB9: case 0xBA: case 0xBB: case 0xBC:
                    resultado[r_idx++] = 'U'; break;
                // Letra C (Cedilha)
                case 0x87: case 0xA7:
                    resultado[r_idx++] = 'C'; break;
                // Letra N (Til - espanhol)
                case 0x91: case 0xB1:
                    resultado[r_idx++] = 'N'; break;
                default: 
                    // Se for um UTF-8 desconhecido, ignoramos o byte extra
                    break;
            }
        } 
        // Tratamento de hífens especiais (dash)
        else if (c == 0xE2 && i + 2 < len) {
             if ((unsigned char)texto[i+1] == 0x80 && ((unsigned char)texto[i+2] == 0x93 || (unsigned char)texto[i+2] == 0x94)) {
                 resultado[r_idx++] = '-';
                 i += 2;
             }
        }
        else {
            // Caractere ASCII comum
            char up = toupper(c);
            // Só adiciona se fizer parte do nosso alfabeto ou for um símbolo aceito
            if (caractere_para_num(up) != -1) {
                resultado[r_idx++] = up;
            } else if (c == '(' || c == ')' || c == ':' || c == '\"' || c == '\'') {
                // Opcional: converter pontuação extra em hífen ou espaço para não quebrar a cifra
                resultado[r_idx++] = '-';
            }
        }
    }
    resultado[r_idx] = '\0';
    return resultado;
}

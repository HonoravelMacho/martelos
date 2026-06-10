#include "martelos.h"

void aplicar_quiasmo(char** elementos, int total) {
    if (total <= 2) return;
    char** temp = malloc(total * sizeof(char*));
    int idx = 0;
    for (int i = 0; i < total; i += 2) temp[idx++] = elementos[i];
    int inicio_par = (total % 2 == 0) ? total - 1 : total - 2;
    for (int i = inicio_par; i >= 0; i -= 2) temp[idx++] = elementos[i];
    for (int i = 0; i < total; i++) elementos[i] = temp[i];
    free(temp);
}

long calcular_casa_quiasmo(const char* frase, int* out_n) {
    char* limpa = remover_acentos_e_upper(frase);
    int cap = 64, n = 0;
    char** palavras = malloc(cap * sizeof(char*));
    char* token = strtok(limpa, " \t\r\n");
    while (token != NULL) {
        if (n >= cap) { cap *= 2; palavras = realloc(palavras, cap * sizeof(char*)); }
        palavras[n++] = strdup(token);
        token = strtok(NULL, " \t\r\n");
    }
    *out_n = n;
    if (n == 0) { free(palavras); free(limpa); return 1; }
    aplicar_quiasmo(palavras, n);
    long soma = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; palavras[i][j] != '\0'; j++) {
            int v = caractere_para_num(palavras[i][j]);
            if (v != -1) soma += v;
        }
        free(palavras[i]);
    }
    free(palavras); free(limpa);
    return (soma <= 0) ? 1 : soma;
}

char* cifrar_texto(const char* texto_puro, long salto, int tipo_const, const char* formula) {
    char* texto = remover_acentos_e_upper(texto_puro);
    long len = strlen(texto);
    long prec = salto + (len * 40) + 1000;
    char* esteira = obter_esteira_numerica(tipo_const, formula, prec);
    char* res = malloc(len + 1);
    long ptr = salto;
    for (long i = 0; i < len; i++) {
        int v = caractere_para_num(texto[i]);
        if (v != -1) {
            char n_str[3] = { esteira[ptr], esteira[ptr+1], '\0' };
            int shift = atoi(n_str);
            res[i] = num_para_caractere(v + shift);
            ptr += v;
        } else res[i] = texto[i];
    }
    res[len] = '\0';
    free(texto);
    return res;
}

char* decifrar_texto(const char* texto_cripto, long salto, int tipo_const, const char* formula) {
    long len = strlen(texto_cripto);
    long prec = salto + (len * 40) + 1000;
    char* esteira = obter_esteira_numerica(tipo_const, formula, prec);
    char* res = malloc(len + 1);
    long ptr = salto;
    for (long i = 0; i < len; i++) {
        int v = caractere_para_num(texto_cripto[i]);
        if (v != -1) {
            char n_str[3] = { esteira[ptr], esteira[ptr+1], '\0' };
            int shift = atoi(n_str);
            int v_orig = (v - shift);
            while (v_orig <= 0) v_orig += MODULO;
            res[i] = num_para_caractere(v_orig);
            ptr += v_orig;
        } else res[i] = texto_cripto[i];
    }
    res[len] = '\0';
    return res;
}

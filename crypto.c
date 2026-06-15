#include "martelos.h"

// --- Engrenagens de Quiasmo (Base do Motor) ---

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

// --- Motores N-Layers (Modo Texto) ---

char* cifrar_texto_n_camadas(const char* texto_puro, long* casas, int n_camadas, int tipo_const, const char* formula) {
    char* resultado = remover_acentos_e_upper(texto_puro);
    
    for (int c = 0; c < n_camadas; c++) {
        long len = strlen(resultado);
        // A Frase 1 (casas[0]) é sempre a âncora inicial
        char* esteira = obter_esteira_numerica(tipo_const, formula, casas[0] + (len * 50) + 1000);
        char* novo = malloc(len + 1);
        long ptr = casas[0]; 

        for (long i = 0; i < len; i++) {
            int v = caractere_para_num(resultado[i]);
            if (v != -1) {
                // Soma o deslocamento da camada atual (casas[c]) ao ponteiro
                long p_local = ptr + casas[c];
                int shift = (esteira[p_local % 1000000] - '0') * 10 + (esteira[(p_local+1) % 1000000] - '0');
                novo[i] = num_para_caractere(v + shift);
                ptr += v;
            } else novo[i] = resultado[i];
        }
        novo[len] = '\0';
        free(resultado);
        resultado = novo;
    }
    return resultado;
}

char* decifrar_texto_n_camadas(const char* texto_cripto, long* casas, int n_camadas, int tipo_const, const char* formula) {
    char* resultado = strdup(texto_cripto);
    
    for (int c = n_camadas - 1; c >= 0; c--) {
        long len = strlen(resultado);
        char* esteira = obter_esteira_numerica(tipo_const, formula, casas[0] + (len * 50) + 1000);
        char* novo = malloc(len + 1);
        long ptr = casas[0];

        for (long i = 0; i < len; i++) {
            int v = caractere_para_num(resultado[i]);
            if (v != -1) {
                long p_local = ptr + casas[c];
                int shift = (esteira[p_local % 1000000] - '0') * 10 + (esteira[(p_local+1) % 1000000] - '0');
                int v_orig = v - shift;
                while (v_orig <= 0) v_orig += MODULO;
                novo[i] = num_para_caractere(v_orig);
                ptr += v_orig;
            } else novo[i] = resultado[i];
        }
        novo[len] = '\0';
        free(resultado);
        resultado = novo;
    }
    return resultado;
}

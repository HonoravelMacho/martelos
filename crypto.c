#include "martelos.h"
void aplicar_quiasmo(char** el, int tot) {
    if (tot <= 2) return; char** tmp = malloc(tot * sizeof(char*)); int idx = 0;
    for (int i = 0; i < tot; i += 2) tmp[idx++] = el[i];
    for (int i = (tot % 2 == 0 ? tot - 1 : tot - 2); i >= 0; i -= 2) tmp[idx++] = el[i];
    for (int i = 0; i < tot; i++) el[i] = tmp[i]; free(tmp);
}
long calcular_casa_quiasmo(const char* fr, int* out_n) {
    char* lim = remover_acentos_e_upper(fr); int cap = 64, n = 0; char** pals = malloc(cap * sizeof(char*));
    char* tok = strtok(lim, " \t\r\n");
    while (tok) { if (n >= cap) { cap *= 2; pals = realloc(pals, cap * sizeof(char*)); } pals[n++] = strdup(tok); tok = strtok(NULL, " \t\r\n"); }
    *out_n = n; if (n == 0) { free(pals); free(lim); return 1; } aplicar_quiasmo(pals, n);
    long soma = 0; for (int i = 0; i < n; i++) { for (int j = 0; pals[i][j]; j++) { int v = caractere_para_num(pals[i][j]); if (v != -1) soma += v; } free(pals[i]); }
    free(pals); free(lim); return (soma <= 0) ? 1 : soma;
}
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f) {
    char* res = remover_acentos_e_upper(t);
    for (int c = 0; c < n; c++) {
        long len = strlen(res); char* est = obter_esteira_numerica(tipo, f, cs[0] + (len * 50) + 1000);
        long tam_e = strlen(est); char* nv = malloc(len + 1); long ptr = cs[0];
        for (long i = 0; i < len; i++) {
            int v = caractere_para_num(res[i]);
            if (v != -1) {
                int sh = (est[(ptr + cs[c]) % tam_e] - '0') * 10 + (est[(ptr + cs[c] + 1) % tam_e] - '0');
                nv[i] = num_para_caractere(v + sh); ptr += v;
            } else nv[i] = res[i];
        } nv[len] = '\0'; free(res); res = nv;
    } return res;
}
char* decifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f) {
    char* res = strdup(t);
    for (int c = n - 1; c >= 0; c--) {
        long len = strlen(res); char* est = obter_esteira_numerica(tipo, f, cs[0] + (len * 50) + 1000);
        long tam_e = strlen(est); char* nv = malloc(len + 1); long ptr = cs[0];
        for (long i = 0; i < len; i++) {
            int v = caractere_para_num(res[i]);
            if (v != -1) {
                int sh = (est[(ptr + cs[c]) % tam_e] - '0') * 10 + (est[(ptr + cs[c] + 1) % tam_e] - '0');
                int vo = v - sh; while (vo <= 0) vo += MODULO;
                nv[i] = num_para_caractere(vo); ptr += vo;
            } else nv[i] = res[i];
        } nv[len] = '\0'; free(res); res = nv;
    } return res;
}

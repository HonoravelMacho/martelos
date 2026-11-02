#include "martelos.h"

// ============================================================================
// LEGADO v6 — comportamento congelado (ouros do teste.c). Os únicos toques
// foram os nomes dos helpers do alfabeto (*_legado). NÃO ALTERAR A MATEMÁTICA.
// ============================================================================

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
    long soma = 0; for (int i = 0; i < n; i++) { for (int j = 0; pals[i][j]; j++) { int v = caractere_para_num_legado(pals[i][j]); if (v != -1) soma += v; } free(pals[i]); }
    free(pals); free(lim); return (soma <= 0) ? 1 : soma;
}
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f) {
    char* res = remover_acentos_e_upper(t);
    for (int c = 0; c < n; c++) {
        long len = strlen(res); char* est = obter_esteira_numerica(tipo, f, cs[0] + (len * 50) + 1000);
        long tam_e = strlen(est); char* nv = malloc(len + 1); long ptr = cs[0];
        for (long i = 0; i < len; i++) {
            int v = caractere_para_num_legado(res[i]);
            if (v != -1) {
                int sh = (est[(ptr + cs[c]) % tam_e] - '0') * 10 + (est[(ptr + cs[c] + 1) % tam_e] - '0');
                nv[i] = num_para_caractere_legado(v + sh); ptr += v;
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
            int v = caractere_para_num_legado(res[i]);
            if (v != -1) {
                int sh = (est[(ptr + cs[c]) % tam_e] - '0') * 10 + (est[(ptr + cs[c] + 1) % tam_e] - '0');
                int vo = v - sh; while (vo <= 0) vo += MODULO_LEGADO;
                nv[i] = num_para_caractere_legado(vo); ptr += vo;
            } else nv[i] = res[i];
        } nv[len] = '\0'; free(res); res = nv;
    } return res;
}

// ============================================================================
// SALTO QUÂNTICO v7 — A casa da frase é um NÚMERO LITERAL (posicional em
// base 95). A ordem é sagrada: AB ≠ BA. O QUIASMO deixa de ser enfeite:
// a leitura da camada c aplica o espelho c vezes antes de virar número.
// A esteira é dimensionada pela MENSAGEM; o ponteiro nasce em
// casa0 % tamanho_da_esteira. Um número de soneto nunca mais infla o pi.
// ============================================================================

// Espelho da frase, uma aplicação do quiasmo:
//  • 3+ palavras: o quiasmo de palavras da casa (pares à frente, ímpares
//    espelhados atrás) — o mesmo aplicar_quiasmo da v6.
//  • até 2 palavras: espelho das letras — o salto literal AB↔BA.
// A aplicação cíclica (profundidade 2, 3...) cobre "espelho do espelho".
static char* frase_espelho(const char* fr, int profundidade) {
    char* lim = normalizar_fonte95(fr);
    long tl = strlen(lim); char* buf = malloc(tl + 1); strcpy(buf, lim); free(lim);
    for (int d = 0; d < profundidade; d++) {
        int tot = 0, inword = 0;
        for (const char* q = buf; *q; q++) { if (*q != ' ') { if (!inword) { tot++; inword = 1; } } else inword = 0; }
        if (tot >= 3) {
            int cap = tot, nw = 0; char** pals = malloc((size_t)cap * sizeof(char*));
            char* tok = strtok(buf, " ");
            while (tok) { pals[nw++] = tok; tok = strtok(NULL, " "); }
            aplicar_quiasmo(pals, tot);
            char* rbuf = malloc(tl + 1); long rr = 0;
            for (int i = 0; i < tot; i++) { if (i) rbuf[rr++] = ' '; long w = strlen(pals[i]); memcpy(rbuf + rr, pals[i], (size_t)w); rr += w; }
            rbuf[rr] = '\0'; free(pals); free(buf); buf = rbuf;
        } else {
            long z = strlen(buf) - 1;
            for (long a = 0; a < z; a++, z--) { char t = buf[a]; buf[a] = buf[z]; buf[z] = t; }
        }
    }
    return buf;
}

// casa = Σ v_i · 95^(len-1-i) — o número literal que a frase ESCREVE.
void calcular_casa_salto(const char* fr, int profundidade, mpz_t out) {
    char* buf = frase_espelho(fr, profundidade);
    long L = strlen(buf);
    if (L == 0) { mpz_set_ui(out, 1); free(buf); return; } // frase vazia: casa 1
    mpz_set_ui(out, 0);
    mpz_t pot; mpz_init(pot);
    mpz_ui_pow_ui(pot, (unsigned long)MODULO, (unsigned long)(L - 1));
    for (long i = 0; i < L; i++) {
        int v = fonte95_para_num(buf[i]);
        if (v > 0) mpz_addmul_ui(out, pot, (unsigned long)v);
        mpz_tdiv_q_ui(pot, pot, MODULO);
    }
    mpz_clear(pot);
    free(buf);
}

// Selo-guarita: dois símbolos derivados do TEXTO CLARO normalizado
// (módulo dos primos 83 e 89, "primos do meio" — não entregam chave alguma).
// Detecta senha errada e cópia mutilada. Checksum leve e público: cifras
// experimentais expostas em plena luz — o selo é a bagagem, não a fechadura.
static void selo_de(const char* plano, char* s1, char* s2) {
    long S1 = 0, S2 = 0;
    for (long i = 0; plano[i]; i++) {
        int v = fonte95_para_num(plano[i]);
        if (v > 0) {
            S1 += v;
            S2 += (long)((i % 89) + 1) * (long)v;
        }
    }
    *s1 = fonte95_num_para_char((int)(S1 % 83) + 1);
    *s2 = fonte95_num_para_char((int)(S2 % 89) + 1);
}

// Recolocação do selo (a regra da memória do capitão):
//  comprimento ÍMPAR: no meio exato; PAR: à esquerda do meio.
static char* inserir_selo(const char* cifrado, const char* plano) {
    long L = strlen(cifrado);
    if (L == 0) return strdup(cifrado); // mensagem vazia não ganha selo
    char s1, s2; selo_de(plano, &s1, &s2);
    long p = (L % 2 == 1) ? L / 2 : L / 2 - 1;
    char* out = malloc((size_t)L + 3);
    memcpy(out, cifrado, (size_t)p);
    out[p] = s1; out[p + 1] = s2;
    memcpy(out + p + 2, cifrado + p, (size_t)(L - p));
    out[L + 2] = '\0';
    return out;
}

char* cifrar_salto(const char* t, mpz_t* casas, int n, int tipo, const char* f, int* selo_estado) {
    if (selo_estado) *selo_estado = 0;
    // Oceano fresco: o cache da esteira não pode cruzar tamanhos de texto
    // distintos no mesmo processo (a esteira maior em cache mudaria tam_e).
    limpar_cache_esteira();
    // MALHA UTF-8: a entrada crua entra de pé — acentos, quebras de linha e
    // qq byte fora da fonte 95 viajam como trios de glifos ("=C3=A9", "=0A").
    // O texto "plano" visto pela aritmética é 100% fonte 95, por construção.
    char* res = malha_utf8_encode(t);
    long len = strlen(res);
    if (len == 0) return res; // texto vazio: nada a selar
    char* plano = strdup(res); // o selo lê o TEXTO CLARO (já malhado) antes das camadas
    // Esteira dimensionada pela MENSAGEM — a casa literal gigante nunca toca
    // a precisão do pi (a lição da "SOMA MODULAR ESTÁVEL" do v2.7).
    char* est = obter_esteira_numerica(tipo, f, len * 120 + 100);
    long tam_e = strlen(est);
    mpz_t rem; mpz_init(rem);
    long* offs = malloc((size_t)n * sizeof(long));
    for (int c = 0; c < n; c++) offs[c] = (long)mpz_fdiv_r_ui(rem, casas[c], (unsigned long)tam_e);
    mpz_clear(rem);
    for (int c = 0; c < n; c++) {
        long ptr = offs[0]; // o ponteiro nasce na casa da PRIMEIRA frase
        for (long i = 0; i < len; i++) {
            int v = fonte95_para_num(res[i]);
            if (v > 0) {
                long p = (ptr + offs[c]) % tam_e;
                int sh = (est[p] - '0') * 10 + (est[(p + 1) % tam_e] - '0');
                res[i] = fonte95_num_para_char(v + sh);
                ptr += v;
            }
            // texto fora da fonte: impossível pós-malha (guarda simbólica)
        }
    }
    char* saida = inserir_selo(res, plano);
    free(plano);
    if (selo_estado) *selo_estado = 1;
    free(res);
    return saida;
}

// decifra um corpo SEM o selo (a própria matemática das camadas em reverso)
static char* decifrar_corpo(const char* corpo, mpz_t* casas, int n, int tipo, const char* f) {
    long len = strlen(corpo);
    if (len == 0) return strdup(corpo);
    // Oceano fresco no tamanho exato deste corpo (simetria com a cifra)
    limpar_cache_esteira();
    char* est = obter_esteira_numerica(tipo, f, len * 120 + 100);
    long tam_e = strlen(est);
    mpz_t rem; mpz_init(rem);
    long* offs = malloc((size_t)n * sizeof(long));
    for (int c = 0; c < n; c++) offs[c] = (long)mpz_fdiv_r_ui(rem, casas[c], (unsigned long)tam_e);
    mpz_clear(rem);
    char* res = strdup(corpo);
    for (int c = n - 1; c >= 0; c--) {
        long ptr = offs[0];
        for (long i = 0; i < len; i++) {
            int v = fonte95_para_num(res[i]);
            if (v > 0) {
                long p = (ptr + offs[c]) % tam_e;
                int sh = (est[p] - '0') * 10 + (est[(p + 1) % tam_e] - '0');
                int vo = v - sh; while (vo <= 0) vo += MODULO;
                res[i] = fonte95_num_para_char(vo);
                ptr += vo;
            }
        }
    }
    free(offs);
    return res;
}

char* decifrar_salto(const char* t, mpz_t* casas, int n, int tipo, const char* f, int* selo_estado) {
    if (selo_estado) *selo_estado = 0;
    // Cauda de cópia de terminal (\n/\r) fora: a cifra v7 nunca termina em quebra
    char* limpa = strdup(t);
    long tl = strlen(limpa);
    while (tl > 0 && (limpa[tl - 1] == '\n' || limpa[tl - 1] == '\r')) limpa[--tl] = '\0';
    if (tl >= 3) {
        // candidata v7: extrai as 2 figurinhas do selo pela regra do meio e valida
        long L = tl - 2;
        long p = (L % 2 == 1) ? L / 2 : L / 2 - 1;
        char c1 = limpa[p], c2 = limpa[p + 1];
        char* corpo = malloc((size_t)tl - 1);
        memcpy(corpo, limpa, (size_t)p);
        memcpy(corpo + p, limpa + p + 2, (size_t)(tl - p - 2));
        corpo[tl - 2] = '\0';
        char* plano = decifrar_corpo(corpo, casas, n, tipo, f);
        char e1, e2; selo_de(plano, &e1, &e2);
        if (e1 == c1 && e2 == c2) {
            if (selo_estado) *selo_estado = 1;
            // MALHA UTF-8: o corpo voltou na forma malhada — devolver a entrada
            // crua byte a byte (acento, quebra de linha e tudo mais de pé).
            char* cru = malha_utf8_decode(plano);
            free(limpa); free(corpo); free(plano);
            return cru;
        }
        free(plano); free(corpo);
    }
    // sem selo válido: decifra cru (mensagem curta, sem selo ou cópia mutilada)
    char* plano2 = decifrar_corpo(limpa, casas, n, tipo, f);
    char* cru2 = malha_utf8_decode(plano2); // canônica: cru volta como veio
    free(plano2); free(limpa);
    return cru2;
}

void liberar_casas(mpz_t* casas, int n) {
    if (!casas) return;
    for (int i = 0; i < n; i++) mpz_clear(casas[i]);
    free(casas);
}

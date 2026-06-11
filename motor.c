#include "martelos.h"

static char* CACHE_ESTEIRA = NULL;
static long TAMANHO_CACHE = 0;
static int TIPO_CACHE = -1; // 1: Pi, 2: Custom, 3: Catalan, 4: Apery, 5: Clausen, 6: Arctan, 7: Log3
static char FORMULA_ATUAL[256] = "";

void limpar_cache_esteira() {
    if (CACHE_ESTEIRA) { free(CACHE_ESTEIRA); CACHE_ESTEIRA = NULL; }
    TAMANHO_CACHE = 0; TIPO_CACHE = -1;
}

// --- Motores de Cálculo de Alta Precisão ---

char* calcular_pi_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 100) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t a, b, t, p, an, bn, temp, pi;
    mpf_inits(a, b, t, p, an, bn, temp, pi, NULL);
    mpf_set_ui(a, 1);
    mpf_set_ui(temp, 2); mpf_sqrt(temp, temp); mpf_ui_div(b, 1, temp);
    mpf_set_d(t, 0.25); mpf_set_ui(p, 1);
    for (int i = 0; i < 25; i++) {
        mpf_add(an, a, b); mpf_div_ui(an, an, 2);
        mpf_mul(bn, a, b); mpf_sqrt(bn, bn);
        mpf_sub(temp, a, an); mpf_mul(temp, temp, temp); mpf_mul(temp, temp, p);
        mpf_sub(t, t, temp);
        mpf_set(a, an); mpf_set(b, bn); mpf_mul_ui(p, p, 2);
    }
    mpf_add(pi, a, b); mpf_mul(pi, pi, pi);
    mpf_mul_ui(temp, t, 4); mpf_div(pi, pi, temp);
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, pi);
    mpf_clears(a, b, t, p, an, bn, temp, pi, NULL);
    return str;
}

char* calcular_catalan_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 500) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t G, termo, num, den;
    mpf_inits(G, termo, num, den, NULL);
    mpf_set_ui(G, 0);
    for (long k = 0; k < 50000; k++) {
        mpf_set_si(num, (k % 2 == 0) ? 1 : -1);
        unsigned long d = 2 * k + 1;
        mpf_set_ui(den, d * d);
        mpf_div(termo, num, den);
        mpf_add(G, G, termo);
    }
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, G);
    mpf_clears(G, termo, num, den, NULL);
    return str;
}

char* calcular_apery_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 200) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t zeta3, termo, n, d, binom;
    mpf_inits(zeta3, termo, n, d, binom, NULL);
    mpf_set_ui(zeta3, 0);
    mpz_t z_binom; mpz_init(z_binom);
    for (unsigned long k = 1; k < 10000; k++) { 
        mpz_bin_uiui(z_binom, 2 * k, k);
        mpf_set_z(binom, z_binom);
        mpf_set_ui(n, 1); if (k % 2 == 0) mpf_neg(n, n); 
        mpf_set_ui(d, k); mpf_pow_ui(d, d, 3); mpf_mul(d, d, binom);
        mpf_div(termo, n, d); mpf_add(zeta3, zeta3, termo);
        if (k > 100 && mpf_cmp_d(termo, 1e-20) == 0) break;
    }
    mpf_mul_ui(zeta3, zeta3, 5); mpf_div_ui(zeta3, zeta3, 2);
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, zeta3);
    mpz_clear(z_binom); mpf_clears(zeta3, termo, n, d, binom, NULL);
    return str;
}

char* calcular_clausen_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 200) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t res, termo, k_sq, sqrt3;
    mpf_inits(res, termo, k_sq, sqrt3, NULL);
    mpf_set_ui(res, 0);
    mpf_set_ui(sqrt3, 3); mpf_sqrt(sqrt3, sqrt3);
    for (unsigned long k = 1; k < 15000; k++) {
        int idx = k % 6; double s = 0;
        if (idx == 1 || idx == 2) s = 1.0; else if (idx == 4 || idx == 5) s = -1.0;
        if (s != 0) {
            mpf_set_ui(k_sq, k * k); mpf_set_d(termo, s);
            mpf_div(termo, termo, k_sq); mpf_add(res, res, termo);
        }
    }
    mpf_mul(res, res, sqrt3); mpf_div_ui(res, res, 2);
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, res);
    mpf_clears(res, termo, k_sq, sqrt3, NULL);
    return str;
}

char* calcular_arctan_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 100) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t pi_val; mpf_init(pi_val);
    char* pi_str = calcular_pi_gmp(precisao_digitos);
    mpf_set_str(pi_val, pi_str, 10);
    mpf_div_ui(pi_val, pi_val, 4); 
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, pi_val);
    free(pi_str); mpf_clear(pi_val);
    return str;
}

char* calcular_log3_gmp(long precisao_digitos) {
    unsigned long bits = (unsigned long)((precisao_digitos + 100) * 3.321928);
    mpf_set_default_prec(bits);
    mpf_t res, termo, den, tres_k;
    mpf_inits(res, termo, den, tres_k, NULL);
    mpf_set_ui(res, 0);
    for (unsigned long k = 1; k < 10000; k++) {
        mpf_set_ui(tres_k, 3); mpf_pow_ui(tres_k, tres_k, k);
        mpf_mul_ui(den, tres_k, k);
        mpf_ui_div(termo, 1, den);
        mpf_add(res, res, termo);
        if (k > 100 && mpf_cmp_d(termo, 1e-25) == 0) break;
    }
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao_digitos + 5, res);
    mpf_clears(res, termo, den, tres_k, NULL);
    return str;
}

char* obter_esteira_numerica(int tipo_constante, const char* formula, long precisao_necessaria) {
    long precisao_real = precisao_necessaria + 100;
    if (TIPO_CACHE == tipo_constante && TAMANHO_CACHE >= precisao_real) {
        if (tipo_constante == 1 || (tipo_constante >= 3 && tipo_constante <= 7)) return CACHE_ESTEIRA;
        if (tipo_constante == 2 && strcmp(FORMULA_ATUAL, formula) == 0) return CACHE_ESTEIRA;
    }
    limpar_cache_esteira();
    printf("\n[⚙️ Motor] Gerando esteira ");
    if (tipo_constante == 1) { printf("(Pi)... "); CACHE_ESTEIRA = calcular_pi_gmp(precisao_real); }
    else if (tipo_constante == 3) { printf("(Catalan)... "); CACHE_ESTEIRA = calcular_catalan_gmp(precisao_real); }
    else if (tipo_constante == 4) { printf("(Apery)... "); CACHE_ESTEIRA = calcular_apery_gmp(precisao_real); }
    else if (tipo_constante == 5) { printf("(Clausen)... "); CACHE_ESTEIRA = calcular_clausen_gmp(precisao_real); }
    else if (tipo_constante == 6) { printf("(Arctan)... "); CACHE_ESTEIRA = calcular_arctan_gmp(precisao_real); }
    else if (tipo_constante == 7) { printf("(Log3)... "); CACHE_ESTEIRA = calcular_log3_gmp(precisao_real); }
    else {
        printf("(%s)... ", formula);
        extern char* calcular_formula_custom(const char* f, long p);
        CACHE_ESTEIRA = calcular_formula_custom(formula, precisao_real);
        strncpy(FORMULA_ATUAL, formula, 255);
    }
    fflush(stdout); TIPO_CACHE = tipo_constante; TAMANHO_CACHE = precisao_real; printf("OK!\n");
    return CACHE_ESTEIRA;
}

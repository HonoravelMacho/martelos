#include "martelos.h"
static char* CACHE_ESTEIRA = NULL;
static long TAMANHO_CACHE = 0;
static int TIPO_CACHE = -1;
static char FORMULA_ATUAL[256] = "";
void limpar_cache_esteira() { if (CACHE_ESTEIRA) free(CACHE_ESTEIRA); CACHE_ESTEIRA = NULL; TAMANHO_CACHE = 0; TIPO_CACHE = -1; }
int converter_digito_base(char c) { if (isdigit(c)) return c - '0'; if (islower(c)) return c - 'a' + 10; if (isupper(c)) return c - 'A' + 36; return 0; }
char* calcular_pi_gmp(long pr) {
    mpf_set_default_prec((pr + 100) * 4);
    mpf_t a, b, t, p, an, bn, tmp, pi; mpf_inits(a,b,t,p,an,bn,tmp,pi,NULL);
    mpf_set_ui(a, 1); mpf_set_ui(tmp, 2); mpf_sqrt(tmp, tmp); mpf_ui_div(b, 1, tmp);
    mpf_set_d(t, 0.25); mpf_set_ui(p, 1);
    for (int i=0; i<25; i++) {
        mpf_add(an, a, b); mpf_div_ui(an, an, 2); mpf_mul(bn, a, b); mpf_sqrt(bn, bn);
        mpf_sub(tmp, a, an); mpf_mul(tmp, tmp, tmp); mpf_mul(tmp, tmp, p);
        mpf_sub(t, t, tmp); mpf_set(a, an); mpf_set(b, bn); mpf_mul_ui(p, p, 2);
    }
    mpf_add(pi, a, b); mpf_mul(pi, pi, pi); mpf_mul_ui(tmp, t, 4); mpf_div(pi, pi, tmp);
    mp_exp_t e; char* s = mpf_get_str(NULL, &e, 10, pr+10, pi); mpf_clears(a,b,t,p,an,bn,tmp,pi,NULL); return s;
}
char* calcular_catalan_gmp(long pr) {
    mpf_set_default_prec((pr+100)*4); mpf_t G; mpf_init_set_ui(G, 0); mpf_t n, d, ter; mpf_inits(n,d,ter,NULL);
    for(long k=0; k<30000; k++){ mpf_set_si(n, (k%2==0)?1:-1); mpf_set_ui(d, (2*k+1)*(2*k+1)); mpf_div(ter, n, d); mpf_add(G, G, ter); }
    mp_exp_t e; char* s = mpf_get_str(NULL, &e, 10, pr+10, G); mpf_clears(G,n,d,ter,NULL); return s;
}
char* calcular_apery_gmp(long pr) {
    mpf_set_default_prec((pr+100)*4); mpf_t z; mpf_init_set_ui(z, 0); mpf_t n, d, b, ter; mpf_inits(n,d,b,ter,NULL); mpz_t zb; mpz_init(zb);
    for(long k=1; k<5000; k++){ mpz_bin_uiui(zb, 2*k, k); mpf_set_z(b, zb); mpf_set_ui(n, 1); if(k%2==0) mpf_neg(n,n); mpf_set_ui(d, k*k*k); mpf_mul(d, d, b); mpf_div(ter, n, d); mpf_add(z, z, ter); }
    mpf_mul_ui(z, z, 5); mpf_div_ui(z, z, 2); mp_exp_t e; char* s = mpf_get_str(NULL, &e, 10, pr+10, z); mpz_clear(zb); mpf_clears(z,n,d,b,ter,NULL); return s;
}
char* calcular_clausen_gmp(long pr) {
    mpf_set_default_prec((pr+100)*4); mpf_t r, s3, n, d, t; mpf_inits(r,s3,n,d,t,NULL); mpf_set_ui(s3, 3); mpf_sqrt(s3, s3);
    for (unsigned long k=1; k<8000; k++) { int i=k%6; double s=(i==1||i==2)?1.0:(i==4||i==5)?-1.0:0; if(s!=0){ mpf_set_d(n,s); mpf_set_ui(d,k*k); mpf_div(t,n,d); mpf_add(r,r,t); } }
    mpf_mul(r,r,s3); mpf_div_ui(r,r,2); mp_exp_t e; char* res = mpf_get_str(NULL,&e,10,pr+10,r); mpf_clears(r,s3,n,d,t,NULL); return res;
}
char* calcular_arctan_gmp(long pr) { char* ps=calcular_pi_gmp(pr); mpf_t pv; mpf_init_set_str(pv,ps,10); mpf_div_ui(pv,pv,4); mp_exp_t e; char* s=mpf_get_str(NULL,&e,10,pr+10,pv); free(ps); mpf_clear(pv); return s; }
char* calcular_log3_gmp(long pr) { mpf_set_default_prec((pr+100)*4); mpf_t r, t, d, tk; mpf_inits(r,t,d,tk,NULL); for(unsigned long k=1;k<5000;k++){ mpf_set_ui(tk,3); mpf_pow_ui(tk,tk,k); mpf_set_ui(d,k); mpf_mul(d,d,tk); mpf_ui_div(t,1,d); mpf_add(r,r,t); } mp_exp_t e; char* s=mpf_get_str(NULL,&e,10,pr+10,r); mpf_clears(r,t,d,tk,NULL); return s; }
char* obter_esteira_numerica(int tipo, const char* f, long prec) {
    if (TIPO_CACHE == tipo && TAMANHO_CACHE >= prec) { if (tipo != 2 || strcmp(FORMULA_ATUAL, f) == 0) return CACHE_ESTEIRA; }
    limpar_cache_esteira();
    if (tipo == 1) CACHE_ESTEIRA = calcular_pi_gmp(prec);
    else if (tipo == 3) CACHE_ESTEIRA = calcular_catalan_gmp(prec);
    else if (tipo == 4) CACHE_ESTEIRA = calcular_apery_gmp(prec);
    else if (tipo == 5) CACHE_ESTEIRA = calcular_clausen_gmp(prec);
    else if (tipo == 6) CACHE_ESTEIRA = calcular_arctan_gmp(prec);
    else if (tipo == 7) CACHE_ESTEIRA = calcular_log3_gmp(prec);
    else { CACHE_ESTEIRA = calcular_formula_custom(f, prec); strncpy(FORMULA_ATUAL, f, 255); }
    TIPO_CACHE = tipo; TAMANHO_CACHE = prec; return CACHE_ESTEIRA;
}

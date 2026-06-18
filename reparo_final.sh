#!/bin/bash
# M.A.R.T.E.L.O.S. v2.7.5 - PROTOCOLO DE SOBERANIA ALGEBRICA (ESTÁVEL)

# 1. MARTELOS.H
cat << 'E_H' > martelos.h
#ifndef MARTELOS_H
#define MARTELOS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <gmp.h>
#define MODULO 34
extern const char ALFABETO[MODULO];
int caractere_para_num(char c);
char num_para_caractere(int num);
char* remover_acentos_e_upper(const char* texto);
int converter_digito_base(char c);
void limpar_cache_esteira();
char* obter_esteira_numerica(int tipo_constante, const char* formula, long precisao_necessaria);
char* calcular_formula_custom(const char* formula, long precisao);
char* calcular_pi_gmp(long pr);
char* calcular_catalan_gmp(long pr);
char* calcular_apery_gmp(long pr);
char* calcular_clausen_gmp(long pr);
char* calcular_arctan_gmp(long pr);
char* calcular_log3_gmp(long pr);
char* ler_texto_ctrl_d(const char* instrucao);
void salvar_em_arquivo(const char* nome_arquivo, const char* conteudo);
long calcular_casa_quiasmo(const char* frase, int* out_n);
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);
char* decifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int modo);
#endif
E_H

# 2. ALFABETO.C (BLINDAGEM UTF-8)
cat << 'E_A' > alfabeto.c
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
E_A

# 3. MOTOR.C
cat << 'E_M' > motor.c
#include "martelos.h"
static char* CACHE_ESTEIRA = NULL; static long TAMANHO_CACHE = 0; static int TIPO_CACHE = -1; static char FORMULA_ATUAL[256] = "";
void limpar_cache_esteira() { if (CACHE_ESTEIRA) free(CACHE_ESTEIRA); CACHE_ESTEIRA = NULL; TAMANHO_CACHE = 0; TIPO_CACHE = -1; }
int converter_digito_base(char c) { if (isdigit(c)) return c - '0'; if (islower(c)) return c - 'a' + 10; if (isupper(c)) return c - 'A' + 36; return 0; }
char* calcular_pi_gmp(long pr) {
    mpf_set_default_prec((pr + 100) * 4); mpf_t a, b, t, p, an, bn, tmp, pi; mpf_inits(a,b,t,p,an,bn,tmp,pi,NULL);
    mpf_set_ui(a, 1); mpf_set_ui(tmp, 2); mpf_sqrt(tmp, tmp); mpf_ui_div(b, 1, tmp); mpf_set_d(t, 0.25); mpf_set_ui(p, 1);
    for (int i=0; i<25; i++) { mpf_add(an, a, b); mpf_div_ui(an, an, 2); mpf_mul(bn, a, b); mpf_sqrt(bn, bn); mpf_sub(tmp, a, an); mpf_mul(tmp, tmp, tmp); mpf_mul(tmp, tmp, p); mpf_sub(t, t, tmp); mpf_set(a, an); mpf_set(b, bn); mpf_mul_ui(p, p, 2); }
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
char* obter_esteira_numerica(int tipo, const char* f, long prec) {
    if (TIPO_CACHE == tipo && TAMANHO_CACHE >= prec) { if (tipo != 2 || strcmp(FORMULA_ATUAL, f) == 0) return CACHE_ESTEIRA; }
    limpar_cache_esteira();
    if (tipo == 1) CACHE_ESTEIRA = calcular_pi_gmp(prec);
    else if (tipo == 3) CACHE_ESTEIRA = calcular_catalan_gmp(prec);
    else if (tipo == 4) CACHE_ESTEIRA = calcular_apery_gmp(prec);
    else { CACHE_ESTEIRA = calcular_formula_custom(f, prec); strncpy(FORMULA_ATUAL, f, 255); }
    TIPO_CACHE = tipo; TAMANHO_CACHE = prec; return CACHE_ESTEIRA;
}
E_M

# 4. CRYPTO.C (SOMA MODULAR ESTÁVEL)
cat << 'E_C' > crypto.c
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
E_C

# 5. BINARIO.C
cat << 'E_B' > binario.c
#include "martelos.h"
#include <dirent.h>
void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam); printf("\n--- ARQUIVOS ---\n");
    struct dirent *dir; if (d) { while ((dir = readdir(d))) if (dir->d_type == DT_REG) printf("- %s\n", dir->d_name); closedir(d); }
}
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int modo) {
    char ni[256], no[256], pi[512], po[512]; const char* h = getenv("HOME");
    snprintf(pi, 512, "%s/Downloads/martelos/entrada/", h); snprintf(po, 512, "%s/Downloads/martelos/saida/", h);
    listar_arquivos_entrada(pi); printf("Entrada: "); scanf("%255s", ni); printf("Saída: "); scanf("%255s", no);
    char fip[1024], fop[1024]; snprintf(fip, 1024, "%s%s", pi, ni); snprintf(fop, 1024, "%s%s", po, no);
    printf("\nCamadas? "); int nf; scanf("%d", &nf); if (nf <= 0) nf = 1; getchar();
    long* cs = malloc(nf * sizeof(long)); for (int i=0; i<nf; i++) { char m[50]; sprintf(m, "FRASE %d/%d:", i+1, nf); char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); }
    char* est = obter_esteira_numerica(tipo, f, 1000000); long tam_e = strlen(est);
    FILE *fin = fopen(fip, "rb"), *fout = fopen(fop, "wb"); if (!fin || !fout) return;
    fseek(fin, 0, SEEK_END); long tt = ftell(fin); rewind(fin);
    unsigned char *buf = malloc(65536); long ini = (modo == 2) ? 10240 : 0; if (ini > tt) ini = tt/10;
    if (ini > 0) { unsigned char *hb = malloc(ini); size_t l_h = fread(hb, 1, ini, fin); fwrite(hb, 1, l_h, fout); free(hb); }
    long ptr = cs[0]; size_t l;
    while ((l = fread(buf, 1, 65536, fin)) > 0) {
        for (size_t i=0; i<l; i++) {
            unsigned char bp;
            if (acao == 1) { bp = buf[i]; for (int j=0; j<nf; j++) { long p=(ptr+cs[j])%tam_e; int s=(est[p]-'0')*100+(est[(p+1)%tam_e]-'0')*10+(est[(p+2)%tam_e]-'0'); buf[i]=(unsigned char)((buf[i]+(s%256))%256); } }
            else { for (int j=nf-1; j>=0; j--) { long p=(ptr+cs[j])%tam_e; int s=(est[p]-'0')*100+(est[(p+1)%tam_e]-'0')*10+(est[(p+2)%tam_e]-'0'); buf[i]=(unsigned char)((buf[i]-(s%256)+256)%256); } bp = buf[i]; }
            ptr += (bp == 0) ? 1 : bp;
        } fwrite(buf, 1, l, fout);
    } fclose(fin); fclose(fout); free(buf); free(cs); printf("OK!\n");
}
E_B

# 6. PARSER.C
cat << 'E_P' > parser.c
#include "martelos.h"
#include <math.h>
typedef enum { T_NUM, T_OP, T_FUN, T_LE, T_LD, T_CST } T_T;
typedef struct { T_T tipo; char s; char f[16]; char v[64]; } Tkn;
#define MAX 512
int prc(char o) { if (o == '+' || o == '-') return 1; if (o == '*' || o == '/') return 2; if (o == '^') return 3; return 0; }
void evl(Tkn* rpn, int tot, mpf_t res) {
    mpf_t p[MAX]; int t = -1;
    for (int i = 0; i < tot; i++) {
        if (rpn[i].tipo == T_NUM) { mpf_init(p[++t]); mpf_set_str(p[t], rpn[i].v, 10); } 
        else if (rpn[i].tipo == T_CST) { mpf_init(p[++t]); if (strcmp(rpn[i].v, "pi")==0) { char* s=calcular_pi_gmp(2000); mpf_set_str(p[t], s, 10); free(s); } else mpf_set_d(p[t], 2.718); }
        else if (rpn[i].tipo == T_OP) {
            if (t < 1) continue; mpf_t r; mpf_init(r);
            if (rpn[i].s == '+') mpf_add(r, p[t-1], p[t]); else if (rpn[i].s == '-') mpf_sub(r, p[t-1], p[t]);
            else if (rpn[i].s == '*') mpf_mul(r, p[t-1], p[t]); else if (rpn[i].s == '/') { if (mpf_cmp_ui(p[t], 0)==0) mpf_set_ui(r, 1); else mpf_div(r, p[t-1], p[t]); }
            else if (rpn[i].s == '^') { unsigned long ex = mpf_get_ui(p[t]); mpf_pow_ui(r, p[t-1], ex); }
            mpf_clear(p[t]); mpf_clear(p[t-1]); mpf_init_set(p[--t], r); mpf_clear(r);
        } else if (rpn[i].tipo == T_FUN) {
            if (t < 0) continue; mpf_t r; mpf_init(r);
            if (strcmp(rpn[i].f, "sqrt")==0) mpf_sqrt(r, p[t]); else if (strcmp(rpn[i].f, "sin")==0) mpf_set_d(r, sin(mpf_get_d(p[t]))); else if (strcmp(rpn[i].f, "cos")==0) mpf_set_d(r, cos(mpf_get_d(p[t])));
            mpf_clear(p[t]); mpf_init_set(p[t], r); mpf_clear(r);
        }
    } if (t >= 0) { mpf_set(res, p[t]); mpf_clear(p[t]); } else mpf_set_ui(res, 0);
}
char* calcular_formula_custom(const char* f, long p) {
    mpf_set_default_prec((p + 100) * 4); Tkn tks[MAX], rpn[MAX], p_op[MAX]; int tc=0, rc=0, pi=-1;
    for (int i=0; f[i]; ) {
        if (isspace(f[i])) { i++; continue; }
        if (isdigit(f[i]) || f[i] == '.') { tks[tc].tipo=T_NUM; int v=0; while(isdigit(f[i])||f[i]=='.') tks[tc].v[v++]=f[i++]; tks[tc].v[v]='\0'; }
        else if (isalpha(f[i])) { char tmp[16]; int v=0; while(isalpha(f[i])) tmp[v++]=f[i++]; tmp[v]='\0'; if (strcmp(tmp, "pi")==0 || strcmp(tmp, "e")==0) { tks[tc].tipo=T_CST; strcpy(tks[tc].v, tmp); } else { tks[tc].tipo=T_FUN; strcpy(tks[tc].f, tmp); } }
        else { if (f[i] == '(') tks[tc].tipo=T_LE; else if (f[i] == ')') tks[tc].tipo=T_LD; else tks[tc].tipo=T_OP; tks[tc].s=f[i++]; } tc++;
    }
    for (int i=0; i<tc; i++) {
        if (tks[i].tipo==T_NUM||tks[i].tipo==T_CST) rpn[rc++]=tks[i];
        else if (tks[i].tipo==T_FUN||tks[i].tipo==T_LE) p_op[++pi]=tks[i];
        else if (tks[i].tipo==T_LD) { while(pi>=0 && p_op[pi].tipo!=T_LE) rpn[rc++]=p_op[pi--]; pi--; }
        else { while(pi>=0 && p_op[pi].tipo==T_OP && prc(p_op[pi].s)>=prc(tks[i].s)) rpn[rc++]=p_op[pi--]; p_op[++pi]=tks[i]; }
    }
    while(pi>=0) rpn[rc++]=p_op[pi--]; mpf_t res; mpf_init(res); evl(rpn, rc, res);
    mp_exp_t ex; char* s=mpf_get_str(NULL, &ex, 10, p+10, res); mpf_clear(res); return s;
}
E_P

# 7. MAIN.C
cat << 'E_MAIN' > main.c
#include "martelos.h"
void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }
void banner() { printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v2.7.5)\n===================================================================\n"); }
void menu_bbp() { printf("\n--- MODO CONSTANTES BBP ---\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3\nEscolha: "); }
void menu_custom() { printf("\n--- MODO CUSTOM / ALGEBRAIC ---\n1 - Fórmulas Personalizadas (C-Style)\n2 - Bases Personalizadas (Em Breve)\nEscolha: "); }
void executar_texto(int tipo, const char* f, int acao) {
    char* t = ler_texto_ctrl_d("\nENTRADA DE TEXTO:");
    printf("\nCamadas de Blindagem? "); char op[10]; fgets(op, 10, stdin); int n = atoi(op); if (n<=0) n=1;
    long* cs = malloc(n * sizeof(long)); for (int i=0; i<n; i++) { char m[50]; sprintf(m, "FRASE %d/%d:", i+1, n); char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); }
    printf("\n[⚙️ ] Processando... "); fflush(stdout); clock_t ini = clock();
    char* res = (acao == 1) ? cifrar_texto_n_camadas(t, cs, n, tipo, f) : decifrar_texto_n_camadas(t, cs, n, tipo, f);
    printf("OK!\n\n--- RESULTADO (em %.4fs) ---\n%s\n", (double)(clock()-ini)/CLOCKS_PER_SEC, res);
    printf("\nSalvar (s/n)? "); char s; if (scanf(" %c", &s)==1 && (s=='s'||s=='S')) { char nm[100]; printf("Nome: "); scanf("%99s", nm); salvar_em_arquivo(nm, res); }
    free(t); free(cs); free(res); lb();
}
int main() {
    char op[10];
    while (1) {
        banner(); printf("1 - Modo BBP (Texto)\n2 - Modo Custom (Texto)\n3 - Modo Arquivos (Multimídia)\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break; int p = atoi(op); if (p == 0) break;
        if (p == 1) {
            menu_bbp(); int s = atoi(fgets(op, 10, stdin)); if(s==0)continue;
            int t = (s==1)?1:(s==2)?3:(s==3)?4:(s==4)?5:(s==5)?6:7;
            printf("1.Cripto | 2.Decripto: "); int a = atoi(fgets(op, 10, stdin)); executar_texto(t, "", a);
        } else if (p == 2) {
            menu_custom(); int s = atoi(fgets(op, 10, stdin)); if(s==1) {
                printf("Fórmula: "); char f[256]; fgets(f, 256, stdin); f[strcspn(f, "\n")] = 0;
                printf("1.Cripto | 2.Decripto: "); int a = atoi(fgets(op, 10, stdin)); executar_texto(2, f, a);
            }
        } else if (p == 3) {
            printf("\n1-Total | 2-Glitch: "); int m = atoi(fgets(op, 10, stdin));
            printf("1-Cripto | 2-Decripto: "); int a = atoi(fgets(op, 10, stdin));
            printf("Constante (1-Pi | 2-Catalan | 3-Apery): "); int c = atoi(fgets(op, 10, stdin));
            processar_arquivo_binario_n_camadas((c==1)?1:(c==2)?3:4, "", a, m);
        }
    } return 0;
}
E_MAIN

# 8. CMAKELISTS.TXT (FIX LIB MATH)
cat << 'E_K' > CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(martelos C)
set(CMAKE_C_STANDARD 11)
find_library(GMP_LIB gmp)
set(SOURCES main.c alfabeto.c motor.c io.c crypto.c parser.c binario.c)
add_executable(martelos ${SOURCES})
target_link_libraries(martelos gmp m)
E_K

echo "✅ Bancada Estável v2.7.5 Restaurada!"
cd build; rm -rf ./*; cmake ..; make; ./martelos

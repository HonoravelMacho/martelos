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

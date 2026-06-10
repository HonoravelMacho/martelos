#include "martelos.h"

typedef enum { TOKEN_NUMERO, TOKEN_OPERADOR, TOKEN_FUNCAO, TOKEN_PAR_ESQ, TOKEN_PAR_DIR } TipoToken;
typedef struct { TipoToken tipo; char simbolo; char funcao[16]; char valor_str[64]; } Token;

#define MAX_TOKENS 512

int obter_precedencia(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

void avaliar_rpn(Token* rpn, int total, mpf_t resultado) {
    mpf_t pilha[MAX_TOKENS];
    int topo = -1;

    for (int i = 0; i < total; i++) {
        if (rpn[i].tipo == TOKEN_NUMERO) {
            mpf_init(pilha[++topo]);
            mpf_set_str(pilha[topo], rpn[i].valor_str, 10);
        } else if (rpn[i].tipo == TOKEN_OPERADOR) {
            mpf_t res; mpf_init(res);
            if (rpn[i].simbolo == '+') mpf_add(res, pilha[topo-1], pilha[topo]);
            else if (rpn[i].simbolo == '-') mpf_sub(res, pilha[topo-1], pilha[topo]);
            else if (rpn[i].simbolo == '*') mpf_mul(res, pilha[topo-1], pilha[topo]);
            else if (rpn[i].simbolo == '/') mpf_div(res, pilha[topo-1], pilha[topo]);
            else if (rpn[i].simbolo == '^') {
                unsigned long exp = mpf_get_ui(pilha[topo]);
                mpf_pow_ui(res, pilha[topo-1], exp);
            }
            mpf_clear(pilha[topo]); mpf_clear(pilha[topo-1]);
            mpf_init_set(pilha[--topo], res);
            mpf_clear(res);
        } else if (rpn[i].tipo == TOKEN_FUNCAO) {
            mpf_t res; mpf_init(res);
            if (strcmp(rpn[i].funcao, "sqrt") == 0) mpf_sqrt(res, pilha[topo]);
            mpf_clear(pilha[topo]);
            mpf_init_set(pilha[topo], res);
            mpf_clear(res);
        }
    }
    mpf_set(resultado, pilha[0]);
    mpf_clear(pilha[0]);
}

char* calcular_formula_custom(const char* formula, long precisao) {
    unsigned long bits = (unsigned long)((precisao + 50) * 3.321928);
    mpf_set_default_prec(bits);

    Token tokens[MAX_TOKENS], rpn[MAX_TOKENS], pilha_op[MAX_TOKENS];
    int t_cnt = 0, r_cnt = 0, p_idx = -1;

    // Tokenizador Simples
    for (int i = 0; formula[i]; ) {
        if (isspace(formula[i])) { i++; continue; }
        if (isdigit(formula[i]) || formula[i] == '.') {
            tokens[t_cnt].tipo = TOKEN_NUMERO;
            int v = 0; while (isdigit(formula[i]) || formula[i] == '.') tokens[t_cnt].valor_str[v++] = formula[i++];
            tokens[t_cnt].valor_str[v] = '\0';
        } else if (isalpha(formula[i])) {
            tokens[t_cnt].tipo = TOKEN_FUNCAO;
            int v = 0; while (isalpha(formula[i])) tokens[t_cnt].funcao[v++] = formula[i++];
            tokens[t_cnt].funcao[v] = '\0';
        } else {
            if (formula[i] == '(') tokens[t_cnt].tipo = TOKEN_PAR_ESQ;
            else if (formula[i] == ')') tokens[t_cnt].tipo = TOKEN_PAR_DIR;
            else tokens[t_cnt].tipo = TOKEN_OPERADOR;
            tokens[t_cnt].simbolo = formula[i++];
        }
        t_cnt++;
    }

    // Shunting-Yard (Infixa -> RPN)
    for (int i = 0; i < t_cnt; i++) {
        if (tokens[i].tipo == TOKEN_NUMERO) rpn[r_cnt++] = tokens[i];
        else if (tokens[i].tipo == TOKEN_FUNCAO || tokens[i].tipo == TOKEN_PAR_ESQ) pilha_op[++p_idx] = tokens[i];
        else if (tokens[i].tipo == TOKEN_PAR_DIR) {
            while (p_idx >= 0 && pilha_op[p_idx].tipo != TOKEN_PAR_ESQ) rpn[r_cnt++] = pilha_op[p_idx--];
            p_idx--; // remove '('
        } else {
            while (p_idx >= 0 && pilha_op[p_idx].tipo == TOKEN_OPERADOR && obter_precedencia(pilha_op[p_idx].simbolo) >= obter_precedencia(tokens[i].simbolo))
                rpn[r_cnt++] = pilha_op[p_idx--];
            pilha_op[++p_idx] = tokens[i];
        }
    }
    while (p_idx >= 0) rpn[r_cnt++] = pilha_op[p_idx--];

    mpf_t res; mpf_init(res);
    avaliar_rpn(rpn, r_cnt, res);
    
    mp_exp_t exp;
    char* str = mpf_get_str(NULL, &exp, 10, precisao + 2, res);
    mpf_clear(res);
    return str;
}

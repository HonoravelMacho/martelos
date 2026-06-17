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
char* calcular_pi_gmp(long precisao_digitos);
char* calcular_catalan_gmp(long precisao_digitos);
char* calcular_apery_gmp(long precisao_digitos);
char* calcular_clausen_gmp(long precisao_digitos);
char* calcular_arctan_gmp(long precisao_digitos);
char* calcular_log3_gmp(long precisao_digitos);
char* ler_texto_ctrl_d(const char* instrucao);
void salvar_em_arquivo(const char* nome_arquivo, const char* conteudo);
long calcular_casa_quiasmo(const char* frase, int* out_n);
char* cifrar_texto_n_camadas(const char* texto, long* casas, int n_camadas, int tipo_const, const char* formula);
char* decifrar_texto_n_camadas(const char* texto, long* casas, int n_camadas, int tipo_const, const char* formula);
void processar_arquivo_binario_n_camadas(int tipo_const, const char* formula, int acao, int modo_glitch);
#endif

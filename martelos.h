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

// Funções de Texto e Alfabeto
int caractere_para_num(char c);
char num_para_caractere(int num);
char* remover_acentos_e_upper(const char* texto);
char* ler_texto_ctrl_d(const char* instrucao);
void salvar_em_arquivo_v2(const char* nome, const char* cont, int is_local);
void salvar_em_arquivo(const char* nome, const char* cont);

// Motor Matemático BBP
void limpar_cache_esteira();
char* obter_esteira_numerica(int tipo_constante, const char* formula, long precisao_necessaria);
char* calcular_formula_custom(const char* formula, long precisao);
char* calcular_pi_gmp(long pr);
char* calcular_catalan_gmp(long pr);
char* calcular_apery_gmp(long pr);
char* calcular_clausen_gmp(long pr);
char* calcular_arctan_gmp(long pr);
char* calcular_log3_gmp(long pr);

// Núcleo de Criptografia
long calcular_casa_quiasmo(const char* frase, int* out_n);
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);
char* decifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);

// Motor de Mídia (Ida e Volta v2.7 Style)
void processar_binario_core(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf);
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo);
void executar_transmutacao_cli(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf);

#endif

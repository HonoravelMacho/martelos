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

// Funções de Alfabeto e Texto
int caractere_para_num(char c);
char num_para_caractere(int num);
char* remover_acentos_e_upper(const char* texto);
char* ler_texto_ctrl_d(const char* instrucao);
void salvar_em_arquivo(const char* nome_arquivo, const char* conteudo);

// Motor de Esteira
void limpar_cache_esteira();
char* obter_esteira_numerica(int tipo_constante, const char* formula, long precisao_necessaria);
char* calcular_formula_custom(const char* formula, long precisao);
char* calcular_pi_gmp(long pr);
char* calcular_catalan_gmp(long pr);
char* calcular_apery_gmp(long pr);

// Criptografia
long calcular_casa_quiasmo(const char* frase, int* out_n);
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);
char* decifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);

// Binário e Transmutação (Modo Glitch)
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int modo);

#endif

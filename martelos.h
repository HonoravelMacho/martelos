#ifndef MARTELOS_H
#define MARTELOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <gmp.h>

#define MODULO 34

// Alfabeto Oficial
extern const char ALFABETO[MODULO];

// Motor de Alfabeto e Limpeza
int caractere_para_num(char c);
char num_para_caractere(int num);
char* remover_acentos_e_upper(const char* texto);

// Motor de Esteira (Math Engine) com Cache
void limpar_cache_esteira();
char* obter_esteira_numerica(int tipo_constante, const char* formula, long precisao_necessaria);

// Motor de Entrada/Saída (I/O)
char* ler_texto_ctrl_d(const char* instrucao);
void salvar_em_arquivo(const char* nome_arquivo, const char* conteudo);

// Protótipos de Criptografia
long calcular_casa_quiasmo(const char* frase, int* num_palavras);
char* cifrar_texto(const char* texto_puro, long casa_inicial, int tipo_const, const char* formula);

char* decifrar_texto(const char* texto_cripto, long casa_inicial, int tipo_const, const char* formula);

char* obter_esteira_bbp(int constante, long precisao);

#endif

#ifndef MARTELOS_H
#define MARTELOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h> // strcasecmp/strncasecmp (o CLI usa em -f)
#include <time.h>
#include <gmp.h>

// ============================================================================
// v7.0.0 "PROTOCOLO SALTO QUÂNTICO" — duas fontes coexistindo:
//
//  • FONTE LEGADA (v6 e anteriores): 34 símbolos. Viva APENAS nos caminhos
//    legados (-legado / opção Legado no menu), para abrir o que já existe.
//    Os vetores-ouro do teste.c congelam esses bytes: JAMAIS alterá-los.
//  • FONTE v7: teclado completo — 95 símbolos ASCII printáveis em ordem de
//    código (espaço=1 … '!'=2 … 'A'=34 … 'a'=66 … 'z'=91 … '~'=95).
//    Minúsculas, dígitos 0-9 e TODA a pontuação têm valor próprio: o balde
//    do '-' da v6 (todo desconhecido igual) morre.
//    '\n' saiu da fonte, mas VIAJA na malha UTF-8 ("=0A"): a letra cifrada
//    nunca contém quebra de linha, a cópia da tela fica honesta, e a
//    decifra devolve a entrada BYTE A BYTE — com acentos de pé (v7.1).
// ============================================================================

// Fonte legada (sinal vivo do passado — decifra somente)
#define MODULO_LEGADO 34
extern const char ALFABETO_LEGADO[MODULO_LEGADO];

// Fonte v7 (caracteres printáveis da tabela ASCII, 32..126)
#define MODULO 95

// Funções da FONTE LEGADA (v6)
#define MART_MAXEXT 32
int caractere_para_num_legado(char c);
char num_para_caractere_legado(int num);
char* remover_acentos_e_upper(const char* texto); // normalização da v6 (tudo caixa alta)

// Funções da FONTE v7
int fonte95_para_num(char c);
char fonte95_num_para_char(int num);
char* normalizar_fonte95(const char* texto); // translitera acentos mantendo caixa; controle -> espaço

// Malha UTF-8 (v7.1): memória byte a byte — acentos e quebras de linha
// sobrevivem ao salto. Todo byte fora da fonte 95 viaja como trio de glifos
// da própria fonte ('=' + 2 hex maiúsculos): 'é' -> "=C3=A9", '\n' -> "=0A",
// '=' -> "=3D". A decodificação é CANÔNICA: decodifica, re-encoda, compara —
// só aceita a leitura escaped quando o ciclo fecha byte a byte (senão, dá
// a corda de volta como veio; zero ambiguidade com texto escrito à mão).
char* malha_utf8_encode(const char* brutos);
char* malha_utf8_decode(const char* s);

// Funções de Texto e I/O
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

// Núcleo de Criptografia — LEGADO (v6, intocado; ouros do teste.c)
long calcular_casa_quiasmo(const char* frase, int* out_n);
char* cifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);
char* decifrar_texto_n_camadas(const char* t, long* cs, int n, int tipo, const char* f);

// Núcleo de Criptografia — SALTO QUÂNTICO v7
// A casa de cada frase é um NÚMERO LITERAL (posicional em base 95) — a ordem
// é sagrada: AB ≠ BA, e um soneto vira um número gigantesco sem cap.
// 'profundidade' é a leitura do QUIASMO: 0 = frase natural, 1 = espelhada,
// 2 = espelho do espelho... (a camada c lê sua frase com profundidade c).
// A esteira é dimensionada pela MENSAGEM; o ponteiro lê o oceano em
// casa % tamanho_da_esteira — o número literal nunca mais infla o pi.
void calcular_casa_salto(const char* frase, int profundidade, mpz_t out);
char* cifrar_salto(const char* t, mpz_t* casas, int n, int tipo, const char* f, int* selo_estado);
char* decifrar_salto(const char* t, mpz_t* casas, int n, int tipo, const char* f, int* selo_estado);
// selo_estado: 1 = selo-guarita verificado (frase íntegra, senha certa);
//              0 = sem selo na entrada (texto legado/v6, mensagem curta ou
//                  cópia mutilada — confira o resultado).
void liberar_casas(mpz_t* casas, int n);

// Protocolo do arquivo .martelos: 1 = MR7T (v7), 2 = MART (legado v6), 0 = sem cabeçalho
int protocolo_martelos_do_arquivo(const char* path);

// Motor de Mídia (as CASAS agora são números literais mpz; a matemática de
// bytes foi preservada byte a byte — só a origem da casa muda)
void processar_binario_core(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, mpz_t* casas, int nf);
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo, int legado);
void executar_transmutacao_cli(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, mpz_t* casas, int nf);
int ler_ext_martelos_arquivo(const char* path, char* ext_out);

#endif

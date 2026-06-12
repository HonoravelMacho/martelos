#include "martelos.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_banner() {
    printf("\n===================================================================\n");
    printf("                   🔨 Projeto M.A.R.T.E.L.O.S.  \n");
    printf("   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n");
    printf("              \"Livre e Open Source\" (v2.5)\n");
    printf("===================================================================\n");
}

void menu_bbp() {
    printf("\n--- MODO CONSTANTES BBP (TEXTO) ---\n");
    printf("1 - Pi | 2 - Catalan | 3 - Apery\n");
    printf("4 - Clausen | 5 - Arctan | 6 - Log3\n");
    printf("0 - Voltar\nEscolha: ");
}

void menu_custom() {
    printf("\n--- MODO CUSTOM / EXPERIMENTAL (TEXTO) ---\n");
    printf("1 - Constante Personalizada (Fórmula)\n");
    printf("2 - Base Personalizada (Em breve)\n");
    printf("0 - Voltar\nEscolha: ");
}

void menu_arquivos() {
    printf("\n--- MODO ARQUIVOS (UNIVERSAL BINARY) ---\n");
    printf("1 - Transmutação Total (.martelos)\n");
    printf("2 - Transmutação Parcial (Glitch Mode)\n");
    printf("0 - Voltar\nEscolha o Modo: ");
}

void executar_motor_texto(int tipo_const, const char* formula, int acao) {
    if (acao != 1 && acao != 2) return;
    char* texto = ler_texto_ctrl_d("\n[1/4] ENTRADA DE TEXTO:");
    char* f1 = ler_texto_ctrl_d("[2/4] FRASE 1 (Rounds A):");
    char* f2 = ler_texto_ctrl_d("[3/4] FRASE 2 (Rounds B):");
    char* f3 = ler_texto_ctrl_d("[4/4] FRASE 3 (Âncora Quântica):");

    printf("\n[⚙️ ] Processando... ");
    fflush(stdout);
    clock_t inicio = clock();

    int n1, n2, n3;
    long casa1 = calcular_casa_quiasmo(f1, &n1);
    long casa2 = calcular_casa_quiasmo(f2, &n2);
    long salto = calcular_casa_quiasmo(f3, &n3);

    char* resultado = NULL;
    if (acao == 1) {
        char* t1 = cifrar_texto(texto, salto, tipo_const, formula);
        char* t2;
        for(int i=0; i<n1; i++) { t2 = cifrar_texto(t1, casa1, tipo_const, formula); free(t1); t1 = t2; }
        for(int i=0; i<n2; i++) { t2 = cifrar_texto(t1, casa2, tipo_const, formula); free(t1); t1 = t2; }
        resultado = t1;
    } else {
        char* t1 = strdup(texto);
        char* t2;
        for(int i=0; i<n2; i++) { t2 = decifrar_texto(t1, casa2, tipo_const, formula); free(t1); t1 = t2; }
        for(int i=0; i<n1; i++) { t2 = decifrar_texto(t1, casa1, tipo_const, formula); free(t1); t1 = t2; }
        resultado = decifrar_texto(t1, salto, tipo_const, formula);
        free(t1);
    }
    clock_t fim = clock();
    printf("OK!\n\n--- RESULTADO (em %.4fs) ---\n%s\n", (double)(fim-inicio)/CLOCKS_PER_SEC, resultado);
    printf("\nSalvar resultado em .txt? (s/n): ");
    char s; if (scanf(" %c", &s) == 1) {
        limpar_buffer();
        if(s == 's' || s == 'S') {
            char nome[100]; printf("Nome do arquivo: "); 
            if (scanf("%99s", nome) == 1) { limpar_buffer(); salvar_em_arquivo(nome, resultado); }
        }
    }
    free(texto); free(f1); free(f2); free(f3); free(resultado);
}

int main() {
    char op[10];
    while (1) {
        exibir_banner();
        printf("1 - Modo Constantes BBP (Texto)\n2 - Modo Custom (Texto)\n3 - Modo Arquivos (Multimídia)\n0 - Sair\nEscolha: ");
        if (!fgets(op, sizeof(op), stdin)) break;
        int principal = atoi(op);
        if (principal == 0) break;

        if (principal == 1) {
            menu_bbp();
            fgets(op, sizeof(op), stdin);
            int sub = atoi(op);
            if (sub == 0) continue;
            int tipo = (sub == 1) ? 1 : (sub == 2) ? 3 : (sub == 3) ? 4 : (sub == 4) ? 5 : (sub == 5) ? 6 : (sub == 6) ? 7 : 0;
            if (tipo > 0) {
                printf("\n1. Criptografar | 2. Descriptografar\nEscolha: ");
                fgets(op, sizeof(op), stdin);
                executar_motor_texto(tipo, "", atoi(op));
            }
        } 
        else if (principal == 2) {
            menu_custom();
            fgets(op, sizeof(op), stdin);
            int sub = atoi(op);
            if (sub == 1) {
                char f[256]; printf("Digite a Fórmula: "); fgets(f, sizeof(f), stdin);
                f[strcspn(f, "\n")] = 0;
                printf("\n1. Criptografar | 2. Descriptografar\nEscolha: ");
                fgets(op, sizeof(op), stdin);
                executar_motor_texto(2, f, atoi(op));
            }
        }
        else if (principal == 3) {
            menu_arquivos();
            fgets(op, sizeof(op), stdin);
            int modo_bin = atoi(op);
            if (modo_bin == 0) continue;
            printf("\n1. Criptografar | 2. Descriptografar\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int acao_bin = atoi(op);
            printf("\nEscolha a Constante:\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int c_sub = atoi(op);
            int tipo_c = (c_sub == 1) ? 1 : (c_sub == 2) ? 3 : (c_sub == 3) ? 4 : (c_sub == 4) ? 5 : (c_sub == 5) ? 6 : (c_sub == 6) ? 7 : 1;
            processar_arquivo_binario(tipo_c, "", acao_bin, modo_bin);
        }
    }
    limpar_cache_esteira();
    printf("\nEngrenagens paradas. Até a próxima, Capitão!\n");
    return 0;
}

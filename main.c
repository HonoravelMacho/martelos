#include "martelos.h"

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_banner() {
    printf("\n===================================================================\n");
    printf("                   🔨 Projeto M.A.R.T.E.L.O.S.  \n");
    printf("   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n");
    printf("              \"Livre e Open Source\" (v2.2)\n");
    printf("===================================================================\n");
}

void menu_bbp() {
    printf("\n--- CONSTANTES BBP (ALTA PERFORMANCE) ---\n");
    printf("1 - Constante de Pi\n");
    printf("2 - Constante de Catalan\n");
    printf("3 - Constante de Apery (Zeta 3)\n");
    printf("4 - Integral de Clausen (pi/3)\n");
    printf("5 - Constante Arctan (Plouffe)\n");
    printf("6 - Logaritmo Natural de 3\n");
    printf("0 - Voltar\nEscolha: ");
}

void menu_custom() {
    printf("\n--- MODO CUSTOM / EXPERIMENTAL ---\n");
    printf("1 - Constante Personalizada (Fórmula)\n");
    printf("2 - Base Personalizada (Desenvolvimento)\n");
    printf("0 - Voltar\nEscolha: ");
}

void menu_imagem() {
    printf("\n--- MODO IMAGEM (BINÁRIO) ---\n");
    printf("1 - Criptografar Imagem\n");
    printf("2 - Descriptografar Imagem\n");
    printf("0 - Voltar\nEscolha: ");
}

void executar_motor(int tipo_const, const char* formula, int acao) {
    if (acao != 1 && acao != 2) return;
    char* texto = ler_texto_ctrl_d("\n[1/4] ENTRADA DE TEXTO:");
    char* f1 = ler_texto_ctrl_d("[2/4] FRASE 1 (Rounds A):");
    char* f2 = ler_texto_ctrl_d("[3/4] FRASE 2 (Rounds B):");
    char* f3 = ler_texto_ctrl_d("[4/4] FRASE 3 (Âncora):");

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
    printf("OK!\n\n--- RESULTADO (em %.4f segundos) ---\n%s\n", (double)(fim-inicio)/CLOCKS_PER_SEC, resultado);
    
    printf("\nSalvar arquivo? (s/n): ");
    char s; 
    if (scanf(" %c", &s) == 1) {
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
        printf("1 - Modo Constantes BBP\n2 - Modo Custom\n3 - Modo Imagem\n0 - Sair\nEscolha: ");
        if (!fgets(op, sizeof(op), stdin)) break;
        int principal = atoi(op);
        if (principal == 0) break;
        if (principal == 1) {
            menu_bbp();
            fgets(op, sizeof(op), stdin);
            int sub = atoi(op);
            if (sub == 0) continue;
            int tipo = 0;
            if (sub == 1) tipo = 1; else if (sub == 2) tipo = 3;
            else if (sub == 3) tipo = 4; else if (sub == 4) tipo = 5;
            else if (sub == 5) tipo = 6; else if (sub == 6) tipo = 7;
            if (tipo > 0) {
                printf("\n1-Cripto | 2-Decripto: "); fgets(op, sizeof(op), stdin);
                executar_motor(tipo, "", atoi(op));
            }
        } 
        else if (principal == 2) {
            menu_custom();
            fgets(op, sizeof(op), stdin);
            int sub = atoi(op);
            if (sub == 1) {
                char f[256]; printf("Fórmula: "); fgets(f, sizeof(f), stdin);
                f[strcspn(f, "\n")] = 0;
                printf("\n1-Cripto | 2-Decripto: "); fgets(op, sizeof(op), stdin);
                executar_motor(2, f, atoi(op));
            }
        }
        else if (principal == 3) {
            menu_imagem();
            fgets(op, sizeof(op), stdin);
            printf("\n[!] Modo Imagem em desenvolvimento...\n");
        }
    }
    limpar_cache_esteira();
    return 0;
}

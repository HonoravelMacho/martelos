#include "martelos.h"

void exibir_banner() {
    printf("\n===================================================================\n");
    printf("                   🔨 Projeto M.A.R.T.E.L.O.S.  \n");
    printf("   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n");
    printf("              \"Livre e Open Source\" (v2.6)\n");
    printf("===================================================================\n");
}

int main() {
    char op[10];
    while (1) {
        exibir_banner();
        printf("1 - Modo Constantes BBP (Texto)\n2 - Modo Custom (Texto)\n3 - Modo Arquivos (Multimídia)\n0 - Sair\nEscolha: ");
        if (!fgets(op, sizeof(op), stdin)) break;
        int principal = atoi(op);
        if (principal == 0) break;

        if (principal == 1 || principal == 2) {
            int tipo_c = 1; char formula[256] = "";
            if (principal == 1) {
                printf("\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3\nEscolha: ");
                fgets(op, sizeof(op), stdin);
                int sub = atoi(op);
                tipo_c = (sub == 1) ? 1 : (sub == 2) ? 3 : (sub == 3) ? 4 : (sub == 4) ? 5 : (sub == 5) ? 6 : (sub == 6) ? 7 : 1;
            } else {
                printf("Digite a Fórmula: "); fgets(formula, sizeof(formula), stdin);
                formula[strcspn(formula, "\n")] = 0; tipo_c = 2;
            }

            printf("\n1. Criptografar | 2. Descriptografar\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int acao = atoi(op);

            char* texto = ler_texto_ctrl_d("\nENTRADA DE TEXTO:");
            printf("\nQuantas camadas de blindagem (frases)? ");
            fgets(op, sizeof(op), stdin);
            int n = atoi(op); if (n <= 0) n = 1;

            long* casas = malloc(n * sizeof(long));
            for (int i = 0; i < n; i++) {
                char msg[50]; sprintf(msg, "FRASE %d/%d:", i+1, n);
                char* f = ler_texto_ctrl_d(msg);
                int dummy; casas[i] = calcular_casa_quiasmo(f, &dummy);
                free(f);
            }

            char* res = (acao == 1) ? cifrar_texto_n_camadas(texto, casas, n, tipo_c, formula) 
                                   : decifrar_texto_n_camadas(texto, casas, n, tipo_c, formula);
            
            printf("\n--- RESULTADO ---\n%s\n", res);
            free(texto); free(casas); free(res);
        }
        else if (principal == 3) {
            printf("\n1 - Total (.martelos) | 2 - Glitch Mode\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int modo = atoi(op);
            printf("\n1. Criptografar | 2. Descriptografar\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int acao = atoi(op);
            printf("\nConstante: 1-Pi | 2-Catalan | 3-Apery | 4-Clausen\nEscolha: ");
            fgets(op, sizeof(op), stdin);
            int c_sub = atoi(op);
            int tipo_c = (c_sub == 1) ? 1 : (c_sub == 2) ? 3 : (c_sub == 3) ? 4 : 5;
            processar_arquivo_binario_n_camadas(tipo_c, "", acao, modo);
        }
    }
    return 0;
}

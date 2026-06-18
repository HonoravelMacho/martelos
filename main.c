#include "martelos.h"

void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }

void banner() { 
    printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v5.7.1)\n===================================================================\n"); 
}

int menu_constantes(int *tipo, char *formula) {
    printf("\n--- ESCOLHA A CONSTANTE ALGEBRICA ---\n");
    printf("1 - Pi\n2 - Catalan\n3 - Apery\n4 - Clausen\n5 - Arctan\n6 - Log3\n7 - Custom (Fórmula)\n0 - Voltar\nEscolha: ");
    char op[10]; if(!fgets(op, 10, stdin)) return 0;
    int s = atoi(op);
    if (s == 0) return 0;
    if (s == 7) {
        *tipo = 2;
        printf("Fórmula C-Style (ou 0 para Voltar): "); fgets(formula, 256, stdin);
        if (strcmp(formula, "0\n") == 0) return 0;
        formula[strcspn(formula, "\n")] = 0;
    } else {
        *tipo = (s==1)?1 : (s==2)?3 : (s==3)?4 : (s==4)?5 : (s==5)?6 : 7;
        strcpy(formula, "");
    }
    return 1;
}

void executar_texto(int tipo, const char* f, int acao) {
    char* t = ler_texto_ctrl_d("\nENTRADA DE TEXTO:");
    if (strcmp(t, "0") == 0) { free(t); return; }
    
    printf("\nCamadas de Blindagem? (ou 0 para Voltar) "); char op[10]; fgets(op, 10, stdin);
    int n = atoi(op); if (n == 0) { free(t); return; } if (n < 0) n = 1;
    
    long* cs = malloc(n * sizeof(long));
    for (int i=0; i<n; i++) {
        char m[50]; sprintf(m, "FRASE %d/%d:", i+1, n);
        char* fr = ler_texto_ctrl_d(m); 
        int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr);
    }
    printf("\n[⚙️] Processando... "); fflush(stdout); clock_t ini = clock();
    char* res = (acao == 1) ? cifrar_texto_n_camadas(t, cs, n, tipo, f) : decifrar_texto_n_camadas(t, cs, n, tipo, f);
    printf("OK!\n\n--- RESULTADO (em %.4fs) ---\n%s\n", (double)(clock()-ini)/CLOCKS_PER_SEC, res);
    printf("\nSalvar (s/n)? "); char s; if (scanf(" %c", &s)==1 && (s=='s'||s=='S')) { char nm[100]; printf("Nome: "); scanf("%99s", nm); salvar_em_arquivo(nm, res); }
    free(t); free(cs); free(res); lb();
}

int main() {
    char op[10];
    while (1) {
        banner();
        printf("1 - Modo Texto (BBP/Custom)\n2 - Modo Arquivos (Multimídia)\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break;
        int p = atoi(op);
        if (p == 0) break;

        if (p == 1) {
            int t; char f[256];
            if (!menu_constantes(&t, f)) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin));
            if (a == 0) continue;
            executar_texto(t, f, a);
        } 
        else if (p == 2) {
            printf("\n--- MODO MULTIMIDIA ---\n");
            printf("1 - Martelos (Qualquer arquivo -> .martelos)\n");
            printf("2 - Imagens  (JPG, PNG, WEBP, GIF, TIFF, SVG, etc -> .bmp)\n");
            printf("3 - Audios   (MP3, FLAC, M4A, AAC, OGG, WMA, etc -> .wav)\n");
            printf("4 - Videos   (Livre - AVI, MP4, MKV, MOV, MPG, etc)\n");
            printf("0 - Voltar\nEscolha: ");
            int sub = atoi(fgets(op, 10, stdin));
            if (sub == 0) continue;

            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin));
            if (a == 0) continue;

            int t; char f[256];
            if (!menu_constantes(&t, f)) continue;
            processar_arquivo_binario_n_camadas(t, f, a, sub);
        }
    }
    return 0;
}

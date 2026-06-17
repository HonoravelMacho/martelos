#include "martelos.h"
void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }
void banner() { printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v2.7.4)\n===================================================================\n"); }
void menu_bbp() { printf("\n--- MODO CONSTANTES BBP ---\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3\nEscolha: "); }
void menu_custom() { printf("\n--- MODO CUSTOM / ALGEBRAIC ---\n1 - Fórmulas Personalizadas (C-Style)\n2 - Bases Personalizadas (Em Breve)\nEscolha: "); }
void executar_texto(int tipo, const char* f, int acao) {
    char* t = ler_texto_ctrl_d("\nENTRADA DE TEXTO:");
    printf("\nCamadas de Blindagem? "); char op[10]; fgets(op, 10, stdin); int n = atoi(op); if (n<=0) n=1;
    long* cs = malloc(n * sizeof(long)); for (int i=0; i<n; i++) { char m[50]; sprintf(m, "FRASE %d/%d:", i+1, n); char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); }
    printf("\n[⚙️ ] Processando... "); fflush(stdout); clock_t ini = clock();
    char* res = (acao == 1) ? cifrar_texto_n_camadas(t, cs, n, tipo, f) : decifrar_texto_n_camadas(t, cs, n, tipo, f);
    printf("OK!\n\n--- RESULTADO (em %.4fs) ---\n%s\n", (double)(clock()-ini)/CLOCKS_PER_SEC, res);
    printf("\nSalvar (s/n)? "); char s; if (scanf(" %c", &s)==1 && (s=='s'||s=='S')) { char nm[100]; printf("Nome: "); scanf("%99s", nm); salvar_em_arquivo(nm, res); }
    free(t); free(cs); free(res); lb();
}
int main() {
    char op[10];
    while (1) {
        banner(); printf("1 - Modo BBP (Texto)\n2 - Modo Custom (Texto)\n3 - Modo Arquivos (Multimídia)\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break; int p = atoi(op); if (p == 0) break;
        if (p == 1) {
            menu_bbp(); int s = atoi(fgets(op, 10, stdin)); if(s==0)continue;
            int t = (s==1)?1:(s==2)?3:(s==3)?4:(s==4)?5:(s==5)?6:7;
            printf("1.Cripto | 2.Decripto: "); int a = atoi(fgets(op, 10, stdin)); executar_texto(t, "", a);
        } else if (p == 2) {
            menu_custom(); int s = atoi(fgets(op, 10, stdin)); if(s==1) {
                printf("Fórmula: "); char f[256]; fgets(f, 256, stdin); f[strcspn(f, "\n")] = 0;
                printf("1.Cripto | 2.Decripto: "); int a = atoi(fgets(op, 10, stdin)); executar_texto(2, f, a);
            }
        } else if (p == 3) {
            printf("\n1-Total | 2-Glitch: "); int m = atoi(fgets(op, 10, stdin));
            printf("1-Cripto | 2-Decripto: "); int a = atoi(fgets(op, 10, stdin));
            printf("Constante (1-Pi | 2-Catalan | 3-Apery): "); int c = atoi(fgets(op, 10, stdin));
            processar_arquivo_binario_n_camadas((c==1)?1:(c==2)?3:4, "", a, m);
        }
    } return 0;
}

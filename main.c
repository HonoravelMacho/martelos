#include "martelos.h"

#define IGNORE_RET(x) if(x){} 

void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }

void banner() { 
    printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v6.1.2)\n===================================================================\n"); 
}

void exibir_documentacao_flags() {
    printf("\n===================================================================\n");
    printf("                  📖 MANUAL DE FLAGS (MODO LOCAL ./)\n");
    printf("===================================================================\n");
    printf("IMPORTANTE: Envolva textos e chaves em aspas: \"#texto#\"\n");
    printf("No modo FLAGS, todos os arquivos sao lidos e salvos na pasta atual.\n\n");
    printf("Uso: ./martelos [operação] [alvo] [entrada] [opções...]\n\n");
    printf("ALVOS:\n  -s \"#texto#\"   Texto Direto\n  -i [arquivo]   Imagem\n  -a [arquivo]   Audio\n  -v [arquivo]   Video\n  -m [arquivo]   Arquivo Qualquer\n\n");
    printf("PARÂMETROS:\n  -k [qtd] \"#chaves#\"\n  -f [constante ou \"#formula#\"]\n\n");
    printf("SAÍDA:\n  -save          Gera arquivo local (cli_output.txt)\n  -print         Mostra no terminal\n===================================================================\n");
}

char* extrair_cerquilha(int* idx, int argc, char** argv) {
    static char buffer[2048]; buffer[0] = '\0';
    if (*idx >= argc) return NULL;
    char* arg = argv[*idx];
    if (arg[0] == '#') {
        size_t len = strlen(arg);
        if (len > 1 && arg[len-1] == '#') {
            strncpy(buffer, arg + 1, len - 2); buffer[len-2] = '\0';
            return buffer;
        }
        strcpy(buffer, arg + 1);
        while (++(*idx) < argc) {
            strcat(buffer, " "); strcat(buffer, argv[*idx]);
            size_t b_len = strlen(buffer);
            if (b_len > 0 && buffer[b_len-1] == '#') { buffer[b_len-1] = '\0'; return buffer; }
        }
    }
    return argv[*idx];
}

void processar_flags(int argc, char** argv) {
    int acao = 0, alvo = 0, tipo_const = 1;
    char entrada[1024] = "", formula[1024] = "", fop[1024] = "";
    int n_camadas = 0; long* chaves = NULL; int salvar = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) acao = 1;
        else if (strcmp(argv[i], "-d") == 0) acao = 2;
        else if (strcmp(argv[i], "-s") == 0) { alvo = 1; i++; char* s = extrair_cerquilha(&i, argc, argv); if(s) strcpy(entrada, s); }
        else if (strcmp(argv[i], "-i") == 0) { alvo = 2; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.bmp", entrada); } }
        else if (strcmp(argv[i], "-a") == 0) { alvo = 3; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.wav", entrada); } }
        else if (strcmp(argv[i], "-v") == 0) { alvo = 4; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.avi", entrada); } }
        else if (strcmp(argv[i], "-m") == 0) { alvo = 5; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s.martelos", entrada); } }
        else if (strcmp(argv[i], "-k") == 0) {
            i++; if(i < argc) {
                n_camadas = atoi(argv[i]);
                if (n_camadas > 0) {
                    chaves = malloc(n_camadas * sizeof(long));
                    for (int j = 0; j < n_camadas; j++) { i++; char* ck = extrair_cerquilha(&i, argc, argv); int d; if(ck) chaves[j] = calcular_casa_quiasmo(ck, &d); }
                }
            }
        }
        else if (strcmp(argv[i], "-f") == 0) {
            i++; if(i < argc) {
                if (strcasecmp(argv[i], "pi") == 0) tipo_const = 1;
                else if (strcasecmp(argv[i], "catalan") == 0) tipo_const = 3;
                else if (strcasecmp(argv[i], "apery") == 0) tipo_const = 4;
                else if (strcasecmp(argv[i], "clausen") == 0) tipo_const = 5;
                else if (strcasecmp(argv[i], "arctan") == 0) tipo_const = 6;
                else if (strcasecmp(argv[i], "log3") == 0) tipo_const = 7;
                else { tipo_const = 2; char* f_str = extrair_cerquilha(&i, argc, argv); if(f_str) strcpy(formula, f_str); }
            }
        }
        else if (strcmp(argv[i], "-save") == 0) salvar = 1;
    }

    if (alvo == 1 && chaves && acao > 0) {
        char* res = (acao == 1) ? cifrar_texto_n_camadas(entrada, chaves, n_camadas, tipo_const, formula) : decifrar_texto_n_camadas(entrada, chaves, n_camadas, tipo_const, formula);
        printf("\n--- RESULTADO CLI ---\n[%s]\n---------------------\n", res);
        if (salvar) {
            extern void salvar_em_arquivo_v2(const char*, const char*, int);
            salvar_em_arquivo_v2("cli_output.txt", res, 1);
        }
        free(res);
    } else if (alvo >= 2 && acao > 0 && chaves) {
        executar_transmutacao_cli(entrada, fop, acao, alvo, tipo_const, formula, chaves, n_camadas);
    }
    if(chaves) free(chaves);
}

int menu_constantes(int *tipo, char *formula) {
    printf("\n--- ESCOLHA A CONSTANTE ALGEBRICA ---\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3 | 7-Custom\n0-Voltar. Escolha: ");
    char op[10]; if(!fgets(op, 10, stdin)) return 0; int s = atoi(op);
    if (s == 0) return 0;
    if (s == 7) { *tipo = 2; printf("Fórmula: "); IGNORE_RET(fgets(formula, 256, stdin)); formula[strcspn(formula, "\n")] = 0; }
    else { *tipo = (s==1)?1 : (s==2)?3 : (s==3)?4 : (s==4)?5 : (s==5)?6 : 7; strcpy(formula, ""); }
    return 1;
}

int main(int argc, char** argv) {
    if (argc > 1) { 
        if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) { exibir_documentacao_flags(); return 0; }
        processar_flags(argc, argv); return 0; 
    }
    char op[10];
    while (1) {
        banner();
        printf("1 - Modo Texto\n2 - Modo Arquivos\n3 - Documentação Flags\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break;
        int p = atoi(op); if (p == 0) break;
        if (p == 3) { exibir_documentacao_flags(); printf("Pressione Enter..."); getchar(); continue; }
        if (p == 1) {
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin)); if (a == 0) continue;
            char* txt = ler_texto_ctrl_d("ENTRADA:"); if(strcmp(txt,"0")==0){ free(txt); continue; }
            printf("Camadas? "); int nl; IGNORE_RET(scanf("%d", &nl)); lb(); if(nl<=0) nl=1;
            long* cs = malloc(nl * sizeof(long));
            for(int i=0; i<nl; i++){ char m[50]; sprintf(m,"CHAVE %d:",i+1); char* fr=ler_texto_ctrl_d(m); int d; cs[i]=calcular_casa_quiasmo(fr,&d); free(fr); }
            char* res = (a==1)?cifrar_texto_n_camadas(txt,cs,nl,t,f):decifrar_texto_n_camadas(txt,cs,nl,t,f);
            printf("\n--- RESULTADO ---\n[%s]\n", res);
            printf("Salvar (s/n)? "); char so; if(scanf(" %c",&so)==1 && (so=='s'||so=='S')){ char nm[100]; printf("Nome: "); IGNORE_RET(scanf("%99s",nm)); 
            salvar_em_arquivo(nm,res); }
            free(txt); free(cs); free(res); lb();
        } else if (p == 2) {
            printf("\n1-Martelos | 2-Imagens | 3-Audios | 4-Videos | 0-Voltar: "); int sub = atoi(fgets(op, 10, stdin));
            if (sub == 0) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin)); if (a == 0) continue;
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            processar_arquivo_binario_n_camadas(t, f, a, sub);
        }
    }
    return 0;
}

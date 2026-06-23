#include "martelos.h"

#define IGNORE_RET(x) if(x){} 

void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }

void banner() { 
    printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v6.2.5)\n===================================================================\n"); 
}

void exibir_documentacao_flags() {
    printf("\n===================================================================");
    printf("\n                  📖 MANUAL DE OPERACAO CLI (v6.2.5)");
    printf("\n===================================================================");
    printf("\n SINTAXE: ./martelos [acao] [alvo] [arquivo/texto] [parametros]");
    printf("\n\n OPERACOES:");
    printf("\n   -c                Criptografar");
    printf("\n   -d                Descriptografar");
    printf("\n\n ALVOS (Pasta Local ./):");
    printf("\n   -s \"#texto#\"      String (Envolva em aspas e #)");
    printf("\n   -i [img]          Imagem -> BMP Raw");
    printf("\n   -a [audio]        Audio  -> WAV PCM");
    printf("\n   -v [video]        Video  -> AVI Raw (Chiado)");
    printf("\n   -m [file]         Qualquer arquivo -> .martelos");
    printf("\n\n PARAMETROS:");
    printf("\n   -k [n] \"#ch#\"     Qtd de camadas e as frases entre #");
    printf("\n   -f [const]        pi, catalan, apery, clausen, arctan, log3");
    printf("\n   -save / -print    Salvar .txt ou apenas exibir (para -s)");
    printf("\n\n EXEMPLO:");
    printf("\n   ./martelos -c -s \"#Ataque#\" -k 1 \"#Senha#\" -f pi -print");
    printf("\n===================================================================\n");
}

char* extrair_cerquilha(int* idx, int argc, char** argv) {
    static char buffer[2048]; buffer[0] = '\0';
    if (*idx >= argc) return NULL;
    char* arg = argv[*idx];
    if (arg[0] == '#') {
        size_t len = strlen(arg);
        if (len > 1 && arg[len-1] == '#') { strncpy(buffer, arg + 1, len - 2); buffer[len-2] = '\0'; return buffer; }
        strcpy(buffer, arg + 1);
        while (++(*idx) < argc) { strcat(buffer, " "); strcat(buffer, argv[*idx]);
            size_t b_len = strlen(buffer); if (b_len > 0 && buffer[b_len-1] == '#') { buffer[b_len-1] = '\0'; return buffer; }
        }
    } return argv[*idx];
}

void processar_flags(int argc, char** argv) {
    int acao = 0, alvo = 0, tipo_const = 1;
    char entrada[1024] = "", formula[1024] = "", fop[1024] = "";
    int n_layers = 0; long* chaves = NULL; int salvar = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) acao = 1;
        else if (strcmp(argv[i], "-d") == 0) acao = 2;
        else if (strcmp(argv[i], "-s") == 0) { alvo = 1; i++; char* s = extrair_cerquilha(&i, argc, argv); if(s) strcpy(entrada, s); }
        else if (strcmp(argv[i], "-i") == 0) { alvo = 2; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.bmp", entrada); } }
        else if (strcmp(argv[i], "-a") == 0) { alvo = 3; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.wav", entrada); } }
        else if (strcmp(argv[i], "-v") == 0) { alvo = 4; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.avi", entrada); } }
        else if (strcmp(argv[i], "-m") == 0) { alvo = 5; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s.martelos", entrada); } }
        else if (strcmp(argv[i], "-k") == 0) {
            i++; if(i < argc) { n_layers = atoi(argv[i]); if (n_layers > 0) { chaves = malloc(n_layers * sizeof(long));
            for (int j = 0; j < n_layers; j++) { i++; char* ck = extrair_cerquilha(&i, argc, argv); int d; if(ck) chaves[j] = calcular_casa_quiasmo(ck, &d); } } }
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
        char* res = (acao == 1) ? cifrar_texto_n_camadas(entrada, chaves, n_layers, tipo_const, formula) : decifrar_texto_n_camadas(entrada, chaves, n_layers, tipo_const, formula);
        printf("\n--- RESULTADO CLI ---\n[%s]\n", res);
        if (salvar) salvar_em_arquivo_v2("cli_output.txt", res, 1);
        free(res);
    } else if (alvo >= 2 && acao > 0 && chaves) {
        executar_transmutacao_cli(entrada, fop, acao, alvo-1, tipo_const, formula, chaves, n_layers);
    } else {
        printf("\n[❌ ERRO] Parametros invalidos. Consulte: ./martelos --help\n");
    }
    if(chaves) free(chaves);
}

int menu_constantes(int *tipo, char *formula) {
    printf("\n--- CONSTANTES BBP ---\n1-Pi | 2-Catalan | 3-Apery | 4-Clausen | 5-Arctan | 6-Log3 | 7-Custom\n0-Voltar: ");
    char op[10]; if(!fgets(op, 10, stdin)) return 0; int s = atoi(op);
    if (s == 0) return 0;
    if (s == 7) { *tipo = 2; printf("Formula: "); IGNORE_RET(fgets(formula, 256, stdin)); formula[strcspn(formula, "\n")] = 0; }
    else { *tipo = (s==1)?1 : (s==2)?3 : (s==3)?4 : (s==4)?5 : (s==5)?6 : 7; strcpy(formula, ""); }
    return 1;
}

int main(int argc, char** argv) {
    if (argc > 1) { if (strcmp(argv[1], "--help") == 0) { exibir_documentacao_flags(); return 0; } processar_flags(argc, argv); return 0; }
    char op[10];
    while (1) {
        banner(); printf("1 - Modo Texto\n2 - Modo Arquivos\n3 - Documentação Flags\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break;
        int p = atoi(op); if (p == 0) break;
        if (p == 3) { exibir_documentacao_flags(); printf("\nPressione Enter..."); getchar(); continue; }
        if (p == 1) {
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin)); if (a == 0) continue;
            char* txt = ler_texto_ctrl_d("ENTRADA:"); if(strcmp(txt,"0")==0){ free(txt); continue; }
            printf("Camadas? "); int nl; IGNORE_RET(scanf("%d", &nl)); lb(); if(nl<=0) nl=1;
            long* cs = malloc(nl * sizeof(long));
            for(int i=0; i<nl; i++){ char m[50]; sprintf(m,"CHAVE %d:",i+1); char* fr=ler_texto_ctrl_d(m); int d; cs[i]=calcular_casa_quiasmo(fr,&d); free(fr); }
            char* res = (a==1)?cifrar_texto_n_camadas(txt,cs,nl,t,f):decifrar_texto_n_camadas(txt,cs,nl,t,f);
            printf("\n--- RESULTADO ---\n[%s]\n", res);
            printf("Salvar (s/n)? "); char so; if(scanf(" %c",&so)==1 && (so=='s'||so=='S')){ char nm[100]; printf("Nome: "); IGNORE_RET(scanf("%99s",nm)); salvar_em_arquivo(nm,res); }
            free(txt); free(cs); free(res); lb();
        } else if (p == 2) {
            printf("\n--- MODO MULTIMIDIA ---\n1-Martelos | 2-Imagens | 3-Audios | 4-Videos | 0-Voltar: "); int sub = atoi(fgets(op, 10, stdin));
            if (sub == 0) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin)); if (a == 0) continue;
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            processar_arquivo_binario_n_camadas(t, f, a, sub);
        }
    } return 0;
}

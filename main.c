#include "martelos.h"

// Macro para silenciar avisos de ignorar valor de retorno
#define IGNORE_RET(x) if(x){} 

void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }

void banner() { 
    printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v6.0.4)\n===================================================================\n"); 
}

void exibir_documentacao_flags() {
    printf("\n===================================================================\n");
    printf("                  📖 MANUAL DE FLAGS DO M.A.R.T.E.L.O.S.\n");
    printf("===================================================================\n");
    printf("IMPORTANTE: No Linux/Termux, envolva argumentos com # em aspas \"#...#\"\n\n");
    printf("Uso: ./martelos [operação] [alvo] [entrada] [opções...]\n\n");
    printf("OPERAÇÕES:\n  -c                   Criptografar\n  -d                   Descriptografar\n\n");
    printf("ALVOS (Pasta Local ./):\n  -s \"#texto#\"         String/Texto (Sempre entre aspas e #)\n  -i [arquivo]         Imagem (Auto-BMP)\n  -a [arquivo]         Áudio (Auto-WAV)\n  -v [arquivo]         Vídeo (Retro-AVI)\n  -m [arquivo]         Martelar Geral (.martelos)\n\n");
    printf("PARÂMETROS OBRIGATÓRIOS:\n  -k [qtd] \"#chaves#\"  Qtd de chaves e as chaves entre aspas e #\n  -b 10                Base (Padrão Decimal)\n  -f [constante]       pi, catalan, apery, clausen, arctan, log3\n                       ou fórmula matemática entre \"#...#\"\n\n");
    printf("SAÍDA (Modo Texto):\n  -save                Gera cli_output.txt\n  -print               Exibe apenas no terminal\n\n");
    printf("EXEMPLO REAL:\n  ./martelos -c -s \"#Ataque#\" -k 1 \"#Senha123#\" -b 10 -f pi -print\n");
    printf("===================================================================\n");
    printf("Pressione Enter para voltar..."); getchar();
}

char* extrair_cerquilha(int* idx, int argc, char** argv) {
    static char buffer[2048]; buffer[0] = '\0';
    if (*idx >= argc) return NULL;

    char* arg = argv[*idx];
    if (arg[0] == '#') {
        size_t len = strlen(arg);
        if (len > 1 && arg[len-1] == '#') {
            strncpy(buffer, arg + 1, len - 2);
            buffer[len-2] = '\0';
            return buffer;
        }
        strcpy(buffer, arg + 1);
        while (++(*idx) < argc) {
            strcat(buffer, " ");
            strcat(buffer, argv[*idx]);
            size_t b_len = strlen(buffer);
            if (b_len > 0 && buffer[b_len-1] == '#') {
                buffer[b_len-1] = '\0';
                return buffer;
            }
        }
    }
    return argv[*idx];
}

void processar_flags(int argc, char** argv) {
    int acao = 0, alvo = 0, tipo_const = 1;
    char entrada[1024] = "", formula[1024] = "", fop[1024] = "";
    int n_camadas = 0; long* chaves = NULL;
    int salvar = 0;

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
        else if (strcmp(argv[i], "-print") == 0) salvar = 0;
    }

    if (alvo == 1 && chaves && acao > 0) {
        char* res = (acao == 1) ? cifrar_texto_n_camadas(entrada, chaves, n_camadas, tipo_const, formula) : decifrar_texto_n_camadas(entrada, chaves, n_camadas, tipo_const, formula);
        printf("\n--- RESULTADO CLI ---\n%s\n", res);
        if (salvar) salvar_em_arquivo("cli_output.txt", res);
        free(res);
    } else if (alvo >= 2 && acao > 0 && chaves) {
        executar_transmutacao_cli(entrada, fop, acao, alvo, tipo_const, formula, chaves, n_camadas);
    } else {
        printf("\n[❌ ERRO CLI] Parametros incompletos ou invalidos.\nConsulte: ./martelos --help\n");
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
        processar_flags(argc, argv);
        return 0;
    }

    char op[10];
    while (1) {
        banner();
        printf("1 - Modo Texto\n2 - Modo Arquivos\n3 - Documentação Flags\n0 - Sair\nEscolha: ");
        if (!fgets(op, 10, stdin)) break;
        int p = atoi(op);
        if (p == 0) break;
        if (p == 3) { exibir_documentacao_flags(); continue; }
        if (p == 1) {
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); int a = atoi(fgets(op, 10, stdin)); if (a == 0) continue;
            char* txt = ler_texto_ctrl_d("ENTRADA:"); if(strcmp(txt,"0")==0){ free(txt); continue; }
            printf("Camadas? "); int n_layers; IGNORE_RET(scanf("%d", &n_layers)); lb(); if(n_layers<=0) n_layers=1;
            long* cs = malloc(n_layers * sizeof(long));
            for(int i=0; i<n_layers; i++){ char m[50]; sprintf(m,"CHAVE %d:",i+1); char* fr=ler_texto_ctrl_d(m); int d; cs[i]=calcular_casa_quiasmo(fr,&d); free(fr); }
            char* res = (a==1)?cifrar_texto_n_camadas(txt,cs,n_layers,t,f):decifrar_texto_n_camadas(txt,cs,n_layers,t,f);
            printf("\n--- RESULTADO ---\n%s\n", res);
            printf("Salvar (s/n)? "); char s_opt; if(scanf(" %c",&s_opt)==1 && (s_opt=='s'||s_opt=='S')){ char nm[100]; printf("Nome: "); IGNORE_RET(scanf("%99s",nm)); salvar_em_arquivo(nm,res); }
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

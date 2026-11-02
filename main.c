#include "martelos.h"

#define IGNORE_RET(x) if(x){}

void lb() { int c; while ((c = getchar()) != '\n' && c != EOF); }

void banner() {
    printf("\n===================================================================\n                   🔨 Projeto M.A.R.T.E.L.O.S.  \n   \"Motor Algébrico Ruptivo Transformacional Experimental\"\n              \"Livre e Open Source\" (v7.0.0 — Protocolo Salto Quântico)\n===================================================================\n");
}

void exibir_documentacao_flags() {
    printf("\n===================================================================");
    printf("\n                  📖 MANUAL DE OPERACAO CLI (v7.0.0)");
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
    printf("\n   -legado           Decifrar com as CHAVES da v6 (soma antiga)");
    printf("\n   -save / -print    Salvar .txt ou apenas exibir (para -s)");
    printf("\n   -o [nome]         Nome do arquivo de saida (padrao: *_glitch.*)");
    printf("\n\n EXEMPLO:");
    printf("\n   ./martelos -c -s \"#Ataque#\" -k 1 \"#Senha#\" -f pi -print");
    printf("\n\n NOTAS v7:");
    printf("\n   • A frase-chave vira numero literal (ordem sagrada: AB != BA).");
    printf("\n   • A cifra v7 sai sem molduras e com 2 símbolos de selo no meio;");
    printf("\n     eles avisam senha errada/copiudosha mutilada na decifra.");
    printf("\n   • Texto/midia da v6: use -legado para decifrar (nada recifrado).");
    printf("\n   • Mídia de glitch cru da v6 sem marcador: -legado na decifra.");
    printf("\n===================================================================\n");
}

char* extrair_cerquilha(int* idx, int argc, char** argv) {
    static char buffer[2048]; buffer[0] = '\0';
    if (*idx >= argc) return NULL;
    char* arg = argv[*idx];
    if (arg[0] == '#') {
        size_t len = strlen(arg);
        if (len >= 2 && arg[len-1] == '#') { strncpy(buffer, arg + 1, len - 2); buffer[len-2] = '\0'; return buffer; }
        // CORREÇÃO v7: cerquilha aberta não engole mais a fila de argumentos
        // (-f, -k etc eram devorados e a camada ficava sem frase). Só é frase
        // válida se algum argumento terminar em #; os espaços internos contam.
        strcpy(buffer, arg + 1);
        while (++(*idx) < argc) {
            strcat(buffer, " "); strcat(buffer, argv[*idx]);
            size_t b_len = strlen(buffer);
            if (b_len > 0 && buffer[b_len-1] == '#') { buffer[b_len-1] = '\0'; return buffer; }
        }
        printf("\n[❌] Cerquilha sem fecho: cada frase-chave precisa abrir e fechar com # (ex.: \"#assim#\").\n");
        return NULL;
    } return argv[*idx];
}

// Protótipos dos helpers legados de texto (definidos mais abaixo): o CLI os
// usa em processar_flags, que aparece antes das definições.
static long* casas_para_legado(mpz_t* casas, int n);
static char* arma_legado_cifrar(const char* t, mpz_t* casas, int n, int tipo, const char* f);
static char* arma_legado_decifrar(const char* t, mpz_t* casas, int n, int tipo, const char* f);

void processar_flags(int argc, char** argv) {
    int acao = 0, alvo = 0, tipo_const = 1;
    char entrada[1024] = "", formula[1024] = "", fop[1024] = "";
    int n_layers = 0; mpz_t* casas = NULL;
    char** frases = NULL; int cerq_erro = 0;
    int salvar = 0; int legado = 0;
    char saida_custom[1024] = ""; int tem_saida_custom = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-c") == 0) acao = 1;
        else if (strcmp(argv[i], "-d") == 0) acao = 2;
        else if (strcmp(argv[i], "-legado") == 0) legado = 1;
        else if (strcmp(argv[i], "-s") == 0) { alvo = 1; i++; char* s = extrair_cerquilha(&i, argc, argv); if (s) { entrada[0] = '\0'; strcpy(entrada, s); } else cerq_erro = 1; }
        else if (strcmp(argv[i], "-i") == 0) { alvo = 2; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.bmp", entrada); } }
        else if (strcmp(argv[i], "-a") == 0) { alvo = 3; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.wav", entrada); } }
        else if (strcmp(argv[i], "-v") == 0) { alvo = 4; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s_glitch.avi", entrada); } }
        else if (strcmp(argv[i], "-m") == 0) { alvo = 5; i++; if(i < argc) { strcpy(entrada, argv[i]); sprintf(fop, "%s.martelos", entrada); } }
        else if (strcmp(argv[i], "-k") == 0) {
            i++; if(i < argc) {
                n_layers = atoi(argv[i]);
                if (n_layers > 0) {
                    frases = malloc((size_t)n_layers * sizeof(char*));
                    for (int j = 0; j < n_layers; j++) frases[j] = NULL;
                    for (int j = 0; j < n_layers; j++) {
                        i++;
                        char* ck = (i < argc) ? extrair_cerquilha(&i, argc, argv) : NULL;
                        if (ck) frases[j] = strdup(ck); else { cerq_erro = 1; break; }
                    }
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
        else if (strcmp(argv[i], "-o") == 0) { i++; if(i < argc) { snprintf(saida_custom, 1024, "%s", argv[i]); tem_saida_custom = 1; } }
    }

    if (cerq_erro || (n_layers > 0 && !frases)) {
        for (int j = 0; j < n_layers && frases && frases[j]; j++) free(frases[j]);
        free(frases);
        printf("\n[❌ ERRO] Frase(s) de chave inválida(s). Envolva cada uma entre #...# — ex.: -k 2 \"#ab#\" \"#cd#\"\n");
        return;
    }

    // Protocolo das chaves na descriptografia:
    //  -legado             → sempre legado (v6).
    //  -m entrada .martelos: o cabeçalho decide (MART = legado, MR7T = salto).
    //  mídia cru (bmp/wav/avi glitch) da v6 sem marcador: use -legado.
    int legado_midia = legado;
    if (alvo == 5 && acao == 2 && !legado) {
        // .martelos: o cabeçalho decide sozinho. Sem cabeçalho (proto 0) tenta
        // o Salto; para um v6 cru sem marcador, o usuário passa -legado.
        int proto = protocolo_martelos_do_arquivo(entrada);
        legado_midia = (proto == 2) ? 1 : 0;
    }
    if (acao == 1) legado_midia = 0; // quem cifra, cifra no protocolo novo

    // Casas (a origem do número depende do protocolo):
    if (n_layers > 0 && frases) {
        casas = malloc((size_t)n_layers * sizeof(mpz_t));
        for (int j = 0; j < n_layers; j++) mpz_init(casas[j]);
        int derivada_legado = (alvo == 1) ? legado : legado_midia;
        for (int j = 0; j < n_layers; j++) {
            int prof = j; // camada c lê a frase com profundidade c (quiasmo vivo)
            if (derivada_legado) { int d; long soma = calcular_casa_quiasmo(frases[j], &d); mpz_set_ui(casas[j], (unsigned long)soma); }
            else calcular_casa_salto(frases[j], prof, casas[j]);
            free(frases[j]);
        }
        free(frases); frases = NULL;
    }

    // CORREÇÃO v6.3.4: descriptografia de mídia via CLI (-i/-a/-v -d) não deve
    // re-aplicar "_glitch" (o caminho interativo usa *_fix_*; os fatos falhavam
    // quando o acao era processado depois do alvo).
    if (alvo >= 2 && alvo <= 4 && acao == 2) {
        const char* extm = (alvo == 2) ? ".bmp" : (alvo == 3) ? ".wav" : ".avi";
        snprintf(fop, 1024, "%s_fix%s", entrada, extm);
    }

    if (tem_saida_custom) snprintf(fop, 1024, "%s", saida_custom); // vale em qualquer ordem das flags
    if (alvo == 5 && acao == 2 && !tem_saida_custom) {
        // Descriptografia .martelos: usa a extensão guardada no cabeçalho
        // (ex: teste.martelos -> teste.avi). Sem cabeçalho, mantém o nome legado.
        char ext_auto[MART_MAXEXT + 1];
        if (ler_ext_martelos_arquivo(entrada, ext_auto)) {
            char base_cli[1024]; strcpy(base_cli, entrada);
            size_t le = strlen(base_cli);
            if (le > 9 && strcmp(base_cli + le - 9, ".martelos") == 0) base_cli[le - 9] = '\0';
            size_t lbx = strlen(base_cli), lx = strlen(ext_auto);
            if (lbx >= lx && strcmp(base_cli + lbx - lx, ext_auto) == 0) snprintf(fop, 1024, "%s", base_cli);
            else snprintf(fop, 1024, "%s%s", base_cli, ext_auto);
        }
    }
    if (alvo == 1 && casas && acao > 0) {
        int selo_st = 0;
        char* res = (legado)
            ? ((acao == 1)
                ? arma_legado_cifrar(entrada, casas, n_layers, tipo_const, formula)
                : arma_legado_decifrar(entrada, casas, n_layers, tipo_const, formula))
            : ((acao == 1)
                ? cifrar_salto(entrada, casas, n_layers, tipo_const, formula, &selo_st)
                : decifrar_salto(entrada, casas, n_layers, tipo_const, formula, &selo_st));
        printf("\n--- RESULTADO CLI ---\n%s\n", res);
        if (acao == 2 && !legado) {
            if (selo_st == 1) printf("[🔐] Selo verificado — frase íntegra e senha certa.\n");
            else printf("[🕰] Sem selo na entrada — texto da v6 (use -legado) ou cópia mutilada; confira o resultado.\n");
        }
        if (salvar) salvar_em_arquivo_v2("cli_output.txt", res, 1);
        free(res);
    } else if (alvo >= 2 && acao > 0 && casas) {
        int sub_modo_cli = (alvo == 5) ? 1 : alvo; // 2=img 3=audio 4=video 5=generico(1)
        executar_transmutacao_cli(entrada, fop, acao, sub_modo_cli, tipo_const, formula, casas, n_layers);
    } else {
        printf("\n[❌ ERRO] Parametros invalidos. Consulte: ./martelos --help\n");
    }
    if (casas) liberar_casas(casas, n_layers);
}

// ============================================================================
// Legacy de texto via casas mpz (conveniente para o CLI -legado): deriva a
// soma v6 para cada camada e chama a cifra/decifra legada intocada.
// ============================================================================
static long* casas_para_legado(mpz_t* casas, int n) {
    long* cs = malloc((size_t)n * sizeof(long));
    for (int i = 0; i < n; i++) cs[i] = mpz_get_ui(casas[i]);
    return cs;
}

static char* arma_legado_cifrar(const char* t, mpz_t* casas, int n, int tipo, const char* f) {
    long* cs = casas_para_legado(casas, n);
    char* r = cifrar_texto_n_camadas(t, cs, n, tipo, f);
    free(cs);
    return r;
}

static char* arma_legado_decifrar(const char* t, mpz_t* casas, int n, int tipo, const char* f) {
    long* cs = casas_para_legado(casas, n);
    char* r = decifrar_texto_n_camadas(t, cs, n, tipo, f);
    free(cs);
    return r;
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
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); char* la = fgets(op, 10, stdin); if (!la) break;
            int a = atoi(la); if (a == 0) continue;
            char* txt = ler_texto_ctrl_d("ENTRADA:"); if(strcmp(txt,"0")==0){ free(txt); continue; }
            printf("Camadas? "); int nl = 0; IGNORE_RET(scanf("%d", &nl)); lb(); if(nl<=0) nl=1;
            int legado_txt = 0;
            if (a == 2) {
                printf("Protocolo: 1-Salto Quântico (v7) | 2-Legado (v6) [Enter=1]: ");
                char pr[10]; if (fgets(pr, 10, stdin)) legado_txt = (atoi(pr) == 2);
                lb();
            }
            char* res = NULL; int selo_st = 0;
            if (legado_txt) {
                long* cs = malloc((size_t)nl * sizeof(long));
                for(int i=0; i<nl; i++){ char m[50]; sprintf(m,"CHAVE %d:",i+1); char* fr=ler_texto_ctrl_d(m); int d; cs[i]=calcular_casa_quiasmo(fr,&d); free(fr); }
                res = (a==1)?cifrar_texto_n_camadas(txt,cs,nl,t,f):decifrar_texto_n_camadas(txt,cs,nl,t,f);
                free(cs);
                printf("\n--- RESULTADO (legado v6) ---\n%s\n", res);
            } else {
                mpz_t* casas = malloc((size_t)nl * sizeof(mpz_t));
                for(int z=0; z<nl; z++) mpz_init(casas[z]);
                for(int i=0; i<nl; i++){ char m[50]; sprintf(m,"CHAVE %d:",i+1); char* fr=ler_texto_ctrl_d(m); calcular_casa_salto(fr, i, casas[i]); free(fr); }
                res = (a==1)?cifrar_salto(txt,casas,nl,t,f,&selo_st):decifrar_salto(txt,casas,nl,t,f,&selo_st);
                liberar_casas(casas, nl);
                printf("\n--- RESULTADO ---\n%s\n", res);
                if (a == 2) {
                    if (selo_st == 1) printf("[🔐] Selo verificado — frase íntegra e senha certa.\n");
                    else printf("[🕰] Sem selo na entrada — v6 (re-escolha 'Legado') ou cópia mutilada; confira o resultado.\n");
                }
            }
            printf("Salvar (s/n)? "); char so; if(scanf(" %c", &so)==1 && (so=='s'||so=='S')){ char nm[100]; printf("Nome: "); IGNORE_RET(scanf("%99s",nm)); salvar_em_arquivo(nm,res); }
            free(txt); free(res); lb();
        } else if (p == 2) {
            printf("\n--- MODO MULTIMIDIA ---\n1-Martelos | 2-Imagens | 3-Audios | 4-Videos | 0-Voltar: "); char* ls = fgets(op, 10, stdin); if (!ls) break;
            int sub = atoi(ls); if (sub == 0) continue;
            printf("1-Cripto | 2-Decripto | 0-Voltar: "); char* la = fgets(op, 10, stdin); if (!la) break;
            int a = atoi(la); if (a == 0) continue;
            int t; char f[256]; if (!menu_constantes(&t, f)) continue;
            int legado_media = 0, auto_map = 0;
            if (a == 2) {
                printf("Protocolo das chaves: 1-Salto Quântico (v7) | 2-Legado (v6) [Enter=1]: ");
                char pr[10]; if (fgets(pr, 10, stdin)) legado_media = (atoi(pr) == 2);
                lb();
                if (sub == 1 && legado_media == 0) auto_map = 1; // .martelos detecta sozinho pelo cabeçalho
            }
            processar_arquivo_binario_n_camadas(t, f, a, sub, auto_map ? -1 : legado_media);
        }
    } return 0;
}

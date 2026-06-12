#include "martelos.h"
#include <dirent.h>

#define CHUNK_SIZE 65536 

void listar_arquivos_entrada(char* caminho) {
    DIR *d; struct dirent *dir; d = opendir(caminho);
    printf("\n--- ARQUIVOS DISPONÍVEIS EM /entrada ---\n");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG) printf("- %s\n", dir->d_name);
        }
        closedir(d);
    }
    printf("---------------------------------------\n");
}

void processar_arquivo_binario(int tipo_const, const char* formula, int acao, int modo_glitch) {
    char nome_in[256], nome_out[256], path_in[512], path_out[512];
    const char* home = getenv("HOME");

    #ifdef __ANDROID__
        snprintf(path_in, sizeof(path_in), "%s/storage/downloads/martelos/entrada/", home);
        snprintf(path_out, sizeof(path_out), "%s/storage/downloads/martelos/saida/", home);
    #else
        snprintf(path_in, sizeof(path_in), "%s/Downloads/martelos/entrada/", home);
        snprintf(path_out, sizeof(path_out), "%s/Downloads/martelos/saida/", home);
    #endif

    listar_arquivos_entrada(path_in);
    printf("Arquivo de ENTRADA: "); scanf("%255s", nome_in); getchar();
    printf("Arquivo de SAÍDA: "); scanf("%255s", nome_out); getchar();

    char full_in[1024], full_out[1024];
    snprintf(full_in, sizeof(full_in), "%s%s", path_in, nome_in);
    snprintf(full_out, sizeof(full_out), "%s%s", path_out, nome_out);

    char* f1 = ler_texto_ctrl_d("\n[1/3] FRASE 1 (Rounds A):");
    char* f2 = ler_texto_ctrl_d("[2/3] FRASE 2 (Rounds B):");
    char* f3 = ler_texto_ctrl_d("[3/3] FRASE 3 (Âncora Quântica):");

    int n1, n2, n3;
    long casa1 = calcular_casa_quiasmo(f1, &n1);
    long casa2 = calcular_casa_quiasmo(f2, &n2);
    long salto = calcular_casa_quiasmo(f3, &n3);

    char* esteira = obter_esteira_numerica(tipo_const, formula, 1000000);
    long tam_est = strlen(esteira);

    printf("\n[⚙️ ] Processando %s... ", (modo_glitch == 2 ? "em MODO GLITCH" : "em MODO TOTAL"));
    fflush(stdout);
    clock_t inicio = clock();

    FILE *f_in = fopen(full_in, "rb");
    FILE *f_out = fopen(full_out, "wb");
    if (!f_in || !f_out) { printf("[!] Erro nos arquivos!\n"); return; }

    // Medição de tamanho para segurança do header
    fseek(f_in, 0, SEEK_END);
    long tamanho_total = ftell(f_in);
    rewind(f_in);

    unsigned char *buffer = malloc(CHUNK_SIZE);
    
    // Configura o pulo: 10KB para Glitch, 0 para Total. 
    // Proteção: Se o arquivo for pequeno, o pulo é apenas 10% do tamanho.
    long inicio_proc = (modo_glitch == 2) ? 10240 : 0; 
    if (modo_glitch == 2 && tamanho_total <= inicio_proc) inicio_proc = tamanho_total / 10;

    if (inicio_proc > 0) {
        unsigned char *h_buffer = malloc(inicio_proc);
        size_t h_lidos = fread(h_buffer, 1, inicio_proc, f_in);
        if (h_lidos > 0) fwrite(h_buffer, 1, h_lidos, f_out);
        free(h_buffer);
    }

    long ptr_esteira = salto;
    size_t lidos;
    while ((lidos = fread(buffer, 1, CHUNK_SIZE, f_in)) > 0) {
        for (size_t i = 0; i < lidos; i++) {
            // ENTROPIA MÁXIMA (v2.5): Pega 3 dígitos da esteira (000-999) mod 256
            int p1 = ptr_esteira % tam_est;
            int p2 = (ptr_esteira + 1) % tam_est;
            int p3 = (ptr_esteira + 2) % tam_est;
            
            int shift = ((esteira[p1] - '0') * 100 + (esteira[p2] - '0') * 10 + (esteira[p3] - '0')) % 256;

            unsigned char byte_pulo;
            if (acao == 1) { 
                byte_pulo = buffer[i]; 
                buffer[i] = (unsigned char)((buffer[i] + shift) % 256);
            } else { 
                buffer[i] = (unsigned char)((buffer[i] - shift + 256) % 256);
                byte_pulo = buffer[i]; 
            }
            ptr_esteira += (byte_pulo == 0) ? 1 : byte_pulo;
        }
        fwrite(buffer, 1, lidos, f_out);
    }

    fclose(f_in); fclose(f_out); free(buffer);
    free(f1); free(f2); free(f3);
    printf("OK!\n✅ Finalizado em %.4fs\n", (double)(clock() - inicio) / CLOCKS_PER_SEC);
}

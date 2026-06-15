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

void processar_arquivo_binario_n_camadas(int tipo_const, const char* formula, int acao, int modo_glitch) {
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

    // --- NOVA LÓGICA DE N FRASES ---
    printf("\nQuantas camadas de blindagem (frases)? ");
    char op_n[10]; fgets(op_n, sizeof(op_n), stdin);
    int n_frases = atoi(op_n); if (n_frases <= 0) n_frases = 1;

    long* casas = malloc(n_frases * sizeof(long));
    for (int i = 0; i < n_frases; i++) {
        char msg[50]; sprintf(msg, "FRASE %d/%d:", i+1, n_frases);
        char* f = ler_texto_ctrl_d(msg);
        int dummy; casas[i] = calcular_casa_quiasmo(f, &dummy);
        free(f);
    }

    char* esteira = obter_esteira_numerica(tipo_const, formula, 1000000);
    long tam_est = strlen(esteira);

    printf("\n[⚙️ ] Martelando em %d camadas... ", n_frases);
    fflush(stdout);
    clock_t inicio = clock();

    FILE *f_in = fopen(full_in, "rb");
    FILE *f_out = fopen(full_out, "wb");
    if (!f_in || !f_out) { printf("[!] Erro nos arquivos!\n"); return; }

    fseek(f_in, 0, SEEK_END);
    long tamanho_total = ftell(f_in);
    rewind(f_in);

    unsigned char *buffer = malloc(CHUNK_SIZE);
    long inicio_proc = (modo_glitch == 2) ? 10240 : 0; 
    if (modo_glitch == 2 && tamanho_total <= inicio_proc) inicio_proc = tamanho_total / 10;

    if (inicio_proc > 0) {
        unsigned char *h_buffer = malloc(inicio_proc);
        size_t h_lidos = fread(h_buffer, 1, inicio_proc, f_in);
        if (h_lidos > 0) fwrite(h_buffer, 1, h_lidos, f_out);
        free(h_buffer);
    }

    // A Frase 1 (casas[0]) é o nosso Salto Quântico (Âncora)
    long ptr_esteira = casas[0];
    size_t lidos;

    while ((lidos = fread(buffer, 1, CHUNK_SIZE, f_in)) > 0) {
        for (size_t i = 0; i < lidos; i++) {
            unsigned char byte_pulo;

            if (acao == 1) { // --- CRIPTOGRAFAR ---
                byte_pulo = buffer[i]; // Guarda original
                for (int j = 0; j < n_frases; j++) {
                    // Cada camada usa um deslocamento baseado na sua respectiva frase
                    long p = (ptr_esteira + casas[j]) % tam_est;
                    int shift = ((esteira[p] - '0') * 100 + (esteira[(p+1)%tam_est] - '0') * 10 + (esteira[(p+2)%tam_est] - '0')) % 256;
                    buffer[i] = (unsigned char)((buffer[i] + shift) % 256);
                }
            } else { // --- DESCRIPTOGRAFAR ---
                // Ordem inversa das camadas para desfazer a cebola
                for (int j = n_frases - 1; j >= 0; j--) {
                    long p = (ptr_esteira + casas[j]) % tam_est;
                    int shift = ((esteira[p] - '0') * 100 + (esteira[(p+1)%tam_est] - '0') * 10 + (esteira[(p+2)%tam_est] - '0')) % 256;
                    buffer[i] = (unsigned char)((buffer[i] - shift + 256) % 256);
                }
                byte_pulo = buffer[i]; // Recupera original
            }
            
            // O ponteiro da esteira caminha uma única vez por byte, 
            // garantindo a estabilidade do código antigo.
            ptr_esteira += (byte_pulo == 0) ? 1 : byte_pulo;
        }
        fwrite(buffer, 1, lidos, f_out);
    }

    fclose(f_in); fclose(f_out); free(buffer); free(casas);
    printf("OK!\n✅ Finalizado em %.4fs\n", (double)(clock() - inicio) / CLOCKS_PER_SEC);
}

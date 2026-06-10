#include "martelos.h"

char* ler_texto_ctrl_d(const char* instrucao) {
    printf("%s", instrucao);
    printf("\n(Pressione Ctrl+D em uma linha vazia para finalizar)\n---\n");
    
    size_t capacidade = 2048;
    size_t tamanho = 0;
    char* buffer = malloc(capacidade);
    int c;

    while ((c = getchar()) != EOF) {
        buffer[tamanho++] = (char)c;
        if (tamanho >= capacidade - 1) {
            capacidade *= 2;
            buffer = realloc(buffer, capacidade);
        }
    }
    buffer[tamanho] = '\0';

    // REMOÇÃO DO "FANTASMA DO W": 
    // Se o último caractere for um Enter logo antes do EOF, nós o limpamos
    // para evitar que o motor tente descriptografar o "vazio".
    if (tamanho > 0 && buffer[tamanho-1] == '\n') {
        buffer[tamanho-1] = '\0';
    }

    clearerr(stdin);
    return buffer;
}

void salvar_em_arquivo(const char* nome_arquivo, const char* conteudo) {
    char caminho_final[512];
    const char* home = getenv("HOME");
    
    #ifdef __ANDROID__
        snprintf(caminho_final, sizeof(caminho_final), "%s/storage/downloads/%s", home, nome_arquivo);
    #else
        snprintf(caminho_final, sizeof(caminho_final), "%s/Downloads/%s", home, nome_arquivo);
    #endif

    FILE *f = fopen(caminho_final, "w");
    if (f == NULL) {
        f = fopen(nome_arquivo, "w");
        strcpy(caminho_final, nome_arquivo);
    }

    if (f) {
        fprintf(f, "%s", conteudo);
        fclose(f);
        printf("\n[💾] Sucesso! Arquivo salvo em: %s\n", caminho_final);
    } else {
        printf("\n[!] Erro ao criar arquivo.\n");
    }
}

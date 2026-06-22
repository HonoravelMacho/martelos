#include "martelos.h"
#include <sys/stat.h>
#include <sys/types.h>

char* ler_texto_ctrl_d(const char* instrucao) {
    printf("%s\n(Ctrl+D para finalizar ou '0' para cancelar)\n---\n", instrucao);
    size_t cap = 2048, tam = 0; char* buf = malloc(cap); int c;
    while ((c = getchar()) != EOF) { 
        buf[tam++] = (char)c; 
        if (tam >= cap - 1) { cap *= 2; buf = realloc(buf, cap); } 
    }
    buf[tam] = '\0';
    if (tam > 0 && buf[tam-1] == '\n') buf[tam-1] = '\0';
    clearerr(stdin);
    return buf;
}

// Agora aceita um booleano 'is_local' para decidir a jurisdição
void salvar_em_arquivo_v2(const char* nome, const char* cont, int is_local) {
    char full_path[2048];
    
    if (is_local) {
        // Modo CLI: Salva na pasta onde o terminal está aberto
        snprintf(full_path, 2048, "./%s", nome);
    } else {
        // Modo Menu: Salva na Bancada de Downloads
        const char* h = getenv("HOME");
        char dir_path[1024];
        snprintf(dir_path, 1024, "%s/Downloads/martelos/textos_salvos", h);
        mkdir(dir_path, 0755); // Garante que a pasta existe
        snprintf(full_path, 2048, "%s/%s", dir_path, nome);
    }
    
    FILE *f = fopen(full_path, "w");
    if (f) {
        fprintf(f, "%s", cont);
        fclose(f);
        printf("\n[💾] SOBERANIA ALGEBRICA! Salvo em: %s\n", full_path);
    } else {
        printf("\n[❌] Erro ao acessar o caminho: %s\n", full_path);
    }
}

// Mantendo compatibilidade com a assinatura antiga no .h
void salvar_em_arquivo(const char* nome, const char* cont) {
    salvar_em_arquivo_v2(nome, cont, 0); // Default para o menu
}

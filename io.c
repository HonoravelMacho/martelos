#include "martelos.h"
#include <sys/stat.h>
#include <sys/types.h>

char* ler_texto_ctrl_d(const char* instrucao) {
    printf("%s\n(Ctrl+D para finalizar ou digite '0' e Enter para cancelar)\n---\n", instrucao);
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

void salvar_em_arquivo(const char* nome, const char* cont) {
    char path[1024];
    const char* h = getenv("HOME");
    
    // 1. Define o caminho da pasta de textos
    snprintf(path, 1024, "%s/Downloads/martelos/textos_salvos", h);
    
    // 2. Cria a pasta se não existir (Permissão 0755)
    mkdir(path, 0755);
    
    // 3. Define o caminho completo do arquivo
    char full_path[2048];
    snprintf(full_path, 2048, "%s/%s", path, nome);
    
    FILE *f = fopen(full_path, "w");
    if (f) {
        fprintf(f, "%s", cont);
        fclose(f);
        printf("\n[💾] SOBERANIA GARANTIDA! Texto salvo em:\n--> %s\n", full_path);
    } else {
        printf("\n[❌] Erro ao salvar arquivo. Verifique as permissoes.\n");
    }
}

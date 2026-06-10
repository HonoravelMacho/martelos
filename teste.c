#include "martelos.h"

int main() {
    printf("Iniciando Teste de Cache...\n");
    char* p1 = obter_esteira_numerica(1, NULL, 1000);
    printf("Primeiros 10 dígitos: %.10s\n", p1);
    
    char* p2 = obter_esteira_numerica(1, NULL, 500); // Deve ser instantâneo
    printf("Teste Cache O(1): %.10s\n", p2);
    
    limpar_cache_esteira();
    return 0;
}

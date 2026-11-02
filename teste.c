#include "martelos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// CONTRATO EXECUTÁVEL v7.1.0 — SALTO QUÂNTICO + MALHA UTF-8
// Os vetores-ouro legados abaixo foram congelados do binário v6 real:
// se algum conferir mal, é o código que mudou, nunca o ouro.
// (Antes de cada chamada legada, limpar_cache_esteira() emula um processo
// fresco, como rodava a v6; as funções v7 já se limpam sozinhas.)
// ============================================================================

static int total = 0, falhas = 0;

static void checa(int cond, const char* nome) {
    total++;
    if (cond) printf("  ✓ %s\n", nome);
    else { falhas++; printf("  ✗ %s\n", nome); }
}

static void checa_str(const char* got, const char* want, const char* nome) {
    total++;
    if (got && strcmp(got, want) == 0) { printf("  ✓ %s\n", nome); return; }
    falhas++;
    printf("  ✗ %s\n      esperado: \"%s\"\n       obtido: \"%s\"\n", nome, want, got ? got : "(nulo)");
}

// casa v7 como string decimal (número literal, pode ser gigante)
static void casa_assert(const char* frase, int prof, const char* esperado, const char* nome) {
    mpz_t c; mpz_init(c);
    calcular_casa_salto(frase, prof, c);
    char* s = mpz_get_str(NULL, 10, c);
    total++;
    if (strcmp(s, esperado) == 0) printf("  ✓ %s\n", nome);
    else { falhas++; printf("  ✗ %s\n       obtido: %s\n      esperado: %s\n", nome, s, esperado); }
    free(s); mpz_clear(c);
}

static mpz_t* casas_de(const char* f1, const char* f2, const char* f3, int* nf) {
    *nf = f3 ? 3 : (f2 ? 2 : 1);
    mpz_t* cs = malloc((size_t)*nf * sizeof(mpz_t));
    const char* fs[3]; fs[0] = f1; fs[1] = f2; fs[2] = f3;
    for (int i = 0; i < *nf; i++) { mpz_init(cs[i]); calcular_casa_salto(fs[i], i, cs[i]); }
    return cs;
}

// O soneto de Camões — o caso REAL do Capitão no Termux (a v7.0.0 dispentava
// os acentos e engolia as quebras). Será a testemunha permanente da Malha UTF-8.
static const char* SONETO_CAPITAO =
    "Amor é fogo que arde sem se ver,\n"
    "é ferida que dói, e não se sente;\n"
    "é um contentamento descontente,\n"
    "é dor que desatina sem doer.\n"
    "\n"
    "É um não querer mais que bem querer;\n"
    "é um andar solitário entre a gente;\n"
    "é nunca contentar-se de contente;\n"
    "é um cuidar que ganha em se perder.\n"
    "\n"
    "É querer estar preso por vontade;\n"
    "é servir a quem vence, o vencedor;\n"
    "é ter com quem nos mata, lealdade.\n"
    "\n"
    "Mas como causar pode seu favor\n"
    "nos corações humanos amizade,\n"
    "se tão contrário a si é o mesmo Amor";

static void teste_roundtrip(const char* ent, mpz_t* casas, int n, const char* tag) {
    char* malha = malha_utf8_encode(ent);
    size_t mn = strlen(malha);
    char nome[128];

    if (mn == 0) { // texto vazio: nada a selar, volta vazio
        int selo = 0;
        char* cifra = cifrar_salto(ent, casas, n, 1, NULL, &selo);
        snprintf(nome, sizeof nome, "%s: vazio fica vazio (sem selo)", tag);
        checa(strlen(cifra) == 0 && selo == 0, nome);
        int selo2 = 0;
        char* plano = decifrar_salto(cifra, casas, n, 1, NULL, &selo2);
        snprintf(nome, sizeof nome, "%s: vazio volta vazio", tag);
        checa(strlen(plano) == 0, nome);
        free(plano); free(cifra); free(malha);
        return;
    }

    int selo = 0;
    char* cifra = cifrar_salto(ent, casas, n, 1, NULL, &selo);
    size_t mc = strlen(cifra);
    snprintf(nome, sizeof nome, "%s: selo marcado na cifra", tag);            checa(selo == 1, nome);
    snprintf(nome, sizeof nome, "%s: cifra tem tamanho malha+2 (%zu = %zu+2)", tag, mc, mn);
    checa(mc == mn + 2, nome);
    int sem_nl = 1; for (size_t i = 0; i < mc; i++) if (cifra[i] == '\n') sem_nl = 0;
    snprintf(nome, sizeof nome, "%s: cifra nunca contém \\n (cópia de terminal honesta)", tag);
    checa(sem_nl, nome);

    int selo2 = 0;
    char* plano = decifrar_salto(cifra, casas, n, 1, NULL, &selo2);
    snprintf(nome, sizeof nome, "%s: decifra confirma o selo", tag);          checa(selo2 == 1, nome);
    snprintf(nome, sizeof nome, "%s: roundtrip BYTE A BYTE (%zu bytes crua)", tag, strlen(ent));
    checa_str(plano, ent, nome); // a entrada CRUA — acentos e \\n de pé!

    free(plano); free(cifra); free(malha);
}

int main(void) {
    printf("====== CONTRATO v7.1.0 — SALTO QUÂNTICO + MALHA UTF-8 ======\n");

    // ------------------------------------------------------------------ 1
    printf("\n[1] Oceano (esteira)\n");
    {
        char* pi = obter_esteira_numerica(1, NULL, 500);
        checa(strncmp(pi, "3141592653", 10) == 0, "pi começa com 3141592653");
        checa(strncmp(obter_esteira_numerica(1, NULL, 100), pi, 10) == 0, "cache entrega esteira consistente");
        limpar_cache_esteira();
    }

    // ------------------------------------------------------------------ 2
    printf("\n[2] Fonte 95 (teclado completo)\n");
    checa(fonte95_para_num(' ') == 1 && fonte95_para_num('!') == 2 &&
          fonte95_para_num('0') == 17 && fonte95_para_num('9') == 26 &&
          fonte95_para_num('A') == 34 && fonte95_para_num('Z') == 59 &&
          fonte95_para_num('a') == 66 && fonte95_para_num('z') == 91 &&
          fonte95_para_num('~') == 95 && fonte95_para_num('\n') == -1,
          "espaço=1 … 0=17 … 9=26 … A=34 … z=91 … ~=95; \\n fora da fonte");
    checa(fonte95_num_para_char(1) == ' ' && fonte95_num_para_char(34) == 'A' &&
          fonte95_num_para_char(66) == 'a' && fonte95_num_para_char(95) == '~',
          "número volta para caractere");

    char* n = normalizar_fonte95("Árvore ção Índigo Ý Ã 1\t9\n");
    checa_str(n, "Arvore cao Indigo Y A 1 9 ", "acento translitera, caixa preservada, \\t/\\n viram espaço");
    free(n);

    // Malha UTF-8: quem está FORA da fonte viaja; quem está DENTRO fica.
    {
        char* m = malha_utf8_encode("dói\n");
        checa_str(m, "d=C3=B3i=0A", "malha: 'dói\\n' viaja como d=C3=B3i=0A");
        free(m);
        m = malha_utf8_encode("a=b");
        checa_str(m, "a=3Db", "malha: o próprio '=' escapa (=3D)");
        free(m);
        checa_str(malha_utf8_decode("a=b41c"), "a=b41c",
                  "malha canônica: '=b4' à mão (b minúsculo) NÃO é escape → literal");
        checa_str(malha_utf8_decode("a=41c"), "a=41c",
                  "malha canônica: '=41' à mão não fecha o ciclo → literal");
        checa_str(malha_utf8_decode("a=3Db"), "a=b",
                  "malha canônica: 'a=3Db' é escape legítimo → devolve 'a=b'");
        char* m2 = malha_utf8_encode("🚀 foco");
        char* b2 = malha_utf8_decode(m2);
        checa_str(b2, "🚀 foco", "malha: emoji atravessa o salto byte a byte");
        free(b2); free(m2);
    }

    // ------------------------------------------------------------------ 3
    printf("\n[3] Casa = número literal (base 95, ordem sagrada)\n");
    casa_assert("ab", 0, "6337", "casa(\"ab\") == 6337");
    casa_assert("ba", 0, "6431", "casa(\"ba\") == 6431 — AB ≠ BA");
    casa_assert("AB", 0, "3265", "casa(\"AB\") == 3265 (a caixa vale)");
    casa_assert("BA", 0, "3359", "casa(\"BA\") == 3359");
    {
        mpz_t ca, cb; mpz_init(ca); mpz_init(cb);
        calcular_casa_salto("ab", 0, ca);
        calcular_casa_salto("ba", 0, cb);
        checa(mpz_cmp(ca, cb) != 0, "\"ab\" e \"ba\" produzem números diferentes");
        calcular_casa_salto("ab", 1, ca);
        checa(mpz_cmp(ca, cb) == 0, "casa(\"ab\" espelhada 1×) == casa(\"ba\") — quiasmo de letras");

        calcular_casa_salto("artelos#", 0, ca);
        calcular_casa_salto("artelos", 0, cb);
        checa(mpz_cmp(ca, cb) != 0, "# agora é letra de verdade: \"artelos#\" ≠ \"artelos\"");
        calcular_casa_salto("martelos 2", 0, ca);
        calcular_casa_salto("martelos2", 0, cb);
        checa(mpz_cmp(ca, cb) != 0, "o espaço pesa: \"martelos 2\" ≠ \"martelos2\"");
        calcular_casa_salto("a b", 0, ca);
        calcular_casa_salto("ab", 0, cb);
        checa(mpz_cmp(ca, cb) != 0, "\"a b\" ≠ \"ab\" — a fonte conta o espaço");

        // Sem cap: casa de 19 letras tem exatamente 38 dígitos decimais
        calcular_casa_salto("ataque ao amanhecer", 0, ca);
        char* s = mpz_get_str(NULL, 10, ca);
        checa(strlen(s) == 38, "casa cresce sem cap (19 letras = 38 dígitos)");
        free(s);

        // Quiasmo vivo em palavras distintas (banana×3 colapsa — por isso 'olho nariz boca')
        calcular_casa_salto("olho nariz boca", 0, ca);
        calcular_casa_salto("olho nariz boca", 1, cb);
        checa(mpz_cmp(ca, cb) != 0, "quiasmo de palavras: leitura espelhada muda a casa");
        calcular_casa_salto("olho nariz boca", 2, cb);
        checa(mpz_cmp(ca, cb) == 0, "espelho do espelho (2×) volta ao original");
        calcular_casa_salto("ab", 2, cb);
        calcular_casa_salto("ab", 0, ca);
        checa(mpz_cmp(cb, ca) == 0, "letras espelhadas 2× também voltam ao original");
        mpz_clear(ca); mpz_clear(cb);
    }

    // ------------------------------------------------------------------ 4
    printf("\n[4] Roundtrips v7.1 byte a byte (3 camadas, pi)\n");
    {
        char* malha = malha_utf8_encode("Árvore boa! 10 metros, sem (%1) prova\n");
        printf("  (malha: \"%s\" — %zu símbolos, acentos e \\n de pé)\n", malha, strlen(malha));
        free(malha);
        int n; mpz_t* cs = casas_de("olho", "ab", "zebra pato", &n);
        teste_roundtrip("banana banana banana", cs, n, "banana");
        teste_roundtrip("Árvore boa! 10 metros, sem (%1) prova\n", cs, n, "ricas");
        teste_roundtrip("3141592653abc XYZ~", cs, n, "dígitos");
        teste_roundtrip("", cs, n, "vazio");
        teste_roundtrip(SONETO_CAPITAO, cs, n, "soneto do capitão");

        // selo-guarita na prática
        int selo = 0;
        char* cifra = cifrar_salto("banana banana banana", cs, n, 1, NULL, &selo);
        checa(strlen(cifra) == 22, "banana: cifra 20 + selo 2");
        char salvo = cifra[3]; cifra[3] = (cifra[3] == '+') ? '-' : '+';
        int selo3 = 0;
        char* quebrada = decifrar_salto(cifra, cs, n, 1, NULL, &selo3);
        checa(selo3 != 1, "cópia mutilada: selo da guarita não passa");
        free(quebrada);
        cifra[3] = salvo; // restaura a cópia honesta
        mpz_add_ui(cs[0], cs[0], 67); // senha errada (últimos símbolos mudam)
        int selo4 = 0;
        char* errada = decifrar_salto(cifra, cs, n, 1, NULL, &selo4);
        checa(selo4 != 1 && strcmp(errada, "banana banana banana") != 0, "senha errada: selo reprova e texto derrete");
        free(errada); free(cifra);
        liberar_casas(cs, n);
    }

    // ------------------------------------------------------------------ 5
    printf("\n[5] Ordem e camadas são sagradas (byte a byte)\n");
    {
        int selo = 0;
        int nA, nB, nC, nD, nE;
        mpz_t* csA = casas_de("ab", "cd", "ef", &nA);
        mpz_t* csB = casas_de("ba", "cd", "ef", &nB);
        mpz_t* csC = casas_de("cd", "ab", "ef", &nC);
        mpz_t* csD = casas_de("abc", "cba", NULL, &nD);
        mpz_t* csE = casas_de("abc", "abc", NULL, &nE);
        char* a = cifrar_salto("banana banana banana", csA, nA, 1, NULL, &selo);
        char* b = cifrar_salto("banana banana banana", csB, nB, 1, NULL, &selo);
        char* c = cifrar_salto("banana banana banana", csC, nC, 1, NULL, &selo);
        char* d = cifrar_salto("banana banana banana", csD, nD, 1, NULL, &selo);
        char* e = cifrar_salto("banana banana banana", csE, nE, 1, NULL, &selo);
        checa(strcmp(a, b) != 0 && strcmp(b, c) != 0 && strcmp(a, c) != 0,
              "frases trocadas de camada → cifra diferente");
        checa(strcmp(d, e) != 0, "\"cba\" espelhada ≠ \"abc\" espelhada → cifra diferente");
        free(a); free(b); free(c); free(d); free(e);
        liberar_casas(csA, nA); liberar_casas(csB, nB);
        liberar_casas(csC, nC); liberar_casas(csD, nD); liberar_casas(csE, nE);
    }

    // ------------------------------------------------------------------ 6
    printf("\n[6] Ouros LEGADOS (v6 congelado, byte a byte)\n");
    {
        // FRASES DESCASCADAS: é assim que a v6 dava a casa ao motor (o CLI
        // peleria as cerquilhas antes — "ab" puro vira a soma 3).
        int d;
        long casa_ab = calcular_casa_quiasmo("ab", &d);
        long casa_cd = calcular_casa_quiasmo("cd", &d);
        long casa_m1 = calcular_casa_quiasmo("martelos1", &d);
        long casa_sa = calcular_casa_quiasmo("senha acentos", &d);
        printf("  (casas legadas: ab=%ld, cd=%ld, martelos1=%ld, \"senha acentos\"=%ld)\n",
               casa_ab, casa_cd, casa_m1, casa_sa);
        checa(casa_ab == 3 && casa_cd == 7, "soma legada: ab=3 (A=1+B=2), cd=7 — \"ab\" e \"ba\" colapsam");
        checa(casa_ab == calcular_casa_quiasmo("ba", &d) && casa_cd == calcular_casa_quiasmo("dc", &d),
              "legado: nos pares A/B a ORDEM não importa (a mácula que a v7 cura)");

        limpar_cache_esteira();
        long cs1[1] = { casa_ab };
        char* r1 = cifrar_texto_n_camadas("banana banana banana", cs1, 1, 1, NULL);
        checa_str(r1, "\nTOJM,,LQJ.Z-DNJGZFQ", "ouro 1: #ab#, pi, 1 camada");
        free(r1);

        limpar_cache_esteira();
        long cs2[1] = { casa_m1 };
        char* r2 = cifrar_texto_n_camadas("banana banana banana", cs2, 1, 1, NULL);
        checa_str(r2, "-!JCDC PYEKAFLSQFXU\n", "ouro 2: #martelos1#, pi (o \\n final é letra)");
        free(r2);

        limpar_cache_esteira();
        long cs3[2] = { casa_ab, casa_cd };
        char* r3 = cifrar_texto_n_camadas("banana banana banana", cs3, 2, 1, NULL);
        checa_str(r3, "ROGDG?HRB??!N.R-BIY ", "ouro 3: 2 camadas, pi (espaço final é letra)");
        free(r3);

        limpar_cache_esteira();
        long cs4[1] = { casa_sa };
        char* r4 = cifrar_texto_n_camadas("José Árvores 10", cs4, 1, 1, NULL);
        checa_str(r4, "SP\n.EPBYKENXQNL", "ouro 4: acentos (v6: dígitos 10 → '--')");
        free(r4);

        limpar_cache_esteira();
        long cs5[1] = { casa_ab };
        char* r5 = cifrar_texto_n_camadas("banana banana banana", cs5, 1, 3, NULL);
        checa_str(r5, " GQTP.FV JE!C X\n?!N?", "ouro 5: catalan");
        free(r5);

        // Abertura legada (o caminho da recuperação)
        limpar_cache_esteira();
        char* ex1 = decifrar_texto_n_camadas("\nTOJM,,LQJ.Z-DNJGZFQ", cs1, 1, 1, NULL);
        checa_str(ex1, "BANANA BANANA BANANA", "decifra legado ouro 1");
        free(ex1);
        limpar_cache_esteira();
        char* ex3 = decifrar_texto_n_camadas("ROGDG?HRB??!N.R-BIY ", cs3, 2, 1, NULL);
        checa_str(ex3, "BANANA BANANA BANANA", "decifra legado ouro 3 (2 camadas)");
        free(ex3);
        limpar_cache_esteira();
        char* ex4 = decifrar_texto_n_camadas("SP\n.EPBYKENXQNL", cs4, 1, 1, NULL);
        checa_str(ex4, "JOSE ARVORES --", "decifra legado ouro 4");
        free(ex4);
        limpar_cache_esteira();
    }

    printf("\n=============== RESUMO ===============\n");
    printf("  %d verificações, %d falhas\n", total, falhas);
    if (falhas) printf("  ❌ CONTRATO QUEBRADO\n");
    else printf("  ✅ CONTRATO INTEIRO — o salto quântico está em pé\n");
    return falhas ? 1 : 0;
}

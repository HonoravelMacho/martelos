#include "martelos.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define IGNORE_RET(x) if(x){}

void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam);
    printf("\n--- PASTA ATUAL ---\n");
    struct dirent *dir;
    if (d) {
        int found = 0;
        // stat() em vez de d_type/DT_REG: dirent do MinGW (Windows) não tem
        // d_type — o .exe quebrava no link/compile.
        while ((dir = readdir(d))) {
            if (!strcmp(dir->d_name, ".") || !strcmp(dir->d_name, "..")) continue;
            char caminho_completo[2048];
            struct stat st;
            IGNORE_RET(snprintf(caminho_completo, sizeof caminho_completo, "%s/%s", cam, dir->d_name));
            if (stat(caminho_completo, &st) == 0 && S_ISREG(st.st_mode)) { printf("- %s\n", dir->d_name); found = 1; }
        }
        if (!found) printf("(Pasta vazia)\n");
        closedir(d);
    }
}

int is_image(const char* ext) {
    char* list[] = {".jpg", ".jpeg", ".png", ".webp", ".bmp", ".gif", ".tiff"};
    for(int i=0; i<7; i++) if(ext && strcasecmp(ext, list[i]) == 0) return 1;
    return 0;
}

int is_audio(const char* ext) {
    char* list[] = {".mp3", ".wav", ".flac", ".ogg", ".m4a", ".aac", ".wma"};
    for(int i=0; i<7; i++) if(ext && strcasecmp(ext, list[i]) == 0) return 1;
    return 0;
}

int is_video(const char* ext) {
    char* list[] = {".mp4", ".avi", ".mkv", ".mov", ".webm", ".flv", ".wmv", ".mpg", ".mpeg", ".m4v", ".3gp", ".ogv", ".ts", ".vob", ".asf"};
    for(int i=0; i<15; i++) if(ext && strcasecmp(ext, list[i]) == 0) return 1;
    return 0;
}

// --- MARTELOS: helpers exclusivos do caminho de ÁUDIO (não tocar imagem/vídeo/texto) ---
static const char* obter_extensao(const char* nome) {
    if (!nome) return NULL;
    const char* p = strrchr(nome, '.');
    return p;
}

// --- Compatibilidade ffmpeg/ffprobe: Linux (Pop_OS/Ubuntu) e Android/Termux ---
// No Termux o instalador antigo pedia o pacote inexistente "gmp" e escondia o
// erro real do ffmpeg com "> /dev/null 2>&1". As funções abaixo detectam
// ffmpeg/ffprobe ausentes, aceitam fallback sem ffprobe e SEMPRE mostram o
// log do ffmpeg quando a conversão falha.
static int tem_comando(const char* cmd) {
    char buf[256];
#ifdef _WIN32
    // Windows (MinGW): o system() vai para o cmd.exe — 'command -v' não existe.
    snprintf(buf, sizeof buf, "where %s > NUL 2>&1", cmd);
#else
    snprintf(buf, sizeof buf, "command -v %s > /dev/null 2>&1", cmd);
#endif
    return system(buf) == 0;
}

// --- CORREÇÃO v6.3.4 (PORTE/APK): resolve ffmpeg/ffprobe em PATH normal OU
// empacotado como lib<cmd>.so no MESMO diretório do binário do martelos
// (Android: nativeLibraryDir, o único ponto executável do APK). Isso permite
// os modos de mídia (-i/-a/-v) funcionarem dentro do app PORTE sem Termux.
static char FF_PATH[1104] = "";  // caminho resolvido do ffmpeg (com aspas)
static char FP_PATH[1104] = "";  // caminho resolvido do ffprobe (com aspas)

static int achar_prog(const char* nome, char* out, size_t n) {
    char buf[256];
    if (tem_comando(nome)) {
        snprintf(out, n, "%s", nome); // no PATH: usa o nome normal
        return 1;
    }
#ifdef _WIN32
    // Windows: sem /proc e sem chmod — só o PATH resolve o ffmpeg.
    return 0;
#else
    // Procura lib<nome>.so empacotado ao lado do próprio martelos
    char self[1024];
    ssize_t rl = readlink("/proc/self/exe", self, sizeof(self) - 1);
    if (rl > 0) {
        self[rl] = '\0';
        char* slash = strrchr(self, '/');
        if (slash) {
            *slash = '\0';
            char cand[1108];
            snprintf(cand, sizeof cand, "%s/lib%s.so", self, nome);
            if (access(cand, X_OK) == 0) {
                snprintf(out, n, "\"%s\"", cand);
                return 1;
            }
        }
    }
    return 0;
#endif
}

static int tem_ffmpeg(void) {
    return achar_prog("ffmpeg", FF_PATH, sizeof FF_PATH);
}

static int tem_ffprobe(void) {
    return achar_prog("ffprobe", FP_PATH, sizeof FP_PATH);
}

static void dica_ffmpeg_instalacao(void) {
    printf("    Dicas de reparo:\n");
    printf("    1) Confira o ffmpeg:  ffmpeg -version\n");
    printf("       Termux (Android): pkg update && pkg install -y ffmpeg libgmp\n");
    printf("       Linux (Pop_OS/Ubuntu): sudo apt update && sudo apt install -y ffmpeg libgmp-dev\n");
    printf("    2) No Termux, na 1a vez rode: termux-setup-storage\n");
    printf("    3) O arquivo precisa estar na pasta onde o terminal foi aberto (use cd antes de martelos).\n");
}

// Roda o ffmpeg guardando o stderr num log temporário. Em caso de falha,
// exibe as últimas linhas do log em vez do genérico "Erro FFmpeg".
// CORREÇÃO v6.3.3: o log NÃO pode ser fixo em /tmp — no Android/Termux o /tmp
// não existe ou não tem escrita ("Permission denied"), o que fazia o próprio
// shell falhar (código 256) antes mesmo do ffmpeg rodar. Agora o diretório
// vem de $TMPDIR (Termux: $PREFIX/tmp), com fallback para $PREFIX/tmp e /tmp,
// e com teste de escrita antes de usar.
// Retorna o código do system() (0 = ok).
static void caminho_log_ffmpeg(char* out, size_t n) {
    const char* td = getenv("TMPDIR");
    char base[1024];
    if (td && *td) snprintf(base, sizeof base, "%s", td);
    else {
        const char* px = getenv("PREFIX"); // Termux: /data/data/com.termux/files/usr
        if (px && *px) snprintf(base, sizeof base, "%s/tmp", px);
        else snprintf(base, sizeof base, "/tmp");
    }
    snprintf(out, n, "%s/martelos_ffmpeg_%d.log", base, (int)getpid());
}

static int rodar_ffmpeg(const char* cmd_sem_log, const char* rotulo) {
    char log[1100];
    caminho_log_ffmpeg(log, sizeof log);
    // Testa se dá para escrever o log; se não der, roda sem log em vez de
    // quebrar o shell com redirecionamento impossível.
    int tem_log = 0;
    {
        FILE* t = fopen(log, "w");
        if (t) { fclose(t); tem_log = 1; }
    }
    char full[8192];
    if (tem_log) snprintf(full, sizeof full, "%s > /dev/null 2> \"%s\"", cmd_sem_log, log);
    else snprintf(full, sizeof full, "%s > /dev/null 2>&1", cmd_sem_log);
    int rc = system(full);
    if (rc != 0) {
        printf("[❌] Erro FFmpeg (%s, código %d).\n", rotulo, rc);
        if (tem_log) {
            FILE* lf = fopen(log, "r");
            if (lf) {
                char linha[512]; int n = 0;
                printf("---- log do ffmpeg ----\n");
                while (fgets(linha, sizeof linha, lf) && n < 15) { printf("%s", linha); n++; }
                if (n == 0) printf("(log vazio)\n");
                printf("-----------------------\n");
                fclose(lf);
            } else {
                printf("(não foi possível ler o log em %s)\n", log);
            }
            unlink(log);
        } else {
            printf("(log do ffmpeg indisponível: sem escrita em %s)\n", log);
        }
        dica_ffmpeg_instalacao();
    } else if (tem_log) {
        unlink(log);
    }
    return rc;
}

// Validação de stream com fallback: se o ffprobe existir, valida o stream
// específico (a/v). Se não existir (Termux mínimo, PATH estranho), usa o
// próprio ffmpeg como sonda e libera a prensagem para tentar — o erro real
// aparece via rodar_ffmpeg(). Retorna 1 se liberado, 0 se bloqueado.
static int validar_stream(const char* fip, char tipo_stream, const char* rotulo) {
    if (!tem_ffmpeg()) {
        printf("[❌] ffmpeg não encontrado (PATH ou empacotado).\n");
        dica_ffmpeg_instalacao();
        return 0;
    }
    if (tem_ffprobe()) {
        char cmd[4096];
        snprintf(cmd, sizeof(cmd),
                 "%s -v error -select_streams %c -show_entries stream=codec_name "
                 "-of default=noprint_wrappers=1:nokey=1 \"%s\" > /dev/null 2>&1",
                 FP_PATH, tipo_stream, fip);
        if (system(cmd) != 0) {
            printf("[❌] Arquivo não contém %s válido ou o ffmpeg não consegue lê-lo: %s\n", rotulo, fip);
            dica_ffmpeg_instalacao();
            return 0;
        }
        return 1;
    }
    // Fallback sem ffprobe: o ffmpeg tenta abrir o arquivo.
    {
        char cmd[4096];
        snprintf(cmd, sizeof(cmd),
                 "%s -v error -i \"%s\" -f null - > /dev/null 2>&1", FF_PATH, fip);
        if (system(cmd) != 0) {
            printf("[⚠️] ffprobe ausente e o ffmpeg não conseguiu abrir: %s\n", fip);
            printf("    Instalando o ffprobe a validação fica completa (ele vem junto com o pacote ffmpeg).\n");
            dica_ffmpeg_instalacao();
            return 0;
        }
        printf("[⚠️] ffprobe não encontrado — validação de %s simplificada (só extensão + abertura).\n", rotulo);
        return 1;
    }
}

// Varre os chunks do RIFF/WAVE e devolve o offset onde começam as amostras PCM
// (ou seja, o tamanho do cabeçalho a preservar). Fallback: 44.
static long detectar_header_wav(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return 44;
    unsigned char hdr[12];
    if (fread(hdr, 1, 12, f) != 12) { fclose(f); return 44; }
    if (memcmp(hdr, "RIFF", 4) != 0 || memcmp(hdr + 8, "WAVE", 4) != 0) { fclose(f); return 44; }
    long off = 12;
    for (int iter = 0; iter < 64; iter++) {
        unsigned char ch[8];
        if (fseek(f, off, SEEK_SET) != 0) break;
        if (fread(ch, 1, 8, f) != 8) break;
        unsigned long sz = (unsigned long)ch[4] | ((unsigned long)ch[5] << 8) |
                           ((unsigned long)ch[6] << 16) | ((unsigned long)ch[7] << 24);
        if (memcmp(ch, "data", 4) == 0) { fclose(f); return off + 8; }
        if (sz > 100000000UL) break; // chunk absurdo = arquivo estranho, desiste
        off += 8 + (long)sz + (sz & 1);
        if (off > 10240) break; // cabeçalho WAV nunca é gigante
    }
    fclose(f);
    return 44;
}

// Retorna 1 se o arquivo pode ser tratado como áudio, 0 caso contrário (com mensagem).
static int validar_entrada_audio(const char* fip) {
    const char* ext = obter_extensao(fip);
    if (!is_audio(ext)) {
        printf("[❌] Formato de áudio não suportado ('%s'). Aceitos: .mp3 .wav .flac .ogg .m4a .aac .wma\n",
               ext ? ext : "(sem extensão)");
        printf("    Dica: escolha a opção correta no menu (1-Martelos 2-Imagens 3-Audios 4-Videos).\n");
        return 0;
    }
    // ffprobe confirma que existe ao menos um stream de áudio decodificável
    // (com fallback para ffmpeg puro quando o ffprobe não está instalado).
    return validar_stream(fip, 'a', "áudio");
}

// Retorna 1 se o arquivo pode ser tratado como vídeo, 0 caso contrário (com mensagem).
static int validar_entrada_video(const char* fip) {
    const char* ext = obter_extensao(fip);
    if (!is_video(ext)) {
        printf("[❌] Formato de vídeo não suportado ('%s'). Aceitos: .mp4 .avi .mkv .mov .webm .flv .wmv .mpg .mpeg .m4v .3gp .ogv .ts .vob .asf\n",
               ext ? ext : "(sem extensão)");
        printf("    Dica: escolha a opção correta no menu (1-Martelos 2-Imagens 3-Audios 4-Videos).\n");
        return 0;
    }
    // ffprobe confirma que existe ao menos um stream de vídeo decodificável
    // (com fallback para ffmpeg puro quando o ffprobe não está instalado).
    return validar_stream(fip, 'v', "vídeo");
}

// Retorna 1 se o arquivo pode ser tratado como imagem, 0 caso contrário (com mensagem).
static int validar_entrada_imagem(const char* fip) {
    const char* ext = obter_extensao(fip);
    if (!is_image(ext)) {
        printf("[❌] Formato de imagem não suportado ('%s'). Aceitos: .jpg .jpeg .png .webp .bmp .gif .tiff\n",
               ext ? ext : "(sem extensão)");
        printf("    Dica: escolha a opção correta no menu (1-Martelos 2-Imagens 3-Audios 4-Videos).\n");
        return 0;
    }
    // ffprobe confirma que o arquivo abre como imagem (imagem = 1 stream de "vídeo")
    // (com fallback para ffmpeg puro quando o ffprobe não está instalado).
    return validar_stream(fip, 'v', "imagem");
}

typedef struct { long ini, fim; } Fatia;

// Mapeia os payloads de frames de um AVI (LIST movi): devolve as faixas de
// bytes que PODEM ser criptografadas. Preserva hdrl, cabeçalho da LIST movi,
// cabeçalho de 8 bytes de cada chunk e o índice idx1 do final.
// Retorna o nº de fatias em *out (0 = falha → o chamador usa o caminho legado).
static int mapear_payloads_avi(const char* path, Fatia** out) {
    *out = NULL;
    FILE* f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long total = ftell(f); rewind(f);
    unsigned char h[12];
    if (total < 12 || fread(h, 1, 12, f) != 12 || memcmp(h, "RIFF", 4) != 0 || memcmp(h + 8, "AVI ", 4) != 0) { fclose(f); return 0; }
    long off = 12, movi_off = -1; unsigned long movi_sz = 0;
    for (int it = 0; it < 64; it++) {
        if (off + 12 > total) break;
        unsigned char ch[12];
        fseek(f, off, SEEK_SET);
        if (fread(ch, 1, 12, f) != 12) break;
        unsigned long sz = (unsigned long)ch[4] | ((unsigned long)ch[5] << 8) |
                           ((unsigned long)ch[6] << 16) | ((unsigned long)ch[7] << 24);
        if (sz > (unsigned long)(total - off)) break;
        if (memcmp(ch, "LIST", 4) == 0 && memcmp(ch + 8, "movi", 4) == 0) { movi_off = off; movi_sz = sz; break; }
        off += 8 + (long)sz + (sz & 1);
    }
    if (movi_off < 0) { fclose(f); return 0; }
    long cur = movi_off + 12, fim = movi_off + 8 + (long)movi_sz;
    if (fim > total) fim = total;
    int cap = 256, n = 0;
    Fatia* v = malloc(cap * sizeof(Fatia));
    if (!v) { fclose(f); return 0; }
    int ok = 1;
    while (cur + 8 <= fim) {
        unsigned char ch[8];
        fseek(f, cur, SEEK_SET);
        if (fread(ch, 1, 8, f) != 8) { ok = 0; break; }
        unsigned long sz = (unsigned long)ch[4] | ((unsigned long)ch[5] << 8) |
                           ((unsigned long)ch[6] << 16) | ((unsigned long)ch[7] << 24);
        long pay_ini = cur + 8, pay_fim = pay_ini + (long)sz;
        if (pay_fim > fim) { ok = 0; break; } // tamanho incoerente: desiste (fallback legado)
        if (sz > 0) {
            if (n >= cap) { cap *= 2; Fatia* nv = realloc(v, cap * sizeof(Fatia)); if (!nv) { ok = 0; break; } v = nv; }
            v[n].ini = pay_ini; v[n].fim = pay_fim; n++;
        }
        cur = pay_fim + (sz & 1);
    }
    fclose(f);
    if (!ok || n == 0) { free(v); return 0; }
    *out = v; return n;
}

// Desenha a barra "progresso" (tty: reescreve a linha; sem tty: 1 linha a cada 10%).
static void atualizar_barra(int eh_tty, double pct, long feito, long total, double decorrido,
                            long* last_pct4, clock_t* last_draw, clock_t agora, int* last_fatia) {
    long pct4 = (long)(pct * 10000.0);
    if (pct4 == *last_pct4 && (agora - *last_draw) <= CLOCKS_PER_SEC / 5) return;
    *last_pct4 = pct4; *last_draw = agora;
    int blocos = (int)(pct * 20.0 / 100.0); if (blocos > 20) blocos = 20;
    char barra[21];
    for (int b = 0; b < 20; b++) barra[b] = (b < blocos) ? '#' : '-';
    barra[20] = '\0';
    double restante = (feito > 0 && decorrido > 0) ? decorrido * (total - feito) / (double)feito : 0.0;
    char t_rest[16];
    if (restante >= 7200.0) snprintf(t_rest, sizeof t_rest, "~%.2fh", restante / 3600.0);
    else if (restante >= 120.0) snprintf(t_rest, sizeof t_rest, "~%.1fmin", restante / 60.0);
    else snprintf(t_rest, sizeof t_rest, "~%.0fs", restante);
    if (eh_tty) {
        printf("\r\033[Kprogresso %8.4f%% [%s] %.1f/%.1fMB %s",
               pct, barra, feito / 1048576.0, total / 1048576.0, t_rest);
        fflush(stdout);
    } else {
        int fatia = (int)(pct / 10.0);
        if (fatia != *last_fatia) {
            *last_fatia = fatia;
            printf("progresso %8.4f%% [%s] %.1f/%.1fMB %s\n",
                   pct, barra, feito / 1048576.0, total / 1048576.0, t_rest);
            fflush(stdout);
        }
    }
}

static int iniciar_barra(void) {
    int tty = isatty(fileno(stdout));
    if (tty) { printf("progresso   0.0000%% [--------------------]"); fflush(stdout); }
    else printf("progresso   0.0000%% [--------------------]\n");
    return tty;
}

// Verificador do cabeçalho .martelos (só metadados: detecta senha/constante
// diferente na descriptografia; não altera a criptografia em nada).
static unsigned long martelos_verificador(long* cs, int nf, int tipo_const, const char* formula) {
    unsigned long v = (unsigned long)tipo_const * 2654435761UL + 0x9E3779B9UL * (unsigned long)nf;
    for (int i = 0; i < nf; i++) v = v * 31 + (unsigned long)(cs[i] & 0xffffffffL);
    if (formula) for (const char* q = formula; *q; q++) v = v * 31 + (unsigned char)*q;
    return v & 0xffffffffUL; // cabeçalho guarda 32 bits: máscara aqui para a comparação bater
}

// Lê só a extensão guardada no cabeçalho .martelos (1 = achou). Arquivos
// antigos (sem cabeçalho) retornam 0 e usam o caminho legado.
int ler_ext_martelos_arquivo(const char* path, char* ext_out) {
    FILE* f = fopen(path, "rb");
    if (!f) return 0;
    fseek(f, 0, SEEK_END); long total = ftell(f); rewind(f);
    unsigned char cab[9];
    size_t lr = fread(cab, 1, 9, f);
    int ok = 0;
    if (lr == 9 && memcmp(cab, "MART", 4) == 0 && cab[8] <= MART_MAXEXT && cab[8] > 0 && 9L + cab[8] <= total) {
        if (fread((unsigned char*)ext_out, 1, cab[8], f) == cab[8]) { ext_out[cab[8]] = '\0'; ok = 1; }
    }
    fclose(f);
    return ok;
}

// O MOTOR DE PRENSAGEM v2.7 (O Chiado Original)
void processar_binario_core(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf) {
    char cmd[4096], f_tmp[2048];
    strcpy(f_tmp, fip);
    long h_size = 0, t_size_prot = 0;

    if (acao == 1) {
        // CORREÇÃO v6.3.4: o ffmpeg só é exigido nos modos de MÍDIA (2=img,
        // 3=audio, 4=video). O modo genérico (-m / sub_modo=1) não usa ffmpeg
        // — antes ele ganhava "[❌] ffmpeg não encontrado" e um exit 0 no
        // Android sem ffmpeg, o que fazia o PORTE/APIs reportarem "sucesso"
        // sem arquivo nenhum criado.
        if (sub_modo >= 2 && !tem_ffmpeg()) {
            printf("[❌] ffmpeg não encontrado no PATH — impossível prensar mídia.\n");
            dica_ffmpeg_instalacao();
            free(cs); return;
        }
        if (sub_modo == 2) { // Imagens -> BMP Raw
            if (!validar_entrada_imagem(fip)) { free(cs); return; }
            snprintf(f_tmp, 2048, "%s_prensa.bmp", fip); h_size = 54;
            // -vf format=bgr24 garante compatibilidade com ffmpeg 4/5/6/7/8 (Termux e desktop).
            sprintf(cmd, "%s -v error -i \"%s\" -y -vf format=bgr24 \"%s\"", FF_PATH, fip, f_tmp);
            if (rodar_ffmpeg(cmd, "imagem -> BMP") != 0) { free(cs); return; }
        } else if (sub_modo == 3) { // Áudio -> WAV PCM (header detectado, não fixo)
            if (!validar_entrada_audio(fip)) { free(cs); return; }
            snprintf(f_tmp, 2048, "%s_prensa.wav", fip);
            // -ac 2 explícito evita falha em builds mínimos (Termux) com entrada mono.
            sprintf(cmd, "%s -v error -i \"%s\" -y -vn -acodec pcm_s16le -ar 44100 -ac 2 \"%s\"", FF_PATH, fip, f_tmp);
            if (rodar_ffmpeg(cmd, "áudio -> WAV") != 0) { free(cs); return; }
            h_size = detectar_header_wav(f_tmp);
        } else if (sub_modo == 4) { // Vídeo -> AVI RAW (CHIADO CINZA)
            if (!validar_entrada_video(fip)) { free(cs); return; }
            snprintf(f_tmp, 2048, "%s_prensa.avi", fip); h_size = 10240; t_size_prot = 512000;
            // -vf format=bgr24 (em vez de -pix_fmt), no mesmo estilo da rota de imagem:
            // garante o chiado CINZA em ffmpeg 4..8, Termux e desktop.
            sprintf(cmd, "%s -v error -i \"%s\" -y -vf format=bgr24 -vcodec rawvideo -acodec pcm_s16le -f avi \"%s\"", FF_PATH, fip, f_tmp);
            if (rodar_ffmpeg(cmd, "vídeo -> AVI raw") != 0) { free(cs); return; }
        }
    } else {
        if (sub_modo == 2) h_size = 54;
        else if (sub_modo == 3) h_size = detectar_header_wav(f_tmp); // f_tmp == fip aqui
        else if (sub_modo == 4) { h_size = 10240; t_size_prot = 512000; }
    }

    FILE *fin = fopen(f_tmp, "rb"), *fout = fopen(fop, "wb");
    if (!fin || !fout) {
        if (!fin) {
            printf("[❌] Não encontrei o arquivo de entrada: %s\n", f_tmp);
            printf("    Dica: no modo por números, o arquivo precisa estar na pasta onde o terminal foi aberto.\n");
            if (fout) { fclose(fout); unlink(fop); } // não deixa .wav de 0 bytes para trás
        }
        else { fclose(fin); printf("[❌] Não consegui criar o arquivo de saída: %s\n", fop); }
        free(cs); return;
    }
    fseek(fin, 0, SEEK_END); long total_size = ftell(fin); rewind(fin);
    if (total_size <= 0) {
        printf("[❌] Arquivo de entrada vazio (0 bytes): %s\n", f_tmp);
        fclose(fin); fclose(fout); free(cs); return;
    }

    // Descriptografia: confere se a entrada parece um glitch gerado pelo martelos.
    // Evita "descriptografar" o arquivo ORIGINAL por engano e gerar lixo silencioso.
    if (acao == 2 && sub_modo >= 2) {
        unsigned char mg[12];
        size_t lm = fread(mg, 1, 12, fin); rewind(fin);
        int okm = 0;
        if (lm >= 12) {
            if (sub_modo == 2) okm = (mg[0] == 'B' && mg[1] == 'M');
            else if (sub_modo == 3) okm = (memcmp(mg, "RIFF", 4) == 0 && memcmp(mg + 8, "WAVE", 4) == 0);
            else if (sub_modo == 4) okm = (memcmp(mg, "RIFF", 4) == 0 && memcmp(mg + 8, "AVI ", 4) == 0);
        }
        if (!okm) {
            const char* esperado = (sub_modo == 2) ? ".bmp de chiado (*_glitch.bmp)"
                : (sub_modo == 3) ? ".wav de chiado (*_glitch.wav)" : ".avi de chiado (*_glitch.avi)";
            printf("[❌] '%s' não parece um glitch gerado pelo martelos.\n", f_tmp);
            printf("    Para descriptografar, escolha o arquivo %s — não o arquivo original.\n", esperado);
            fclose(fin); fclose(fout); unlink(fop); free(cs); return;
        }
    }

    char* est = obter_esteira_numerica(tipo_const, formula, 2000000);
    long tam_e = strlen(est), ptr = cs[0];
    unsigned char *buf = malloc(65536);
    clock_t t_i = clock();

    // Vídeo: mapeia os payloads de frames do AVI (preserva hdrl/cabeçalhos/idx1
    // e criptografa TODOS os frames, inclusive os últimos — sem cauda em claro).
    Fatia* fatias = NULL; int nfat = 0;
    if (sub_modo == 4) nfat = mapear_payloads_avi(f_tmp, &fatias);

    if (sub_modo == 1) {
        // .martelos com cabeçalho: "MART" + ver[4] + tam_ext[1] + ext + payload.
        // Arquivos antigos (sem cabeçalho) usam o modo legado: criptografa tudo.
        if (acao == 1) {
            const char* e = strrchr(fip, '.');
            const char* barra = strrchr(fip, '/');
            char ext_m[MART_MAXEXT + 1] = "";
            if (e && (!barra || e > barra) && strlen(e) <= MART_MAXEXT) strcpy(ext_m, e);
            unsigned long vv = martelos_verificador(cs, nf, tipo_const, formula);
            unsigned char cab[9];
            memcpy(cab, "MART", 4);
            cab[4] = (unsigned char)(vv & 0xFF); cab[5] = (unsigned char)((vv >> 8) & 0xFF);
            cab[6] = (unsigned char)((vv >> 16) & 0xFF); cab[7] = (unsigned char)((vv >> 24) & 0xFF);
            cab[8] = (unsigned char)strlen(ext_m);
            fwrite(cab, 1, 9, fout);
            if (ext_m[0]) fwrite(ext_m, 1, strlen(ext_m), fout);
            printf("[📦] Extensão '%s' guardada no .martelos (volta sozinha na descriptografia).\n",
                   ext_m[0] ? ext_m : "(nenhuma)");
        } else {
            unsigned char cab[9];
            size_t lr = fread(cab, 1, 9, fin);
            if (lr == 9 && memcmp(cab, "MART", 4) == 0 && cab[8] <= MART_MAXEXT && 9L + cab[8] <= total_size) {
                long base_pay = 9L + cab[8];
                if (cab[8] > 0) {
                    char tmp[MART_MAXEXT + 1];
                    if (fread(tmp, 1, cab[8], fin) != cab[8]) { rewind(fin); base_pay = 0; }
                }
                if (base_pay > 0) {
                    unsigned long vv = martelos_verificador(cs, nf, tipo_const, formula);
                    unsigned long vl = (unsigned long)cab[4] | ((unsigned long)cab[5] << 8) |
                                       ((unsigned long)cab[6] << 16) | ((unsigned long)cab[7] << 24);
                    if (vl != vv)
                        printf("[⚠️] Aviso: frases/constante parecem diferentes das usadas na criptografia.\n");
                } else rewind(fin);
            } else rewind(fin); // legado: sem cabeçalho, descriptografa tudo
        }
        long base_pay = ftell(fin);
        long total_pay = total_size - base_pay;
        int eh_tty = iniciar_barra();
        long last_pct4 = -1; clock_t last_draw = 0; int last_fatia = -1;
        while (ftell(fin) < total_size) {
            size_t want = (size_t)(total_size - ftell(fin)); if (want > 65536) want = 65536;
            size_t r = fread(buf, 1, want, fin);
            if (r == 0) break;
            for (size_t i = 0; i < r; i++) {
                unsigned char original = buf[i];
                for (int j = 0; j < nf; j++) {
                    long p = (ptr + cs[j]) % tam_e;
                    int s = (est[p]-'0')*100 + (est[(p+1)%tam_e]-'0')*10 + (est[(p+2)%tam_e]-'0');
                    if (acao == 1) buf[i] = (unsigned char)((buf[i] + (s % 256)) % 256);
                    else buf[i] = (unsigned char)((buf[i] - (s % 256) + 256) % 256);
                }
                if (acao == 1) ptr += (original == 0) ? 1 : original;
                else ptr += (buf[i] == 0) ? 1 : buf[i];
            }
            fwrite(buf, 1, r, fout);
            if (total_pay > 0) {
                long feito = ftell(fin) - base_pay;
                if (feito < 0) feito = 0; if (feito > total_pay) feito = total_pay;
                atualizar_barra(eh_tty, feito * 100.0 / (double)total_pay, feito, total_pay,
                                (double)(clock() - t_i) / CLOCKS_PER_SEC,
                                &last_pct4, &last_draw, clock(), &last_fatia);
            }
        }
        if (total_pay > 0) printf("\n");
    } else if (sub_modo == 4 && nfat > 0) {
        long total_cript = 0;
        for (int k = 0; k < nfat; k++) total_cript += fatias[k].fim - fatias[k].ini;
        long last_pct4 = -1;
        clock_t last_draw = 0;
        int eh_tty = isatty(fileno(stdout));
        int last_fatia = -1;
        if (eh_tty) { printf("progresso   0.0000%% [--------------------]"); fflush(stdout); }
        else printf("progresso   0.0000%% [--------------------]\n");
        long feito = 0, pos = 0; int k = 0;
        while (pos < total_size) {
            while (k < nfat && pos >= fatias[k].fim) k++;
            int em_pay = (k < nfat && pos >= fatias[k].ini);
            long limite = em_pay ? fatias[k].fim : (k < nfat ? fatias[k].ini : total_size);
            if (limite <= pos) { if (em_pay) k++; else break; continue; }
            size_t want = (size_t)(limite - pos); if (want > 65536) want = 65536;
            size_t r = fread(buf, 1, want, fin);
            if (r == 0) break;
            if (em_pay) {
                for (size_t i = 0; i < r; i++) {
                    unsigned char original = buf[i];
                    for (int j = 0; j < nf; j++) {
                        long p = (ptr + cs[j]) % tam_e;
                        int s = (est[p]-'0')*100 + (est[(p+1)%tam_e]-'0')*10 + (est[(p+2)%tam_e]-'0');
                        if (acao == 1) buf[i] = (unsigned char)((buf[i] + (s % 256)) % 256);
                        else buf[i] = (unsigned char)((buf[i] - (s % 256) + 256) % 256);
                    }
                    if (acao == 1) ptr += (original == 0) ? 1 : original;
                    else ptr += (buf[i] == 0) ? 1 : buf[i];
                }
                feito += (long)r;
                if (total_cript > 0) {
                    double pct = feito * 100.0 / (double)total_cript;
                    long pct4 = (long)(pct * 10000.0);
                    clock_t agora = clock();
                    double decorrido = (double)(agora - t_i) / CLOCKS_PER_SEC;
                    if (pct4 != last_pct4 || (agora - last_draw) > CLOCKS_PER_SEC / 5) {
                        last_pct4 = pct4; last_draw = agora;
                        int blocos = (int)(pct * 20.0 / 100.0);
                        if (blocos > 20) blocos = 20;
                        char barra[21];
                        for (int b = 0; b < 20; b++) barra[b] = (b < blocos) ? '#' : '-';
                        barra[20] = '\0';
                        double restante = (feito > 0 && decorrido > 0)
                            ? decorrido * (total_cript - feito) / (double)feito : 0.0;
                        char t_rest[16];
                        if (restante >= 7200.0) snprintf(t_rest, sizeof t_rest, "~%.2fh", restante / 3600.0);
                        else if (restante >= 120.0) snprintf(t_rest, sizeof t_rest, "~%.1fmin", restante / 60.0);
                        else snprintf(t_rest, sizeof t_rest, "~%.0fs", restante);
                        if (eh_tty) {
                            printf("\r\033[Kprogresso %8.4f%% [%s] %.1f/%.1fMB %s",
                                   pct, barra, feito / 1048576.0, total_cript / 1048576.0,
                                   t_rest);
                            fflush(stdout);
                        } else {
                            int fatia = (int)(pct / 10.0);
                            if (fatia != last_fatia) {
                                last_fatia = fatia;
                                printf("progresso %8.4f%% [%s] %.1f/%.1fMB %s\n",
                                       pct, barra, feito / 1048576.0, total_cript / 1048576.0,
                                       t_rest);
                                fflush(stdout);
                            }
                        }
                    }
                }
            }
            fwrite(buf, 1, r, fout);
            pos += (long)r;
        }
        if (total_cript > 0) printf("\n");
    } else {
    if (h_size > total_size) h_size = 0;
    if (h_size > 0) {
        unsigned char* h_buf = malloc(h_size);
        if(fread(h_buf, 1, h_size, fin) == (size_t)h_size) fwrite(h_buf, 1, h_size, fout);
        free(h_buf);
    }

    long body_limit = total_size - t_size_prot;
    if (body_limit <= h_size) body_limit = total_size;

    long total_body = body_limit - h_size;
    if (total_body < 0) total_body = 0;
    long last_pct4 = -1;
    clock_t last_draw = 0;
    // Barra horizontal em UMA linha via ANSI (zero dependências externas):
    // \r volta ao início + \033[K apaga a linha (evita "empilhar" na vertical).
    // Sem tty (saída redirecionada), imprime uma linha a cada 10% em vez de \r.
    int eh_tty = isatty(fileno(stdout));
    int last_fatia = -1;
    if (eh_tty) { printf("progresso   0.0000%% [--------------------]"); fflush(stdout); }
    else printf("progresso   0.0000%% [--------------------]\n");

    while (ftell(fin) < body_limit) {
        size_t to_read = (ftell(fin) + 65536 > body_limit) ? (body_limit - ftell(fin)) : 65536;
        size_t r = fread(buf, 1, to_read, fin);
        if (r <= 0) break;
        for (size_t i = 0; i < r; i++) {
            unsigned char original = buf[i];
            for (int j = 0; j < nf; j++) {
                long p = (ptr + cs[j]) % tam_e;
                int s = (est[p]-'0')*100 + (est[(p+1)%tam_e]-'0')*10 + (est[(p+2)%tam_e]-'0');
                if (acao == 1) buf[i] = (unsigned char)((buf[i] + (s % 256)) % 256);
                else buf[i] = (unsigned char)((buf[i] - (s % 256) + 256) % 256);
            }
            // O SALTO DINÂMICO DA v2.7 QUE GERA O CHIADO
            if (acao == 1) ptr += (original == 0) ? 1 : original;
            else ptr += (buf[i] == 0) ? 1 : buf[i];
        }
        fwrite(buf, 1, r, fout);

        if (total_body > 0) {
            long done = ftell(fin) - h_size;
            if (done < 0) done = 0;
            if (done > total_body) done = total_body;
            double pct = done * 100.0 / (double)total_body;
            long pct4 = (long)(pct * 10000.0);
            clock_t agora = clock();
            double decorrido = (double)(agora - t_i) / CLOCKS_PER_SEC;
            if (pct4 != last_pct4 || (agora - last_draw) > CLOCKS_PER_SEC / 5) {
                last_pct4 = pct4; last_draw = agora;
                int blocos = (int)(pct * 20.0 / 100.0);
                if (blocos > 20) blocos = 20;
                char barra[21];
                for (int b = 0; b < 20; b++) barra[b] = (b < blocos) ? '#' : '-';
                barra[20] = '\0';
                double restante = (done > 0 && decorrido > 0)
                    ? decorrido * (total_body - done) / (double)done : 0.0;
                // Tempo restante compacto: ~45s, ~3.2min ou ~2.50h (linha curta p/ caber no terminal)
                char t_rest[16];
                if (restante >= 7200.0) snprintf(t_rest, sizeof t_rest, "~%.2fh", restante / 3600.0);
                else if (restante >= 120.0) snprintf(t_rest, sizeof t_rest, "~%.1fmin", restante / 60.0);
                else snprintf(t_rest, sizeof t_rest, "~%.0fs", restante);
                if (eh_tty) {
                    printf("\r\033[Kprogresso %8.4f%% [%s] %.1f/%.1fMB %s",
                           pct, barra, done / 1048576.0, total_body / 1048576.0,
                           t_rest);
                    fflush(stdout);
                } else {
                    int fatia = (int)(pct / 10.0);
                    if (fatia != last_fatia) {
                        last_fatia = fatia;
                        printf("progresso %8.4f%% [%s] %.1f/%.1fMB %s\n",
                               pct, barra, done / 1048576.0, total_body / 1048576.0,
                               t_rest);
                        fflush(stdout);
                    }
                }
            }
        }
    }
    if (total_body > 0) printf("\n");

    if (ftell(fin) < total_size) {
        unsigned char* t_buf = malloc(65536); size_t tr;
        while((tr = fread(t_buf, 1, 65536, fin)) > 0) fwrite(t_buf, 1, tr, fout);
        free(t_buf);
    }
    } // fim do caminho legado (imagem/audio/generico/fallback de video)

    fclose(fin); fclose(fout);
    if (acao == 1 && sub_modo >= 2) unlink(f_tmp);
    free(buf); free(cs); free(fatias);
    printf(" OK! (%.4fs)\n[✅] Finalizado em: %s\n", (double)(clock()-t_i)/CLOCKS_PER_SEC, fop);
}

void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo) {
    char ni[256], no[300], fip[1024], fop[1024];

    // Opera na pasta onde o terminal foi aberto (nada de bancada fixa).
    listar_arquivos_entrada(".");
    printf("\nArquivo (na pasta atual): "); IGNORE_RET(scanf("%255s", ni)); if (strcmp(ni, "0") == 0) return;
    snprintf(fip, 1024, "%s", ni);

    char *ext_ptr = strrchr(ni, '.');
    char base[256]; strcpy(base, ni); if(ext_ptr) base[ext_ptr - ni] = '\0';

    if (acao == 1) {
        if (sub_modo == 1) sprintf(no, "%s.martelos", ni);
        else if (sub_modo == 2) sprintf(no, "%s_glitch.bmp", base);
        else if (sub_modo == 3) sprintf(no, "%s_glitch.wav", base);
        else sprintf(no, "%s_glitch.avi", base);
    } else {
        if (sub_modo == 1) {
            char ext_auto[MART_MAXEXT + 1];
            if (acao == 2 && ler_ext_martelos_arquivo(fip, ext_auto)) {
                sprintf(no, "%s_restaurado%s", base, ext_auto);
                printf("Extensão detectada no arquivo: %s (sem precisar digitar)\n", ext_auto);
            } else {
                printf("Extensao original (ex: .pdf): "); IGNORE_RET(scanf("%255s", no));
                char f_no[350]; sprintf(f_no, "%s_restaurado%s", base, no); strcpy(no, f_no);
            }
        }
        else if (sub_modo == 2) sprintf(no, "%s_fix.bmp", base);
        else if (sub_modo == 3) sprintf(no, "%s_fix.wav", base);
        else sprintf(no, "%s_fix.avi", base);
    }

    snprintf(fop, 1024, "%s", no);
    if (acao == 1) {
        // Nome personalizado do arquivo criptografado (Enter = mantém o padrão).
        const char* dext = (sub_modo == 1) ? ".martelos" : (sub_modo == 2) ? ".bmp" : (sub_modo == 3) ? ".wav" : ".avi";
        printf("Nome do arquivo criptografado [Enter = %s]: ", no);
        int c; while ((c = getchar()) != '\n' && c != EOF); // limpa o resto da linha anterior
        char custom[320];
        if (fgets(custom, sizeof custom, stdin)) {
            custom[strcspn(custom, "\n")] = 0;
            if (custom[0] != '\0') {
                if (strrchr(custom, '.') == NULL) snprintf(no, 300, "%s%s", custom, dext);
                else snprintf(no, 300, "%s", custom);
                snprintf(fop, 1024, "%s", no);
            }
        }
    }
    printf("Camadas? "); int nf; IGNORE_RET(scanf("%d", &nf)); if (nf <= 0) nf = 1; getchar();
    long* cs = malloc(nf * sizeof(long));
    for (int i=0; i<nf; i++) { 
        char m[50]; sprintf(m, "FRASE %d/%d:", i+1, nf); 
        char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); 
    }
    processar_binario_core(fip, fop, acao, sub_modo, tipo, f, cs, nf);
}

void executar_transmutacao_cli(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf) {
    // O core assume a posse do vetor e dá free ao final; aqui duplicamos para
    // que o chamador (processar_flags) continue dono do original e não haja double-free.
    long* copia = malloc(nf * sizeof(long));
    if (!copia) return;
    memcpy(copia, cs, nf * sizeof(long));
    processar_binario_core(fip, fop, acao, sub_modo, tipo_const, formula, copia, nf);
}

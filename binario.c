#include "martelos.h"
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#define IGNORE_RET(x) if(x){}

void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam);
    printf("\n--- BANCADA DE ENTRADA (%s) ---\n", cam);
    struct dirent *dir;
    if (d) {
        int found = 0;
        while ((dir = readdir(d))) if (dir->d_type == DT_REG) { printf("- %s\n", dir->d_name); found = 1; }
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

// --- MARTELOS: helpers exclusivos do caminho de ÁUDIO (não tocar imagem/vídeo/texto) ---
static const char* obter_extensao(const char* nome) {
    if (!nome) return NULL;
    const char* p = strrchr(nome, '.');
    return p;
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
        return 0;
    }
    // ffprobe confirma que existe ao menos um stream de áudio decodificável
    char cmd[4096];
    snprintf(cmd, sizeof(cmd),
             "ffprobe -v error -select_streams a -show_entries stream=codec_name "
             "-of default=noprint_wrappers=1:nokey=1 \"%s\" > /dev/null 2>&1", fip);
    if (system(cmd) != 0) {
        printf("[❌] Arquivo não contém áudio válido ou o ffmpeg não consegue lê-lo: %s\n", fip);
        return 0;
    }
    return 1;
}

// O MOTOR DE PRENSAGEM v2.7 (O Chiado Original)
void processar_binario_core(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf) {
    char cmd[4096], f_tmp[2048];
    strcpy(f_tmp, fip);
    long h_size = 0, t_size_prot = 0;

    if (acao == 1) {
        if (sub_modo == 2) { // Imagens -> BMP Raw
            snprintf(f_tmp, 2048, "%s_prensa.bmp", fip); h_size = 54;
            sprintf(cmd, "ffmpeg -i \"%s\" -y -pix_fmt bgr24 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("[❌] Erro FFmpeg.\n"); return; }
        } else if (sub_modo == 3) { // Áudio -> WAV PCM (header detectado, não fixo)
            if (!validar_entrada_audio(fip)) { free(cs); return; }
            snprintf(f_tmp, 2048, "%s_prensa.wav", fip);
            sprintf(cmd, "ffmpeg -i \"%s\" -y -vn -acodec pcm_s16le -ar 44100 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("[❌] Erro FFmpeg: não foi possível converter '%s' para WAV.\n", fip); free(cs); return; }
            h_size = detectar_header_wav(f_tmp);
        } else if (sub_modo == 4) { // Vídeo -> AVI RAW (CHIADO CINZA)
            snprintf(f_tmp, 2048, "%s_prensa.avi", fip); h_size = 10240; t_size_prot = 512000;
            sprintf(cmd, "ffmpeg -i \"%s\" -y -vcodec rawvideo -acodec pcm_s16le -f avi \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("[❌] Erro FFmpeg.\n"); return; }
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
            printf("    Dica: no modo por números o arquivo precisa estar em ~/Downloads/martelos/entrada/\n");
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

    if (h_size > total_size) h_size = 0;
    if (h_size > 0) {
        unsigned char* h_buf = malloc(h_size);
        if(fread(h_buf, 1, h_size, fin) == (size_t)h_size) fwrite(h_buf, 1, h_size, fout);
        free(h_buf);
    }

    char* est = obter_esteira_numerica(tipo_const, formula, 2000000);
    long tam_e = strlen(est), ptr = cs[0];
    unsigned char *buf = malloc(65536);
    long body_limit = total_size - t_size_prot;
    if (body_limit <= h_size) body_limit = total_size;

    clock_t t_i = clock();
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

    fclose(fin); fclose(fout);
    if (acao == 1 && sub_modo >= 2) unlink(f_tmp);
    free(buf); free(cs);
    printf(" OK! (%.4fs)\n[✅] Finalizado em: %s\n", (double)(clock()-t_i)/CLOCKS_PER_SEC, fop);
}

void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo) {
    char ni[256], no[300], fip[1024], fop[1024];
    const char* h = getenv("HOME");
    char pi[512], po[512];
    snprintf(pi, 512, "%s/Downloads/martelos/entrada/", h);
    snprintf(po, 512, "%s/Downloads/martelos/saida/", h);
    
    listar_arquivos_entrada(pi);
    printf("\nArquivo: "); IGNORE_RET(scanf("%255s", ni)); if (strcmp(ni, "0") == 0) return;
    snprintf(fip, 1024, "%s%s", pi, ni);

    char *ext_ptr = strrchr(ni, '.');
    char base[256]; strcpy(base, ni); if(ext_ptr) base[ext_ptr - ni] = '\0';

    if (acao == 1) {
        if (sub_modo == 1) sprintf(no, "%s.martelos", ni);
        else if (sub_modo == 2) sprintf(no, "%s_glitch.bmp", base);
        else if (sub_modo == 3) sprintf(no, "%s_glitch.wav", base);
        else sprintf(no, "%s_glitch.avi", base);
    } else {
        if (sub_modo == 1) { 
            printf("Extensao original (ex: .pdf): "); IGNORE_RET(scanf("%255s", no)); 
            char f_no[350]; sprintf(f_no, "%s_restaurado%s", base, no); strcpy(no, f_no); 
        }
        else if (sub_modo == 2) sprintf(no, "%s_fix.bmp", base);
        else if (sub_modo == 3) sprintf(no, "%s_fix.wav", base);
        else sprintf(no, "%s_fix.avi", base);
    }

    snprintf(fop, 1024, "%s%s", po, no);
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

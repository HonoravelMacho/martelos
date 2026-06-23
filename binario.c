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
        } else if (sub_modo == 3) { // Áudio -> WAV PCM
            snprintf(f_tmp, 2048, "%s_prensa.wav", fip); h_size = 44;
            sprintf(cmd, "ffmpeg -i \"%s\" -y -acodec pcm_s16le -ar 44100 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("[❌] Erro FFmpeg.\n"); return; }
        } else if (sub_modo == 4) { // Vídeo -> AVI RAW (CHIADO CINZA)
            snprintf(f_tmp, 2048, "%s_prensa.avi", fip); h_size = 10240; t_size_prot = 512000;
            sprintf(cmd, "ffmpeg -i \"%s\" -y -vcodec rawvideo -acodec pcm_s16le -f avi \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("[❌] Erro FFmpeg.\n"); return; }
        }
    } else {
        if (sub_modo == 2) h_size = 54;
        else if (sub_modo == 3) h_size = 44;
        else if (sub_modo == 4) { h_size = 10240; t_size_prot = 512000; }
    }

    FILE *fin = fopen(f_tmp, "rb"), *fout = fopen(fop, "wb");
    if (!fin || !fout) { if(fin) fclose(fin); free(cs); return; }
    fseek(fin, 0, SEEK_END); long total_size = ftell(fin); rewind(fin);

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
    printf("[⚙️] Prensando..."); fflush(stdout);

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
    }

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
    processar_binario_core(fip, fop, acao, sub_modo, tipo_const, formula, cs, nf);
}

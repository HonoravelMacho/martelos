#include "martelos.h"
#include <dirent.h>
#include <unistd.h>
#include <time.h>

// Silenciador de avisos de retorno para compilação profissional
#define IGNORE_RET(x) if(x){}

void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam);
    printf("\n--- BANCADA DE ENTRADA (%s) ---\n", cam);
    struct dirent *dir;
    if (d) {
        int found = 0;
        while ((dir = readdir(d))) {
            if (dir->d_type == DT_REG) { printf("- %s\n", dir->d_name); found = 1; }
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

// FUNÇÃO 1: MODO INTERATIVO (Downloads/martelos)
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo) {
    char ni[256], no[300], pi[512], po[512], fip[1024], fop[1024], cmd[4096], f_tmp[2048];
    const char* h = getenv("HOME");
    snprintf(pi, 512, "%s/Downloads/martelos/entrada/", h);
    snprintf(po, 512, "%s/Downloads/martelos/saida/", h);
    
    listar_arquivos_entrada(pi);
    printf("\nArquivo: "); IGNORE_RET(scanf("%255s", ni));
    if (strcmp(ni, "0") == 0) return;

    snprintf(fip, 1024, "%s%s", pi, ni);
    if (access(fip, F_OK) == -1) { printf("\n[❌] Erro: Arquivo nao encontrado.\n"); return; }

    char *ext = strrchr(ni, '.');
    char nome_base[256]; strcpy(nome_base, ni);
    if(ext) nome_base[ext - ni] = '\0';

    long h_size = 0;
    snprintf(f_tmp, 2048, "%s/%s_inter.tmp.mpg", pi, nome_base);

    if (sub_modo == 1) { 
        snprintf(no, 300, "%s.martelos", ni); h_size = 0; strcpy(f_tmp, fip);
    } 
    else if (sub_modo == 2) { 
        if (!ext || !is_image(ext)) { printf("\n[❌] Imagem nao suportada.\n"); return; }
        snprintf(no, 300, "%s_glitch.bmp", nome_base); h_size = 54;
        if (acao == 1) {
            sprintf(cmd, "ffmpeg -i \"%s\" -y -pix_fmt bgr24 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            IGNORE_RET(system(cmd));
        } else strcpy(f_tmp, fip);
    }
    else if (sub_modo == 3) { 
        if (!ext || !is_audio(ext)) { printf("\n[❌] Audio nao suportado.\n"); return; }
        snprintf(no, 300, "%s_glitch.wav", nome_base); h_size = 44;
        if (acao == 1) {
            sprintf(cmd, "ffmpeg -i \"%s\" -y -acodec pcm_s16le -ar 44100 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            IGNORE_RET(system(cmd));
        } else strcpy(f_tmp, fip);
    }
    else if (sub_modo == 4) { 
        printf("Extensao de saida (ex: .ts, .mpg): "); IGNORE_RET(scanf("%255s", no));
        char final_no[350]; sprintf(final_no, "%s_glitch%s", nome_base, no); strcpy(no, final_no);
        h_size = 131072;
        if (acao == 1) {
            sprintf(cmd, "ffmpeg -y -f lavfi -i color=c=black:s=1280x720:r=30:d=5 -i \"%s\" -f lavfi -i color=c=black:s=1280x720:r=30:d=5 "
                         "-filter_complex \"[1:v]scale=1280:720,format=yuv420p[v1]; [0:v]format=yuv420p[v0]; [2:v]format=yuv420p[v2]; "
                         "[v0][v1][v2]concat=n=3:v=1:a=0[out]\" -map \"[out]\" -an -vcodec mpeg2video -b:v 2M -f mpeg \"%s\" > /dev/null 2>&1", fip, f_tmp);
            IGNORE_RET(system(cmd));
        } else strcpy(f_tmp, fip);
    }

    snprintf(fop, 1024, "%s%s", po, no);
    printf("Camadas? "); int nf; IGNORE_RET(scanf("%d", &nf)); if (nf <= 0) nf = 1; getchar();
    long* cs = malloc(nf * sizeof(long));
    for (int i=0; i<nf; i++) { char m[50]; sprintf(m, "FRASE %d/%d:", i+1, nf); char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); }

    FILE *fin = fopen(f_tmp, "rb"), *fout = fopen(fop, "wb");
    if (!fin || !fout) { if(fin) fclose(fin); free(cs); return; }
    fseek(fin, 0, SEEK_END); long t_size = ftell(fin); rewind(fin);

    if (h_size > t_size / 2) h_size = t_size / 10;
    if (h_size > 0) {
        unsigned char* h_buf = malloc(h_size);
        if(fread(h_buf, 1, h_size, fin) == (size_t)h_size) fwrite(h_buf, 1, h_size, fout);
        free(h_buf);
    }

    char* est = obter_esteira_numerica(tipo, f, 2000000);
    long tam_e = strlen(est), ptr = cs[0];
    unsigned char *buf = malloc(65536);

    printf("[⚙️] Prensando..."); fflush(stdout); clock_t t_i = clock();
    while (!feof(fin)) {
        size_t r = fread(buf, 1, 65536, fin); if (r <= 0) break;
        for (size_t i = 0; i < r; i++) {
            unsigned char b_o = buf[i];
            for (int j = 0; j < nf; j++) {
                long p = (ptr + cs[j]) % tam_e;
                int s = (est[p]-'0')*100 + (est[(p+1)%tam_e]-'0')*10 + (est[(p+2)%tam_e]-'0');
                if (acao == 1) buf[i] = (unsigned char)((buf[i] + (s % 256)) % 256);
                else buf[i] = (unsigned char)((buf[i] - (s % 256) + 256) % 256);
            }
            if (acao == 1) ptr += (b_o == 0) ? 1 : b_o; else ptr += (buf[i] == 0) ? 1 : buf[i];
        }
        fwrite(buf, 1, r, fout);
    }
    fclose(fin); fclose(fout);
    // CORREÇÃO: modo substituído por sub_modo para saber se houve FFmpeg
    if (sub_modo >= 2 && acao == 1) unlink(f_tmp);
    free(buf); free(cs);
    printf(" OK! (%.4fs)\n[✅] Salvo: %s\n", (double)(clock()-t_i)/CLOCKS_PER_SEC, no);
}

// FUNÇÃO 2: MODO CLI (Local ./)
void executar_transmutacao_cli(const char* fip, const char* fop, int acao, int sub_modo, int tipo_const, const char* formula, long* cs, int nf) {
    if (access(fip, F_OK) == -1) { printf("\n[❌ CLI] Arquivo '%s' nao encontrado.\n", fip); return; }
    char cmd[4096], f_tmp[2048]; strcpy(f_tmp, fip);
    long h_size = (sub_modo == 2) ? 54 : (sub_modo == 3) ? 44 : (sub_modo == 4) ? 131072 : 0;

    if (acao == 1) {
        snprintf(f_tmp, 2048, "./.temp_cli_martelos.tmp");
        if (sub_modo == 2) sprintf(cmd, "ffmpeg -i \"%s\" -y -pix_fmt bgr24 \"%s\" > /dev/null 2>&1", fip, f_tmp);
        else if (sub_modo == 3) sprintf(cmd, "ffmpeg -i \"%s\" -y -acodec pcm_s16le -ar 44100 \"%s\" > /dev/null 2>&1", fip, f_tmp);
        else if (sub_modo == 4) sprintf(cmd, "ffmpeg -y -f lavfi -i color=c=black:s=1280x720:r=30:d=5 -i \"%s\" -f lavfi -i color=c=black:s=1280x720:r=30:d=5 -filter_complex \"[1:v]scale=1280:720,format=yuv420p[v1]; [0:v]format=yuv420p[v0]; [2:v]format=yuv420p[v2]; [v0][v1][v2]concat=n=3:v=1:a=0[out]\" -map \"[out]\" -an -vcodec mpeg2video -b:v 2M -f mpeg \"%s\" > /dev/null 2>&1", fip, f_tmp);
        else strcpy(f_tmp, fip);
        if (acao == 1 && sub_modo >= 2) IGNORE_RET(system(cmd));
    }

    FILE *fin = fopen(f_tmp, "rb"), *fout = fopen(fop, "wb");
    if (!fin || !fout) { if(fin) fclose(fin); return; }
    fseek(fin, 0, SEEK_END); long t_size = ftell(fin); rewind(fin);

    if (h_size > 0 && h_size < t_size) {
        unsigned char* h_buf = malloc(h_size);
        if(fread(h_buf, 1, h_size, fin) == (size_t)h_size) fwrite(h_buf, 1, h_size, fout);
        free(h_buf);
    }

    char* est = obter_esteira_numerica(tipo_const, formula, 2000000);
    long tam_e = strlen(est), ptr = cs[0];
    unsigned char *buf = malloc(65536);
    clock_t t_i = clock();
    while (!feof(fin)) {
        size_t r = fread(buf, 1, 65536, fin); if (r <= 0) break;
        for (size_t i = 0; i < r; i++) {
            unsigned char b_o = buf[i];
            for (int j = 0; j < nf; j++) {
                long p = (ptr + cs[j]) % tam_e;
                int s = (est[p]-'0')*100 + (est[(p+1)%tam_e]-'0')*10 + (est[(p+2)%tam_e]-'0');
                if (acao == 1) buf[i] = (unsigned char)((buf[i] + (s % 256)) % 256);
                else buf[i] = (unsigned char)((buf[i] - (s % 256) + 256) % 256);
            }
            if (acao == 1) ptr += (b_o == 0) ? 1 : b_o; else ptr += (buf[i] == 0) ? 1 : buf[i];
        }
        fwrite(buf, 1, r, fout);
    }
    fclose(fin); fclose(fout);
    if (acao == 1 && sub_modo >= 2) unlink(f_tmp);
    free(buf);
    printf("[✅ CLI] Operacao Local concluida em %.4fs -> %s\n", (double)(clock()-t_i)/CLOCKS_PER_SEC, fop);
}

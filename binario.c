#include "martelos.h"
#include <dirent.h>
#include <unistd.h>

void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam);
    printf("\n--- BANCADA DE ENTRADA (Arquivos em ~/Downloads/martelos/entrada/) ---\n");
    struct dirent *dir;
    if (d) {
        int found = 0;
        while ((dir = readdir(d))) {
            if (dir->d_type == DT_REG) { printf("- %s\n", dir->d_name); found = 1; }
        }
        if (!found) printf("(Nenhum arquivo encontrado)\n");
        closedir(d);
    }
}

int is_image(const char* ext) {
    char* list[] = {".jpg", ".jpeg", ".png", ".webp", ".bmp", ".gif", ".tiff"};
    for(int i=0; i<7; i++) if(strcasecmp(ext, list[i]) == 0) return 1;
    return 0;
}

int is_audio(const char* ext) {
    char* list[] = {".mp3", ".wav", ".flac", ".ogg", ".m4a", ".aac", ".wma"};
    for(int i=0; i<7; i++) if(strcasecmp(ext, list[i]) == 0) return 1;
    return 0;
}

void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int sub_modo) {
    char ni[256], no[256], pi[512], po[512], fip[1024], fop[1024], cmd[4096], f_tmp[1024];
    const char* h = getenv("HOME");
    snprintf(pi, 512, "%s/Downloads/martelos/entrada/", h);
    snprintf(po, 512, "%s/Downloads/martelos/saida/", h);
    
    listar_arquivos_entrada(pi);
    printf("\nNome do Arquivo (ou 0 para Voltar): "); scanf("%255s", ni);
    if (strcmp(ni, "0") == 0) return;

    snprintf(fip, 1024, "%s%s", pi, ni);
    if (access(fip, F_OK) == -1) { printf("\n[❌] Erro: Arquivo nao encontrado.\n"); return; }

    char *ext = strrchr(ni, '.');
    char nome_base[256]; strcpy(nome_base, ni);
    if(ext) nome_base[ext - ni] = '\0';

    long h_size = 0;

    // LÓGICA DE CONVERSÃO COM EXTENSÕES EXPLÍCITAS
    if (sub_modo == 1) { // MARTELOS
        sprintf(no, "%s.martelos", ni);
        h_size = 0;
        strcpy(f_tmp, fip);
    } 
    else if (sub_modo == 2) { // IMAGENS
        if (!ext || !is_image(ext)) { printf("\n[❌] Formato incompativel para Imagem.\n"); return; }
        sprintf(no, "%s_glitch.bmp", nome_base);
        h_size = 54;
        if (acao == 1) {
            snprintf(f_tmp, 1024, "%s_intermediario.bmp", pi); // AGORA COM EXTENSÃO .BMP
            sprintf(cmd, "ffmpeg -i \"%s\" -y -pix_fmt bgr24 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("\n[❌] Falha do FFmpeg ao converter para BMP.\n"); return; }
        } else strcpy(f_tmp, fip);
    }
    else if (sub_modo == 3) { // AUDIOS
        if (!ext || !is_audio(ext)) { printf("\n[❌] Formato incompativel para Audio.\n"); return; }
        sprintf(no, "%s_glitch.wav", nome_base);
        h_size = 44;
        if (acao == 1) {
            snprintf(f_tmp, 1024, "%s_intermediario.wav", pi); // AGORA COM EXTENSÃO .WAV
            sprintf(cmd, "ffmpeg -i \"%s\" -y -acodec pcm_s16le -ar 44100 \"%s\" > /dev/null 2>&1", fip, f_tmp);
            if (system(cmd) != 0) { printf("\n[❌] Falha do FFmpeg ao converter para WAV.\n"); return; }
        } else strcpy(f_tmp, fip);
    }
    else if (sub_modo == 4) { // VIDEOS
        printf("Extensao final desejada (ex: .ts, .mpg) ou 0 para Voltar: "); scanf("%255s", no);
        if (strcmp(no, "0") == 0) return;
        char final_no[300]; sprintf(final_no, "%s_glitch%s", nome_base, no); strcpy(no, final_no);
        h_size = 10240; 
        strcpy(f_tmp, fip);
    }

    snprintf(fop, 1024, "%s%s", po, no);

    printf("Camadas? (ou 0 para Voltar): "); int nf; scanf("%d", &nf);
    if (nf == 0) return; if (nf < 0) nf = 1; getchar();

    long* cs = malloc(nf * sizeof(long));
    for (int i=0; i<nf; i++) {
        char m[50]; sprintf(m, "FRASE %d/%d:", i+1, nf);
        char* fr = ler_texto_ctrl_d(m); 
        int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr);
    }

    FILE *fin = fopen(f_tmp, "rb"), *fout = fopen(fop, "wb");
    if (!fin || !fout) { if(fin) fclose(fin); free(cs); return; }
    fseek(fin, 0, SEEK_END); long t_size = ftell(fin); rewind(fin);

    if (h_size > t_size) h_size = 0;
    if (h_size > 0) {
        unsigned char* h_buf = malloc(h_size);
        if (fread(h_buf, 1, h_size, fin) == (size_t)h_size) {
            fwrite(h_buf, 1, h_size, fout);
        }
        free(h_buf);
    }

    char* est = obter_esteira_numerica(tipo, f, 2000000);
    long tam_e = strlen(est);
    long ptr = cs[0];
    unsigned char *buf = malloc(65536);

    printf("[⚙️] Prensando..."); fflush(stdout);
    while (!feof(fin)) {
        size_t r = fread(buf, 1, 65536, fin);
        if (r <= 0) break;
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
    }

    fclose(fin); fclose(fout);
    if ((sub_modo == 2 || sub_modo == 3) && acao == 1) unlink(f_tmp);
    free(buf); free(cs);
    printf(" OK!\n[✅] Finalizado em: %s\n", no);
}

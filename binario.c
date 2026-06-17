#include "martelos.h"
#include <dirent.h>
void listar_arquivos_entrada(char* cam) {
    DIR *d = opendir(cam); printf("\n--- ARQUIVOS ---\n");
    struct dirent *dir; if (d) { while ((dir = readdir(d))) if (dir->d_type == DT_REG) printf("- %s\n", dir->d_name); closedir(d); }
}
void processar_arquivo_binario_n_camadas(int tipo, const char* f, int acao, int modo) {
    char ni[256], no[256], pi[512], po[512]; const char* h = getenv("HOME");
    snprintf(pi, 512, "%s/Downloads/martelos/entrada/", h); snprintf(po, 512, "%s/Downloads/martelos/saida/", h);
    listar_arquivos_entrada(pi); printf("Entrada: "); scanf("%255s", ni); printf("Saída: "); scanf("%255s", no);
    char fip[1024], fop[1024]; snprintf(fip, 1024, "%s%s", pi, ni); snprintf(fop, 1024, "%s%s", po, no);
    printf("\nCamadas? "); int nf; scanf("%d", &nf); if (nf <= 0) nf = 1; getchar();
    long* cs = malloc(nf * sizeof(long)); for (int i=0; i<nf; i++) { char m[50]; sprintf(m, "FRASE %d/%d:", i+1, nf); char* fr = ler_texto_ctrl_d(m); int d; cs[i] = calcular_casa_quiasmo(fr, &d); free(fr); }
    char* est = obter_esteira_numerica(tipo, f, 1000000); long tam_e = strlen(est); if (tam_e < 10) tam_e = 10;
    FILE *fin = fopen(fip, "rb"), *fout = fopen(fop, "wb"); if (!fin || !fout) return;
    fseek(fin, 0, SEEK_END); long tt = ftell(fin); rewind(fin);
    unsigned char *buf = malloc(65536); long ini = (modo == 2) ? 10240 : 0; if (ini > tt) ini = tt/10;
    if (ini > 0) { unsigned char *hb = malloc(ini); size_t lh = fread(hb, 1, ini, fin); fwrite(hb, 1, lh, fout); free(hb); }
    long ptr = cs[0]; size_t l;
    while ((l = fread(buf, 1, 65536, fin)) > 0) {
        for (size_t i=0; i<l; i++) {
            unsigned char bp;
            if (acao == 1) { bp = buf[i]; for (int j=0; j<nf; j++) { long p=(ptr+cs[j])%tam_e; int s=(est[p]-'0')*100+(est[(p+1)%tam_e]-'0')*10+(est[(p+2)%tam_e]-'0'); buf[i]=(unsigned char)((buf[i]+(s%256))%256); } }
            else { for (int j=nf-1; j>=0; j--) { long p=(ptr+cs[j])%tam_e; int s=(est[p]-'0')*100+(est[(p+1)%tam_e]-'0')*10+(est[(p+2)%tam_e]-'0'); buf[i]=(unsigned char)((buf[i]-(s%256)+256)%256); } bp = buf[i]; }
            ptr += (bp == 0) ? 1 : bp;
        } fwrite(buf, 1, l, fout);
    } fclose(fin); fclose(fout); free(buf); free(cs); printf("OK!\n");
}

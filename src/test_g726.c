#include <stdint.h>
#include <stdio.h>

#include "g726.h"

struct PCMHeaderInfo {
    char RIFF[4];
    uint32_t filesize;
    char WAVE[4];
    char FMT[4];
    uint32_t format;
    uint16_t fmttag;
    uint16_t channels;
    uint32_t samplerate;
    uint32_t byterate;
    uint16_t blockalign;
    uint16_t bitspersample;
    char DATA[4];
    uint32_t datalength;
};

#define ABS(a) (((a) < 0) ? -(a) : a)

// g726_16:reduce size from 480 16bit PCM (960 bytes) to 120 bytes (ratio 1:8)
// running 8kHz makes ---> 16 frames with 480 samples = 16*480=8000,
// 16 frames encoded to 120*16=1920 bytes/sec

// g726_32: 960 bytes to 240 bytes(ratio 1:4)

//gcc -o test_g726.exe test_g726.c g726.c g726_32.c g72x.c g711.c g726_16.c
static int16_t pcm_in[480];
static int16_t pcm_out[480];
static int8_t bitstream[240]; //bitstream[120];

int main(void)
{
    int i;

    printf("Test G726\n");

    FILE *fin, *fout;

    if (NULL == (fout = fopen("out.wav", "wb"))) {
        exit(-1);
    }
    if (NULL == (fin = fopen("44K16B_pcm.wav", "rb"))) {
        exit(-1);
    }

    struct PCMHeaderInfo pcmHead;
    fread(&pcmHead, sizeof(pcmHead), 1, fin);
    fread(&pcm_in,sizeof(pcm_in),1,fin);
    fclose(fin);
    // for (i = 0; i < 480; i++) {
    //     pcm_in[i] = -0x7800;
    //     pcm_out[i] = 0;
    // }

    printf("Encode...\n");
    g726_encode(pcm_in, bitstream);

    printf("Decode...\n");
    g726_decode(bitstream, pcm_out);

    pcmHead.filesize = sizeof(pcm_out);
    fwrite(&pcmHead,sizeof(pcmHead),1,fout);
    fwrite(&pcm_out,sizeof(pcm_out),1,fout);
    fclose(fout);
    for (i = 0; i < 480; i++) {
        int16_t cin = pcm_in[i];
        int16_t cout = pcm_out[i];
        int16_t diff = ABS(cin - cout);
        printf("%04x: [%04x - %04x: %d]\n", i, cin & 0xFFFF, cout & 0xFFFF, diff & 0xFFFF);
    }

    printf("Done.\n");
    return 0;
}

#include "g726.h"

#include <stdio.h>

#include "g72x.h"
#include "memory.h"

//#define g726_16
//----------------------------------------
void g726_encode(int16_t *pcm, int8_t *bitstream)
{
    g726_state state_ptr;

    g726_init_state(&state_ptr);

    int i;
#ifdef g726_16
    for (i = 0; i < 120; i++) {
        *(bitstream + i) =
            (((char)(g726_16_encoder(pcm[i * 4], AUDIO_ENCODING_LINEAR, &state_ptr))) << 6) |
            (((char)(g726_16_encoder(pcm[i * 4 + 1], AUDIO_ENCODING_LINEAR, &state_ptr))) << 4) |
            (((char)(g726_16_encoder(pcm[i * 4 + 2], AUDIO_ENCODING_LINEAR, &state_ptr))) << 2) |
            (((char)(g726_16_encoder(pcm[i * 4 + 3], AUDIO_ENCODING_LINEAR, &state_ptr))));
    }
#else
    for (i = 0; i < 240; i++) {
        *(bitstream + i) =
            (((char)(g726_32_encoder(pcm[i * 2], AUDIO_ENCODING_LINEAR, &state_ptr))) << 4) |
            (((char)(g726_32_encoder(pcm[i * 2 + 1], AUDIO_ENCODING_LINEAR, &state_ptr))));
    }
#endif
}

//-----------------------------------------
void g726_decode(int8_t *bitstream, int16_t *pcm)
{
    g726_state state_ptr;

    g726_init_state(&state_ptr);

    int i;
#ifdef g726_16
    for (i = 0; i < 120; i++) {
        int in = (int)(((*(bitstream + i)) & (char)192) >> 6);
        pcm[i * 4] = g726_16_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);

        in = (int)(((*(bitstream + i)) & (char)48) >> 4);
        pcm[i * 4 + 1] = g726_16_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);

        in = (int)(((*(bitstream + i)) & (char)12) >> 2);
        pcm[i * 4 + 2] = g726_16_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);

        in = (int)(((*(bitstream + i)) & (char)3));
        pcm[i * 4 + 3] = g726_16_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);
    }
#else
    for (i = 0; i < 240; i++) {
        int in = (int)(((*(bitstream + i)) & (char)240) >> 4);
        pcm[i * 2] = g726_32_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);

        in = (int)(((*(bitstream + i)) & (char)15));
        pcm[i * 2 + 1] = g726_32_decoder(in, AUDIO_ENCODING_LINEAR, &state_ptr);
    }
#endif
}

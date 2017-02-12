#ifndef AUDIO_WAV_H
#define AUDIO_WAV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ff.h>
#include <stdint.h>
#include <stddef.h>
#include "audio_dac.h"

struct wav_data {
    FIL *file;
    uint32_t sample_rate;
    uint8_t sample_size;
    uint32_t data_len;
    uint32_t data_pos;
};

int wav_read_header(struct wav_data *d, FIL *f);

bool wav_read_cb(void *arg, audio_sample_t *buffer, size_t buf_len, size_t *samples_written);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_WAV_H */
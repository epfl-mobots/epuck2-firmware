#ifndef AUDIO_DAC_H
#define AUDIO_DAC_H

#include <hal.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef bool (*audio_callback_t)(void *arg, dacsample_t *buffer, size_t len,
                                 size_t *samples_written);

void audio_dac_play(audio_callback_t data_cb, void *cb_arg, uint32_t sample_rate,
                    dacsample_t *buffer, size_t len);

void audio_dac_init(void);

void audio_dac_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DAC_H */

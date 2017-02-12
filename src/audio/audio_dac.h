#ifndef AUDIO_DAC_H
#define AUDIO_DAC_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef uint16_t audio_sample_t;

enum {
    AUDIO_DAC_OK = 0,
    AUDIO_DAC_ERR
};

/** Audio data callback
 *
 * @param [in] arg Argument pointer.
 * @param [in] buffer Sample buffer.
 * @param [in] len Buffer length.
 * @param [out] samples_written The number of samples written by the callback.
 * @returns true on last conversion.
 */
typedef bool (*audio_callback_t)(void *arg,
                                 audio_sample_t *buffer,
                                 size_t len,
                                 size_t *samples_written);

/** Contiuous audio conversion
 *
 * @param [in] data_cb Data callback which is periodically called to fill the DAC conversion buffer.
 * @param [in] cb_arg Argument pointer passed to the data callback.
 * @param [in] sample_rate Audio sample rate.
 * @param [in] buffer Audio sample buffer.
 * @param [in] len  Buffer length, must be multiple of 2.
 * @returns 0 if successful, error code otherwise
 */
int audio_dac_convert(audio_callback_t data_cb,
                      void *cb_arg,
                      uint32_t sample_rate,
                      audio_sample_t *buffer,
                      size_t len);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_DAC_H */

#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <audio_dac.h>
#include <ff.h>
#include "sdcard.h"

#define DAC_BUFFER_SIZE 1000
static dacsample_t buffer[DAC_BUFFER_SIZE];

static bool file_read_cb(void *arg, dacsample_t *buffer, size_t len, size_t *samples_written)
{
    FIL *file = (FIL *)arg;
    UINT n;
    FRESULT res = f_read(file, buffer, len * 2, &n);
    if (res != FR_OK) {
        /* read error, stop conversion */
        *samples_written = 0;
        return true;
    }
    size_t i;
    for (i = 0; i < n / 2; i++) {
        buffer[i] += -INT16_MIN;
    }
    *samples_written = n / 2;
    if (n != len * 2) {
        /* end of file */
        return true;
    } else {
        return false;
    }
}

void audio_thd_main(void *arg)
{
    (void) arg;

    while (1) {
        static FIL file;
        FRESULT res = f_open(&file, "/sound.wav", FA_READ);
        if (res != FR_OK) {
            chThdSleepMilliseconds(100);
            continue;
        }

        // todo: decode WAV header
        uint32_t sample_rate = 44100;

        audio_dac_init();
        audio_dac_play(file_read_cb, &file, sample_rate, buffer, DAC_BUFFER_SIZE);
        audio_dac_deinit();

        f_close(&file);

        chThdSleepMilliseconds(100);
    }
}

void audio_start(void)
{
    static THD_WORKING_AREA(audio_thd, 2000);
    chThdCreateStatic(audio_thd, sizeof(audio_thd), LOWPRIO, audio_thd_main, NULL);
}

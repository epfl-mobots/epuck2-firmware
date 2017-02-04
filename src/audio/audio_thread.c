#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <ff.h>
#include "main.h"

#include "sdcard.h"
#include "audio/audio_dac.h"
#include "audio/audio_thread.h"

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
    static TOPIC_DECL(request_topic, audio_play_request_t);
    static TOPIC_DECL(result_topic, audio_play_result_t);

    messagebus_advertise_topic(&bus, &request_topic.topic, "/audio/play/request");
    messagebus_advertise_topic(&bus, &result_topic.topic, "/audio/play/result");

    while (1) {
        /* Wait for a request to play a file. */
        audio_play_request_t req;
        messagebus_topic_wait(&request_topic.topic, &req, sizeof(req));

        /* If the file was not found, reply with appropriate status. */
        static FIL file;
        if (f_open(&file, req.path, FA_READ) != FR_OK) {
            audio_play_result_t res;
            res.status = AUDIO_FILE_NOT_FOUND;
            messagebus_topic_publish(&result_topic.topic, &res, sizeof(res));
            continue;
        }

        // TODO decode WAV header
        uint32_t sample_rate = 44100;

        /* Play the file. */
        audio_dac_init();
        audio_dac_play(file_read_cb, &file, sample_rate, buffer, DAC_BUFFER_SIZE);
        audio_dac_deinit();
        f_close(&file);

        /* Signal success. */
        audio_play_result_t res;
        res.status = AUDIO_OK;
        messagebus_topic_publish(&result_topic.topic, &res, sizeof(res));
    }
}

void audio_start(void)
{
    static THD_WORKING_AREA(audio_thd, 2000);
    chThdCreateStatic(audio_thd, sizeof(audio_thd), LOWPRIO, audio_thd_main, NULL);
}

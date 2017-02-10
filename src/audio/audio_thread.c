#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <ff.h>
#include <sdcard.h>
#include <main.h>

#include "audio_dac.h"
#include "audio_wav.h"
#include "audio_thread.h"

#define DAC_BUFFER_SIZE 1000
static dacsample_t buffer[DAC_BUFFER_SIZE];

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

        /* WAV decodede error. */
        static struct wav_data wav;
        if (wav_read_header(&wav, &file) != 0) {
            audio_play_result_t res;
            res.status = AUDIO_WAV_DECODE;
            messagebus_topic_publish(&result_topic.topic, &res, sizeof(res));
            f_close(&file);
            continue;
        }

        /* Play the file. */
        audio_dac_convert(wav_read_cb, &wav, wav.sample_rate, buffer, DAC_BUFFER_SIZE);
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

#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <audio/audio_dac.h>
#include "file.h"
#include <chprintf.h>
extern BaseSequentialStream *stdout;

#define DAC_SAMPLE_MAX ((1<<16) - 1)

float saw(float x)
{
    return x - floorf(x);
}

float rect(float x)
{
    if (x - floorf(x) > M_PI) {
        return 0.0f;
    } else {
        return 1.0f;
    }
}

float sine(float x)
{
    return 0.5*sinf(2*M_PI*x)+0.5;
}

struct callback_arg_s {
    float omega;
    uint32_t time;
    uint32_t sample_rate;
    float (*func)(float t);
};

bool data_callback(void *arg, dacsample_t *buffer, size_t len, size_t *samples_written)
{
    struct callback_arg_s *args = (struct callback_arg_s *)arg;
    palTogglePad(GPIOD, GPIOD_LED4);
    if (palReadPad(GPIOA, GPIOA_BUTTON)) {
        *samples_written = 0;
        return true;
    }
    size_t i;
    uint32_t t = args->time;
    float omega = args->omega;
    for (i = 0; i < len; i++) {
        buffer[i] = (dacsample_t) (DAC_SAMPLE_MAX * args->func(omega*(i + t)));
    }
    t += len;
    if (t >= args->sample_rate) {
        t -= args->sample_rate;
    }
    args->time = t;
    *samples_written = len;
    return false;
}

bool rect_callback(void *arg, dacsample_t *buffer, size_t len, size_t *samples_written)
{
    palTogglePad(GPIOD, GPIOD_LED4);
    if (palReadPad(GPIOA, GPIOA_BUTTON)) {
        *samples_written = 0;
        return true;
    }
    *samples_written = len;
    return false;
}


#define DAC_BUFFER_SIZE 10000
void audio_thread_main(void *arg)
{
    (void) arg;
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);

    static dacsample_t buffer[DAC_BUFFER_SIZE];

    sd_init();
    sdcard_mount();
    const char *file = "/sound.wav";
    int res = sound_file_open(file);
    if (res) {
        chprintf(stdout, "cannot open file\n");
    } else {
        chprintf(stdout, "open %s\n", file);

        audio_dac_convert(sdc_callback, NULL, 44100, buffer, DAC_BUFFER_SIZE);

        sound_file_close();
    }
    chprintf(stdout, "SD: unmount\n");
    sdcard_unmount();

    uint32_t freq = 440;
    dacsample_t rect_buf[2] = {0, 0xffff};
    audio_dac_convert(rect_callback, NULL, 2*freq, rect_buf, 2);


    struct callback_arg_s args;
    args.sample_rate = 44100;
    args.omega = (float) 440 / args.sample_rate;

    args.time = 0;
    args.func = sine;

    while (true) {
        audio_dac_convert(data_callback, &args, args.sample_rate, buffer, DAC_BUFFER_SIZE);

        chThdSleepMilliseconds(500);
        while(!palReadPad(GPIOA, GPIOA_BUTTON)) {
            chThdSleepMilliseconds(100);
        }
        chThdSleepMilliseconds(500);
    }
}

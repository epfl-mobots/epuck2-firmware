#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <audio_dac.h>

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

#define DAC_BUFFER_SIZE 1000
void audio_thread_main(void *arg)
{
    (void) arg;
    palSetPadMode(GPIOA, 4, PAL_MODE_INPUT_ANALOG);

    static dacsample_t buffer[DAC_BUFFER_SIZE];
    struct callback_arg_s args;
    args.sample_rate = 44100;
    args.omega = (float) 440 / args.sample_rate;

    args.time = 0;
    args.func = sine;

    while (true) {
        audio_dac_init();
        audio_dac_play(data_callback, &args, args.sample_rate, buffer, DAC_BUFFER_SIZE);
        audio_dac_deinit();

        chThdSleepMilliseconds(500);
        while(!palReadPad(GPIOA, GPIOA_BUTTON)) {
            chThdSleepMilliseconds(100);
        }
        chThdSleepMilliseconds(500);
    }
}

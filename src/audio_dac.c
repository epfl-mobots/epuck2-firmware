#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include "audio_dac.h"

static void dac_continuous_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n);
static void dac_single_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n);
static void dac_final_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n);
static void dac_stop_cb(DACDriver *dacp, const dacsample_t *prev, size_t n);
static void error_cb(DACDriver *dacp, dacerror_t err);
static size_t dac_adjust_length(size_t len);
static void dac_final_conversion(dacsample_t *buf, size_t len);
static dacsample_t *get_next_buffer(void);
static void dac_start_timer(uint32_t sample_rate);
static void dac_single_conversion(uint32_t sample_rate, dacsample_t *buf, size_t len);
static void dac_continuous_conversion(uint32_t sample_rate, dacsample_t *buf, size_t len);

static const DACConfig dac2cfg1 = {
    init: 0,
    datamode: DAC_DHRM_12BIT_LEFT
};

static DACConversionGroup dacgrpcfg1;
static binary_semaphore_t dac_signal;
static size_t buffer_len;
static dacsample_t *next_buffer;
static dacsample_t *dual_buffer;

static void dac_continuous_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n)
{
    (void)dacp;
    /* set next buffer to be filled */
    if (prev == &dual_buffer[0]) {
        next_buffer = &dual_buffer[0];
    } else {
        next_buffer = &dual_buffer[n];
    }

    /* signal thread */
    chBSemSignalI(&dac_signal);
}

/* second last callback, called on half conversion */
static void dac_single_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n)
{
    (void)dacp;
    (void)prev;
    (void)n;
    /* stop DAC when conversion is complete. */
    dacgrpcfg1.end_cb = dac_stop_cb;
}

/* restarts DAC for final conversion length */
static void dac_final_conversion_cb(DACDriver *dacp, const dacsample_t *prev, size_t n)
{
    (void)prev;
    (void)n;
    dacStopConversionI(dacp);
    dacgrpcfg1.end_cb = dac_single_conversion_cb;

    /* restart dac for single conversion with new length */
    dacStartConversionI(dacp, &dacgrpcfg1, next_buffer, buffer_len);

    /* no thread signaling until last conversion done */
}

static void dac_stop_cb(DACDriver *dacp, const dacsample_t *prev, size_t n)
{
    (void)prev;
    (void)n;

    /* stop the DAC */
    gptStopTimerI(&GPTD7);
    dacStopConversionI(dacp);

    /* signal thread */
    chBSemSignalI(&dac_signal);
}

static void error_cb(DACDriver *dacp, dacerror_t err)
{
    (void)dacp;
    (void)err;
    chSysHalt("DAC failure");
}

static size_t dac_adjust_length(size_t len)
{
    /* length must be an even number */
    if (len & 1) {
        len -= 1;
    }
    return len;
}

static void dac_final_conversion(dacsample_t *buf, size_t len)
{
    chSysLock();
    len = dac_adjust_length(len);
    if (len > 0) {
        next_buffer = buf;
        buffer_len = len;
        dacgrpcfg1.end_cb = dac_final_conversion_cb;
    } else {
        dacgrpcfg1.end_cb = dac_stop_cb;
    }
    chBSemWaitS(&dac_signal);
    chSysUnlock();
}

static dacsample_t *get_next_buffer(void)
{
    chBSemWait(&dac_signal);
    return next_buffer;
}

static void dac_start_timer(uint32_t sample_rate)
{
    static GPTConfig gpt7cfg1;
    gpt7cfg1.frequency = 2*sample_rate;
    gpt7cfg1.callback = NULL;
    gpt7cfg1.cr2 = TIM_CR2_MMS_1; /* trigger output on timer update */
    gpt7cfg1.dier = 0U;
    gptStart(&GPTD7, &gpt7cfg1);
    gptStartContinuous(&GPTD7, 2U);
}

static void dac_single_conversion(uint32_t sample_rate, dacsample_t *buf, size_t len)
{
    dacgrpcfg1.end_cb = dac_single_conversion_cb;

    chBSemObjectInit(&dac_signal, true);

    dacStartConversion(&DACD2, &dacgrpcfg1, buf, len*2);
    dac_start_timer(sample_rate);

    chBSemWait(&dac_signal);
}

static void dac_continuous_conversion(uint32_t sample_rate, dacsample_t *buf, size_t len)
{
    dual_buffer = buf;
    dacgrpcfg1.end_cb = dac_continuous_conversion_cb;

    chBSemObjectInit(&dac_signal, true);

    dacStartConversion(&DACD2, &dacgrpcfg1, buf, len);
    dac_start_timer(sample_rate);
}

void audio_dac_play(audio_callback_t data_cb, void *cb_arg, uint32_t sample_rate, dacsample_t *buffer, size_t len)
{
    size_t samples;
    bool last;

    len = dac_adjust_length(len);
    if (len == 0) {
        return;
    }

    last = data_cb(cb_arg, buffer, len, &samples);
    if (last) {
        dac_single_conversion(sample_rate, buffer, samples);
        return;
    }

    dac_continuous_conversion(sample_rate, buffer, len);

    while (1) {
        dacsample_t* buf;
        buf = get_next_buffer();
        samples = 0;
        last = data_cb(cb_arg, buf, len/2, &samples);
        if (last) {
            dac_final_conversion(buf, samples);
            break;
        }
    }
}

void audio_dac_init(void)
{
    dacgrpcfg1.num_channels = 1U;
    dacgrpcfg1.end_cb = NULL;
    dacgrpcfg1.error_cb = error_cb;
    dacgrpcfg1.trigger = DAC_TRG(0);
    dacStart(&DACD2, &dac2cfg1);
}

void audio_dac_deinit(void)
{
    dacStop(&DACD2);
}

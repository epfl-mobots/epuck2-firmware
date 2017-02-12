#include <ch.h>
#include <hal.h>
#include <stdint.h>
#include <stdbool.h>
#include "audio_dac.h"

/**
 * Audio DAC driver
 *
 * The driver periodically calls a data callback to fill a sample buffer, which
 * is converted continuously in the background by the DAC peripheral using DMA.
 * The data callback must finish writing samples before the DAC driver starts
 * converting the new buffer, otherwise audible glitches may appear.
 *
 * The STM32 DAC peripheral allows continuous conversion of a sample buffer:
 * When it reaches the end of the buffer it restarts at the beginning.
 * There are two interrupts generated during conversion, at half and at the end
 * of the buffer. In the ISR the audio thread is notified about where to write
 * new samples and the DAC is reconfigured for the next conversion.
 */

/* Select DAC driver: DACD1 or DACD2 */
#define DAC_DRIVER DACD2

static void dac_continuous_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n);
static void dac_last_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n);
static void dac_single_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n);
static void dac_stop_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n);
static void error_cb(DACDriver *dacp, dacerror_t err);
static audio_sample_t *dac_next_write_pos(void);
static void dac_last_conversion(audio_sample_t *buf, size_t len);
static void dac_continuous_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len);
static void dac_single_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len);
static void dac_start_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len);
static void dac_start(void);

static DACConversionGroup dac_conversion;
static binary_semaphore_t dac_signal;
static size_t conversion_len;
static audio_sample_t *dac_write_pos;
static audio_sample_t *dac_buffer;

/* DAC driver interrupt callbacks */

/* Signals audio thread where to write new data. */
static void dac_continuous_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n)
{
    (void)dacp;
    chSysLockFromISR();
    /* set next write position */
    if (prev == &dac_buffer[0]) {
        dac_write_pos = &dac_buffer[0];
    } else {
        dac_write_pos = &dac_buffer[n];
    }

    /* wakeup thread */
    chBSemSignalI(&dac_signal);
    chSysUnlockFromISR();
}

/* configures DAC for last conversion with different buffer length */
static void dac_last_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n)
{
    (void)prev;
    (void)n;
    chSysLockFromISR();

    /* start final conversion */
    dacStopConversionI(dacp);
    dac_conversion.end_cb = dac_single_conversion_cb;

    /* restart dac for single conversion with new length */
    dacStartConversionI(dacp, &dac_conversion, (dacsample_t *)dac_write_pos, conversion_len);

    chSysUnlockFromISR();
}

/* configures DAC to stop at end of ongoing conversion. */
static void dac_single_conversion_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n)
{
    (void)dacp;
    (void)prev;
    (void)n;
    /* stop DAC when conversion is complete. */
    dac_conversion.end_cb = dac_stop_cb;
}

static void dac_stop_cb(DACDriver *dacp, const audio_sample_t *prev, size_t n)
{
    (void)prev;
    (void)n;
    chSysLockFromISR();

    /* stop the DAC */
    gptStopTimerI(&GPTD6);
    dacStopConversionI(dacp);

    /* wakeup thread */
    chBSemSignalI(&dac_signal);
    chSysUnlockFromISR();
}

static void error_cb(DACDriver *dacp, dacerror_t err)
{
    (void)dacp;
    (void)err;
    chSysHalt("DAC error");
}

static audio_sample_t *dac_next_write_pos(void)
{
    chBSemWait(&dac_signal);
    return dac_write_pos;
}

static void dac_last_conversion(audio_sample_t *buf, size_t len)
{
    chSysLock();

    /* buffer length must be even */
    len -= len & 1;

    if (len > 0) {
        dac_write_pos = buf;
        conversion_len = len;
        dac_conversion.end_cb = dac_last_conversion_cb;
    } else {
        dac_conversion.end_cb = dac_stop_cb;
    }

    /* wait until conversion done */
    chBSemWaitS(&dac_signal);

    chSysUnlock();
}

static void dac_single_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len)
{
    /* buffer length must be even */
    len -= len & 1;

    if (len == 0) {
        return;
    }

    dac_conversion.end_cb = dac_single_conversion_cb;
    dac_start_conversion(sample_rate, buf, len);

    /* wait until conversion done */
    chBSemWait(&dac_signal);
}

static void dac_continuous_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len)
{
    dac_buffer = buf;
    dac_conversion.end_cb = dac_continuous_conversion_cb;
    dac_start_conversion(sample_rate, buf, len);
}

static void dac_start_conversion(uint32_t sample_rate, audio_sample_t *buf, size_t len)
{
    /* init thread signaling semaphore */
    chBSemObjectInit(&dac_signal, true);

    dacStartConversion(&DAC_DRIVER, &dac_conversion, (dacsample_t *)buf, len);

    /* start timer for DAC trigger */
    static GPTConfig config;
    config.frequency = STM32_TIMCLK1; /* run timer at full frequency */
    config.callback = NULL;
    config.cr2 = TIM_CR2_MMS_1; /* trigger output on timer update */
    config.dier = 0U;
    gptStart(&GPTD6, &config);
    gptStartContinuous(&GPTD6, STM32_TIMCLK1 / sample_rate); /* rounded divider */
}

static void dac_start(void)
{
    dac_conversion.num_channels = 1U;
    dac_conversion.end_cb = NULL;
    dac_conversion.error_cb = error_cb;
    dac_conversion.trigger = DAC_TRG(0);

    static DACConfig dac_config;
    dac_config.init = 0;
    dac_config.datamode = DAC_DHRM_12BIT_LEFT;

    dacStart(&DAC_DRIVER, &dac_config);
}

int audio_dac_convert(audio_callback_t data_cb,
                      void *cb_arg,
                      uint32_t sample_rate,
                      audio_sample_t *buffer,
                      size_t len)
{
    size_t nb_samples = 0;
    bool last;

    if (len < 2) {
        return AUDIO_DAC_ERR;
    }

    if (len & 1) {
        len -= 1;
    }

    dac_start();

    /* fill the first buffer */
    last = data_cb(cb_arg, buffer, len, &nb_samples);

    if (last) {
        /* done after one conversion. */
        dac_single_conversion(sample_rate, buffer, nb_samples);
        goto stop;
    }

    dac_continuous_conversion(sample_rate, buffer, len);

    /* data copy loop */
    while (1) {
        audio_sample_t* buf;
        nb_samples = 0;

        /* wait for next writing position to become free  */
        buf = dac_next_write_pos();

        last = data_cb(cb_arg, buf, len / 2, &nb_samples);

        if (last) {
            /* start last conversion, waits until done */
            dac_last_conversion(buf, nb_samples);
            break;
        }
    }

stop:
    dacStop(&DAC_DRIVER);
    return AUDIO_DAC_OK;
}

#include "ch.h"
#include "hal.h"
#include "main.h"
#include "sensors/range.h"
#include "sensors/vl6180x/vl6180x.h"

#define MILLIMETER_TO_METER 1e-3f

void range_get_range(float *range)
{
    messagebus_topic_t *topic;
    range_t range_sample;

    topic = messagebus_find_topic_blocking(&bus, "/range");

    messagebus_topic_wait(topic, &range_sample, sizeof(range_sample));

    *range = range_sample.raw;
}

static THD_WORKING_AREA(range_reader_thd_wa, 1024);
static THD_FUNCTION(range_reader_thd, arg)
{
    (void)arg;
    vl6180x_t vl6180x;


    messagebus_topic_t range_topic;
    MUTEX_DECL(range_topic_lock);
    CONDVAR_DECL(range_topic_condvar);
    range_t range_topic_value;

    range_t range_sample;

    static uint8_t temp;
    chRegSetThreadName("Range_reader");

    /* Power on the ToF sensor. */
    palSetPad(GPIOD, GPIOD_2V8_ON);
    palSetPad(GPIOE, GPIOE_RF_GPIO0_1);

    chThdSleepMilliseconds(100);

    /* Create a sensor instance and configure it. */
    vl6180x_init(&vl6180x, &I2CD1, VL6180X_DEFAULT_ADDRESS);
    i2cAcquireBus(vl6180x.i2c);
    vl6180x_configure(&vl6180x);
    i2cReleaseBus(vl6180x.i2c);

    /* Create the range topic. */
    messagebus_topic_init(&range_topic, &range_topic_lock, &range_topic_condvar, &range_topic_value, sizeof(range_topic_value));
    messagebus_advertise_topic(&bus, &range_topic, "/range");

    while (TRUE) {
        // Read sensor
        i2cAcquireBus(vl6180x.i2c);
        vl6180x_measure_distance(&vl6180x, &temp);
        i2cReleaseBus(vl6180x.i2c);

        range_sample.raw_mm = temp;
        range_sample.raw = range_sample.raw_mm * MILLIMETER_TO_METER;

        messagebus_topic_publish(&range_topic, &range_sample, sizeof(range_sample));
    }
}

void range_start(void)
{
    chThdCreateStatic(range_reader_thd_wa,
                      sizeof(range_reader_thd_wa),
                      NORMALPRIO,
                      range_reader_thd,
                      NULL);
}

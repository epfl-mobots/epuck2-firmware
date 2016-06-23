#include <stdio.h>
#include <string.h>
#include <ch.h>
#include <hal.h>
#include "body_leds.h"
#include "main.h"

/** Address of the LTC3220 LED driver. */
#define LED_DRIVER_ADDRESS 0x1c

#define LED_BRIGHNESS_MAX_VALUE 31

static struct {
    messagebus_topic_t topic;
    mutex_t lock;
    condition_variable_t condvar;
    body_led_msg_t value;
} led_topics[BODY_LED_COUNT];

/** Sets a body LED to a given value.
 *
 * @param [in] led Led index, between 0 and 18.
 * @param [in] value Current value, between 0 and 64 (20 mA).
 */
static void led_set(int led, int value)
{
    uint8_t reg[] = {
        led + 1,
        value,
    };

    i2cAcquireBus(&I2CD1);
    i2cMasterTransmit(&I2CD1, LED_DRIVER_ADDRESS, reg, sizeof(reg), NULL, 0);
    i2cReleaseBus(&I2CD1);
}


static THD_WORKING_AREA(body_led_thd_wa, 512);
static THD_FUNCTION(body_led_thd, arg)
{
    (void) arg;
    char name[32];

    chRegSetThreadName(__FUNCTION__);

    memset(led_topics, 0, sizeof(led_topics));

    for (int i = 0; i < 18; i++) {
        led_set(i, 0);
    }

    for (int i = BODY_LED_COUNT - 1; i >= 0; i--) {
        sprintf(name, "/body_leds/%d", i);

        chMtxObjectInit(&led_topics[i].lock);
        chCondObjectInit(&led_topics[i].condvar);

        messagebus_topic_init(&led_topics[i].topic,
                              &led_topics[i].lock,
                              &led_topics[i].condvar,
                              &led_topics[i].value,
                              sizeof(body_led_msg_t));
        messagebus_advertise_topic(&bus, &led_topics[i].topic, name);
    }

    while (true) {
        body_led_msg_t msg;
        for (int i = 0; i < BODY_LED_COUNT; i++) {
            messagebus_topic_read(&led_topics[i].topic, &msg, sizeof(msg));
            led_set(i, (int)(msg.value * LED_BRIGHNESS_MAX_VALUE));
        }
        chThdSleepMilliseconds(100);
    }
}

void body_leds_start(void)
{
    chThdCreateStatic(body_led_thd_wa,
                      sizeof(body_led_thd_wa),
                      NORMALPRIO,
                      body_led_thd,
                      NULL);
}

#include <ch.h>
#include <hal.h>
#include "sensors/battery_level.h"
#include "main.h"

/** Level at which the warning led will start blinking. */
#define WARNING_LEVEL_V 7.

/** Level at which the robot will shut itself down. */
#define SHUTDOWN_LEVEL_V 6.5

#define WARNING_BLINK_FREQUENCY 2

static THD_FUNCTION(battery_protection_thd, arg)
{
    (void) arg;
    chRegSetThreadName(__FUNCTION__);

    battery_msg_t msg;
    messagebus_topic_t *topic;

    /* Waits for the topic to appear. */
    topic = messagebus_find_topic_blocking(&bus, "/battery_level");

    /* Make sure the topic was published to at least once. */
    messagebus_topic_wait(topic, &msg, sizeof(msg));

    while (true) {
        messagebus_topic_read(topic, &msg, sizeof(msg));

        if (msg.voltage < WARNING_LEVEL_V) {
            palTogglePad(GPIOD, GPIOD_LED_ERROR);
        }

        if (msg.voltage < SHUTDOWN_LEVEL_V) {
            /* Only works if the force power jumper is off. */
            board_shutdown();
        }

        chThdSleepMilliseconds(500 / WARNING_BLINK_FREQUENCY);
    }
}

void battery_protection_start(void)
{
    static THD_WORKING_AREA(battery_protection_thd_wa, 1024);
    chThdCreateStatic(battery_protection_thd_wa,
                      sizeof(battery_protection_thd_wa),
                      HIGHPRIO,
                      battery_protection_thd,
                      NULL);
}

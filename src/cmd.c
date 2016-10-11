#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <hal.h>
#include <test.h>
#include <chprintf.h>
#include <shell.h>
#include "usbconf.h"
#include "sensors/imu.h"
#include "sensors/encoder.h"
#include "sensors/range.h"
#include "config_flash_storage.h"
#include "sensors/proximity.h"
#include "sensors/battery_level.h"
#include "sensors/imu.h"
#include "sensors/motor_current.h"
#include "motor_pwm.h"
#include "ff.h"
#include "main.h"
#include "body_leds.h"

#define TEST_WA_SIZE        THD_WORKING_AREA_SIZE(256)
#define SHELL_WA_SIZE       THD_WORKING_AREA_SIZE(2048)

static void cmd_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    thread_t *tp;

    (void)argv;
    if (argc > 0) {
        chprintf(chp, "Usage: test\r\n");
        return;
    }
    tp = chThdCreateFromHeap(NULL, TEST_WA_SIZE, chThdGetPriorityX(),
                             TestThread, chp);
    if (tp == NULL) {
        chprintf(chp, "out of memory\r\n");
        return;
    }
    chThdWait(tp);
}

static void cmd_motor(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 2) {
        chprintf(chp, "Usage: pwm left|right percentage\r\n");
        return;
    }

    float value = atoi(argv[1]) / 100.;

    if (!strcmp("left", argv[0])) {
        motor_left_pwm_set(value);
    } else if (!strcmp("right", argv[0]))   {
        motor_right_pwm_set(value);
    } else {
        chprintf(chp, "Unknown motor \"%s\".\r\n", argv[0]);
    }
}

static void cmd_encoders(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    messagebus_topic_t *encoders_topic;
    encoders_msg_t values;

    encoders_topic = messagebus_find_topic_blocking(&bus, "/encoders");
    messagebus_topic_wait(encoders_topic, &values, sizeof(values));

    chprintf(chp, "left: %ld\r\nright: %ld\r\n", values.left, values.right);
}

static void cmd_wheel_pos(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    messagebus_topic_t *wheel_pos_topic;
    wheel_pos_msg_t values;

    wheel_pos_topic = messagebus_find_topic_blocking(&bus, "/wheel_pos");
    messagebus_topic_wait(wheel_pos_topic, &values, sizeof(values));

    chprintf(chp, "left: %f\r\nright: %f\r\n", values.left, values.right);
}

static void cmd_imu(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    messagebus_topic_t *topic;
    imu_msg_t msg;
    topic = messagebus_find_topic(&bus, "/imu");

    if (topic == NULL) {
        chprintf(chp, "Could not find topic.\r\n");
        return;
    }

    if (messagebus_topic_read(topic, &msg, sizeof(msg)) == false) {
        chprintf(chp, "Topic was never published.\r\n");
        return;
    }

    chprintf(chp, "gyro: %.2f %.2f %.2f\r\n", msg.roll_rate[0], msg.roll_rate[1], msg.roll_rate[2]);
    chprintf(chp, "acc: %.2f %.2f %.2f\r\n", msg.acceleration[0], msg.acceleration[1], msg.acceleration[2]);
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;
    NVIC_SystemReset();
}

static void cmd_sdcard(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;

    FRESULT err;
    FATFS SDC_FS;

    chprintf(chp, "Starting SDC driver...\r\n");
    sdcStart(&SDCD1, NULL);
    chThdSleepMilliseconds(500);

    chprintf(chp, "Connecting to SDC...");
    while (true) {
        if (sdcConnect(&SDCD1) == 0) {
            chprintf(chp, "OK");
            break;
        } else {
            chprintf(chp, "FAILED");
        }
        chThdSleepMilliseconds(50);
    }
    chprintf(chp, "\r\n");

    chThdSleepMilliseconds(500);

    chprintf(chp, "Mouting card... ");
    err = f_mount(&SDC_FS, "", 1);
    if (err == FR_OK) {
        chprintf(chp, "OK");
    } else {
        chprintf(chp, "FAILED (err code=%d)", err);
    }

    chprintf(chp, "\r\n");

    sdcDisconnect(&SDCD1);
}

static void cmd_range(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    float distance_m;

    range_get_range(&distance_m);

    chprintf(chp, "distance = %.2f m\r\n", distance_m);
}

static void cmd_proximity(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    proximity_msg_t msg;
    messagebus_topic_t *topic;

    topic = messagebus_find_topic(&bus, "/proximity");

    if (topic == NULL) {
        chprintf(chp, "Proximity topic not found!\r\n");
        return;
    }

    messagebus_topic_read(topic, &msg, sizeof(msg));

    for (int i = 0; i < PROXIMITY_NB_CHANNELS; i++) {
        chprintf(chp, "%4d\t", msg.delta[i]);
    }
    chprintf(chp, "\r\n");
}

static void cmd_battery(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    battery_msg_t msg;
    messagebus_topic_t *topic = messagebus_find_topic(&bus, "/battery_level");

    if (topic == NULL) {
        chprintf(chp, "Battery topic not found!\r\n");
        return;
    }

    messagebus_topic_read(topic, &msg, sizeof(msg));

    chprintf(chp, "Battery voltage: %.2f [V]\r\n", msg.voltage);
}

static void cmd_topics(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    chprintf(chp, "available topics:\r\n");

    MESSAGEBUS_TOPIC_FOREACH(&bus, topic) {
        chprintf(chp, "%s\r\n", topic->name);
    }
}

static void tree_indent(BaseSequentialStream *out, int indent)
{
    int i;
    for (i = 0; i < indent; ++i) {
        chprintf(out, "  ");
    }
}

static void show_config_tree(BaseSequentialStream *out, parameter_namespace_t *ns, int indent)
{
    parameter_t *p;
    char string_buf[64];

    tree_indent(out, indent);
    chprintf(out, "%s:\r\n", ns->id);

    for (p=ns->parameter_list; p!=NULL; p=p->next) {
        tree_indent(out, indent + 1);
        if (parameter_defined(p)) {
            switch (p->type) {
                case _PARAM_TYPE_SCALAR:
                    chprintf(out, "%s: %f\r\n", p->id, parameter_scalar_get(p));
                    break;

                case _PARAM_TYPE_INTEGER:
                    chprintf(out, "%s: %d\r\n", p->id, parameter_integer_get(p));
                    break;

                case _PARAM_TYPE_BOOLEAN:
                    chprintf(out, "%s: %s\r\n", p->id, parameter_boolean_get(p)?"true":"false");
                    break;

                case _PARAM_TYPE_STRING:
                    parameter_string_get(p, string_buf, sizeof(string_buf));
                    chprintf(out, "%s: %s\r\n", p->id, string_buf);
                    break;

                default:
                    chprintf(out, "%s: unknown type %d\r\n", p->id, p->type);
                    break;
            }
        } else {
            chprintf(out, "%s: [not set]\r\n", p->id);
        }
    }

    if (ns->subspaces) {
        show_config_tree(out, ns->subspaces, indent + 1);
    }

    if (ns->next) {
        show_config_tree(out, ns->next, indent);
    }
}

static void cmd_config_tree(BaseSequentialStream *chp, int argc, char **argv)
{
    parameter_namespace_t *ns;
    if (argc != 1) {
        ns = &parameter_root;
    } else {
        ns = parameter_namespace_find(&parameter_root, argv[0]);
        if (ns == NULL) {
            chprintf(chp, "Cannot find subtree.\r\n");
            return;
        }
    }

    show_config_tree(chp, ns, 0);
}

static void cmd_config_set(BaseSequentialStream *chp, int argc, char **argv)
{
    parameter_t *param;
    int value_i;

    if (argc != 2) {
        chprintf(chp, "Usage: config_set /parameter/url value.\r\n");
        return;
    }

    param = parameter_find(&parameter_root, argv[0]);

    if (param == NULL) {
        chprintf(chp, "Could not find parameter \"%s\"\r\n", argv[0]);
        return;
    }

    switch (param->type) {
        case _PARAM_TYPE_INTEGER:
            if (sscanf(argv[1], "%d", &value_i) == 1) {
                parameter_integer_set(param, value_i);
            } else {
                chprintf(chp, "Invalid value for integer parameter.\r\n");
            }
            break;

        case _PARAM_TYPE_BOOLEAN:
            if (!strcmp(argv[1], "true")) {
                parameter_boolean_set(param, true);
            } else if (!strcmp(argv[1], "false")) {
                parameter_boolean_set(param, false);
            } else {
                chprintf(chp, "Invalid value for boolean parameter, must be true or false.\r\n");
            }
            break;

        case _PARAM_TYPE_STRING:
            if (argc == 2) {
                parameter_string_set(param, argv[1]);
            } else {
                chprintf(chp, "Invalid value for string parameter, must not use spaces.\r\n");
            }
            break;

        default:
            chprintf(chp, "%s: unknown type %d\r\n", param->id, param->type);
            break;
    }
}

static void cmd_config_erase(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    (void) chp;
    extern uint8_t _config_start;

    config_erase(&_config_start);
}

static void cmd_config_save(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    extern uint8_t _config_start, _config_end;
    size_t len = (size_t)(&_config_end - &_config_start);
    bool success;

    // First write the config to flash
    config_save(&_config_start, len, &parameter_root);

    // Second try to read it back, see if we failed
    success = config_load(&parameter_root, &_config_start);

    if (success) {
        chprintf(chp, "OK.\r\n");
    } else {
        chprintf(chp, "Save failed.\r\n");
    }
}

static void cmd_config_load(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    extern uint8_t _config_start;
    bool success;

    success = config_load(&parameter_root, &_config_start);

    if (success) {
        chprintf(chp, "OK.\r\n");
    } else {
        chprintf(chp, "Load failed.\r\n");
    }
}

static void cmd_shutdown(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;

    board_shutdown();
}
static void cmd_leds(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) argc;
    (void) argv;
    (void) chp;

    char name[32];
    body_led_msg_t msg;
    messagebus_topic_t *topic;

    for (int i = 0; i < BODY_LED_COUNT; i++) {
        sprintf(name, "/body_leds/%d", i);

        topic = messagebus_find_topic(&bus, name);
        if (topic) {
            msg.value = 1.;
            messagebus_topic_publish(topic, &msg, sizeof(msg));
        } else {
            chprintf(chp, "Cannot find topic \"%s\"\r\n", name);
        }
    }
}

static void cmd_current(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    motor_current_msg_t msg;
    messagebus_topic_t *topic;

    topic = messagebus_find_topic_blocking(&bus, "/motors/current");
    messagebus_topic_wait(topic, &msg, sizeof(msg));

    chprintf(chp, "left=%.2f\r\nright=%.2f\r\n", msg.left, msg.right);
}

const ShellCommand shell_commands[] = {
    {"test", cmd_test},
    {"range", cmd_range},
    {"proximity", cmd_proximity},
    {"battery", cmd_battery},
    {"current", cmd_current},
    {"topics", cmd_topics},
    {"pwm", cmd_motor},
    {"encoders", cmd_encoders},
    {"wheel_pos", cmd_wheel_pos},
    {"imu", cmd_imu},
    {"reboot", cmd_reboot},
    {"test", cmd_test},
    {"sdcard", cmd_sdcard},
    {"config_tree", cmd_config_tree},
    {"config_set", cmd_config_set},
    {"config_save", cmd_config_save},
    {"config_load", cmd_config_load},
    {"config_erase", cmd_config_erase},
    {"shutdown", cmd_shutdown},
    {"leds", cmd_leds},

    {NULL, NULL}
};

static THD_FUNCTION(shell_spawn_thd, p)
{
    (void) p;
    thread_t *shelltp = NULL;

    static const ShellConfig shell_cfg = {
        (BaseSequentialStream *)&SDU1,
        shell_commands
    };

    shellInit();


    while (TRUE) {
        if (!shelltp) {
            if (SDU1.config->usbp->state == USB_ACTIVE) {
                shelltp = shellCreate(&shell_cfg, SHELL_WA_SIZE, NORMALPRIO);
            }
        } else {
            if (chThdTerminatedX(shelltp)) {
                chThdRelease(shelltp);
                shelltp = NULL;
            }
        }
        chThdSleepMilliseconds(500);
    }
}


void shell_start(void)
{
    static THD_WORKING_AREA(wa, SHELL_WA_SIZE);

    chThdCreateStatic(wa, sizeof(wa), NORMALPRIO, shell_spawn_thd, NULL);
}

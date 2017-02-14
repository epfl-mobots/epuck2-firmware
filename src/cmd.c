#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <hal.h>
#include <test.h>
#include <chprintf.h>
#include <shell.h>
#include <ff.h>
#include "usbconf.h"
#include "sensors/imu.h"
#include "sensors/encoder.h"
#include "sensors/range.h"
#include "config_flash_storage.h"
#include "sensors/proximity.h"
#include "sensors/battery_level.h"
#include "sensors/imu.h"
#include "sensors/motor_current.h"
#include "motor_pid_thread.h"
#include "audio/audio_thread.h"
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

static void cmd_setpoint(BaseSequentialStream *chp, int argc, char *argv[])
{
    if (argc < 3) {
        chprintf(chp, "Usage: setpoint cur|vel|pos left right\r\n");
        return;
    }

    wheels_setpoint_t setpoint;

    messagebus_topic_t *topic;
    topic = messagebus_find_topic(&bus, "/motors/setpoint");

    if (topic == NULL) {
        chprintf(chp, "Cannot find setpoint topic.\r\n");
        return;
    }

    setpoint.left = atof(argv[1]);
    setpoint.right = atof(argv[2]);

    if (!strcmp("cur", argv[0]) || !strcmp("current", argv[0])) {
        setpoint.mode = MOTOR_CONTROLLER_CURRENT;
    } else if (!strcmp("vel", argv[0]) || !strcmp("velocity", argv[0])) {
        setpoint.mode = MOTOR_CONTROLLER_VELOCITY;
    } else if (!strcmp("pos", argv[0]) || !strcmp("position", argv[0])) {
        setpoint.mode = MOTOR_CONTROLLER_POSITION;
    } else {
        chprintf(chp, "Unknown setpoint mode\r\n.");
        return;
    }

    messagebus_topic_publish(topic, &setpoint, sizeof(setpoint));
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

static void cmd_wheel_vel(BaseSequentialStream *chp, int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    messagebus_topic_t *wheel_velocities_topic;
    wheel_velocities_msg_t values;

    wheel_velocities_topic = messagebus_find_topic_blocking(&bus, "/wheel_velocities");
    messagebus_topic_wait(wheel_velocities_topic, &values, sizeof(values));

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
    chprintf(chp,
             "acc: %.2f %.2f %.2f\r\n",
             msg.acceleration[0],
             msg.acceleration[1],
             msg.acceleration[2]);
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char **argv)
{
    (void) chp;
    (void) argc;
    (void) argv;
    NVIC_SystemReset();
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

    const char *usage = "usage:\r\n"
                        "topics list -- Lists all available topics.\r\n"
                        "topics hz topic_name -- Displays the rate of the topic over a 5 second window.";

    if (argc < 1) {
        chprintf(chp, "%s\r\n", usage);
        return;
    }

    if (!strcmp(argv[0], "list")) {
        chprintf(chp, "available topics:\r\n");

        MESSAGEBUS_TOPIC_FOREACH(&bus, topic) {
            chprintf(chp, "%s\r\n", topic->name);
        }
    } else if (!strcmp(argv[0], "hz")) {
        if (argc != 2) {
            chprintf(chp, "%s\r\n", usage);
            return;
        }

        messagebus_topic_t *topic = messagebus_find_topic(&bus, argv[1]);
        if (topic == NULL) {
            chprintf(chp, "Cannot find topic \"%s\".\r\n", argv[1]);
            return;
        }

        systime_t start = chVTGetSystemTime();
        unsigned int message_counter = 0;

        while (chVTGetSystemTime() < start + MS2ST(5000)) {
            chMtxLock(topic->lock);
            if (chCondWaitTimeout(topic->condvar, MS2ST(10)) != MSG_TIMEOUT) {
                message_counter ++;
                chMtxUnlock(topic->lock);
            }
        }

        if (message_counter == 0) {
            chprintf(chp, "No messages.\r\n");
        } else {
            chprintf(chp, "Average rate: %.2f Hz\r\n", message_counter / 5.);
        }
    } else {
        chprintf(chp, "%s\r\n", usage);
        return;
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

    for (p = ns->parameter_list; p != NULL; p = p->next) {
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
                    chprintf(out, "%s: %s\r\n", p->id, parameter_boolean_get(p) ? "true" : "false");
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
    float value_f;

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

        case _PARAM_TYPE_SCALAR:
            if (sscanf(argv[1], "%f", &value_f) == 1) {
                parameter_scalar_set(param, value_f);
            } else {
                chprintf(chp, "Invalid value for scalar parameter.\r\n");
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

/* MPU test functions are marked as not optimized because GCC can detect and
 * optimize their faulty behaviour away. */
__attribute__((optimize("O0"), noinline))
static void stack_overflow(BaseSequentialStream *chp)
{
    chprintf(chp, ".\r\n");
    stack_overflow(chp);
}

__attribute__((optimize("O0"), noinline))
static void cmd_mpu_test(BaseSequentialStream *chp, int argc, char *argv[])
{
    const char *usage = "Usage: mpu_test xn|nullptr|stack";
    if (argc < 1) {
        chprintf(chp, "%s\r\n", usage);
        return;
    }

    if (!strcmp(argv[0], "xn")) {
        /* Checks if jumping to non exec memory works. */
        int a;
        void (*f)(void) = (void *)&a;
        f();
    } else if (!strcmp(argv[0], "nullptr")) {
        /* Checks if dereferencing NULL is caught */
        int *a = NULL;
        *a = 0;
    } else if (!strcmp(argv[0], "stack")) {
        /* Checks if stack overflow are caught. */
        stack_overflow(chp);
    } else {
        chprintf(chp, "%s\r\n", usage);
    }
}

static void cmd_play(BaseSequentialStream *chp, int argc, char *argv[])
{
    messagebus_topic_t *req_topic, *res_topic;
    req_topic = messagebus_find_topic(&bus, "/audio/play/request");
    res_topic = messagebus_find_topic(&bus, "/audio/play/result");

    if (req_topic == NULL || res_topic == NULL) {
        chprintf(chp, "Cannot find topics. Is audio thread running?\r\n");
        return;
    }

    if (argc != 1) {
        chprintf(chp, "Usage: play file.wav\r\n");
        return;
    }

    /* Send the request */
    audio_play_request_t request;
    memset(&request, 0, sizeof(request));
    strncpy(request.path, argv[0], sizeof(request.path) - 1);
    chprintf(chp, "Playing %s...\r\n", request.path);
    messagebus_topic_publish(req_topic, &request, sizeof(request));

    /* Wait for the reply. */
    audio_play_result_t result;
    messagebus_topic_wait(res_topic, &result, sizeof(result));

    if (result.status == AUDIO_OK) {
        chprintf(chp, "Done.\r\n");
    } else {
        chprintf(chp, "Error: %d\r\n", result.status);
    }
}

static void cmd_rec(BaseSequentialStream *chp, int argc, char *argv[])
{
    messagebus_topic_t *topic = messagebus_find_topic(&bus, "/microphone");
    if (topic == NULL) {
        chprintf(chp, "Cannot read topic, aborting.\r\n");
        return;
    }

    static FIL file;
    if (f_open(&file, "test.pcm", FA_WRITE) != FR_OK) {
        chprintf(chp, "Cannot open file for writing.\r\n");
        return;
    }

    microphone_msg_t msg;
    unsigned sample_rate = 44100;

    for(uint32_t i=0 ; i<5*sample_rate/64; i++) {
        UINT written;
        messagebus_topic_wait(topic, &msg, sizeof(msg));

        if (f_write(&file, msg.samples, 64, &written) != FR_OK) {
            chprintf(chp, "cannot write\r\n", written);
        }

    }

    f_close(&file);
}

static ShellCommand shell_commands[] = {
    {"test", cmd_test},
    {"range", cmd_range},
    {"proximity", cmd_proximity},
    {"battery", cmd_battery},
    {"current", cmd_current},
    {"topics", cmd_topics},
    {"setpoint", cmd_setpoint},
    {"encoders", cmd_encoders},
    {"wheel_pos", cmd_wheel_pos},
    {"wheel_vel", cmd_wheel_vel},
    {"imu", cmd_imu},
    {"reboot", cmd_reboot},
    {"test", cmd_test},
    {"config_tree", cmd_config_tree},
    {"config_set", cmd_config_set},
    {"config_save", cmd_config_save},
    {"config_load", cmd_config_load},
    {"config_erase", cmd_config_erase},
    {"shutdown", cmd_shutdown},
    {"leds", cmd_leds},
    {"mpu_test", cmd_mpu_test},
    {"play", cmd_play},
    {"rec", cmd_rec},

    {NULL, NULL}
};

/** Helper function used to sort the commands by name. */
static int compare_shell_commands(const void *ap, const void *bp)
{
    const ShellCommand *a = (ShellCommand *)ap;
    const ShellCommand *b = (ShellCommand *)bp;

    return strcmp(a->sc_name, b->sc_name);
}

static THD_FUNCTION(shell_spawn_thd, p)
{
    (void) p;
    thread_t *shelltp = NULL;

    static ShellConfig shell_cfg = {
        (BaseSequentialStream *)&SDU1,
        shell_commands
    };

    /* Sort the commands by name. */
    qsort(shell_commands,
          /* Ignore the last element, which is NULL. */
          (sizeof(shell_commands) / sizeof(ShellCommand)) - 1,
          sizeof(ShellCommand),
          compare_shell_commands);

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

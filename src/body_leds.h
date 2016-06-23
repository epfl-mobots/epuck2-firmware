#ifndef BODY_LEDS_H
#define BODY_LEDS_H

#ifdef __cplusplus
extern "C" {
#endif

#define BODY_LED_COUNT 18

typedef struct {
    float value; /** Led value, between 0 and 1. */
} body_led_msg_t;

void body_leds_start(void);

#ifdef __cplusplus
}
#endif

#endif

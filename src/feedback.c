#include "feedback.h"
#include "sensors/encoder.h"
#include "analogic.h"
#include "ch.h"
#include "hal.h"

#define CONVERSION_CURRENT 0.0045
#define CONVERSION_VELOCITY  519.34// rads per secs
#define CONVERSION_POSITION 0.0029499 // position in rad


void feedback_get(feedback *left, feedback *right)
{
    uint32_t buff_left = 0;
    uint32_t buff_right = 0;

    float delta_left = 0;
    float delta_right = 0;

    buff_left = encoder_get_left();
    buff_right = encoder_get_right();

    delta_left = CONVERSION_POSITION * ((float) encoder_tick_diff(left->previous_tick, buff_left));
    delta_right = CONVERSION_POSITION *
                  ((float) encoder_tick_diff(right->previous_tick, buff_right));

    left->position += delta_left;
    right->position += delta_right;

    left->velocity = CONVERSION_VELOCITY * delta_left;
    right->velocity = CONVERSION_VELOCITY * delta_right;

    left->previous_tick = buff_left;
    right->previous_tick = buff_right;


    int32_t motor_current[2];

    analog_get_motor(motor_current);
    left->current = CONVERSION_CURRENT * ((float) motor_current[0]);
    right->current = CONVERSION_CURRENT * ((float) motor_current[1]);


}

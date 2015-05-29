#include "feedback.h"
#include "sensors/encoder.h"
#include "analogic.h"
#include "ch.h"
#include "hal.h"



void feedback_get(feedback *left, feedback *right)
{
	int buff_left = 0;
	int buff_right = 0;

	buff_left = encoder_get_left();
	buff_right = encoder_get_right();

	left->velocity = (float) encoder_tick_diff((int)left->position, buff_left);
	right->velocity = (float) encoder_tick_diff((int)right->position, buff_right);

	left->position += left->velocity;
	right->position += right->velocity;

	int32_t motor_current[2];

	analog_get_motor(motor_current);
	left->current = (float) motor_current[0];
	right->current = (float) motor_current[1];
}
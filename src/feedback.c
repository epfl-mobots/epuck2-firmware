#include "feedback.h"
#include "ch.h"
#include "hal.h"


void feedback_get (feedback *left, feedback *right)
{
	int buff_left = 0;
	int buff_right = 0;

	buff_left = encoder_get_left();
	buff_right = encoder_get_right();

	left->velocity = (float) encoder_tick_diff(left->position, buff_left);
	right->velocity = (float) encoder_tick_diff(right->position, buff_right);

	left->position = (float) encoder_get_left();
	right->position = (float) encoder_get_right();

	left->current = 0;
	right->current = 0;
}
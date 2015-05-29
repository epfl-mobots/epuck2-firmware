#include "ch.h"
#include "hal.h"
#include "setpoints.h"

void setpoints_update(setpoints *left, setpoints *right, float encoder_speed_left, float encoder_speed_right)
{
	left->position += encoder_speed_left;
	right->position += encoder_speed_right;

	left->velocity = 0;
	right->velocity = 0;

	left->current = 0;
	right->current = 0;
}

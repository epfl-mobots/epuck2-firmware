#include "ch.h"
#include "hal.h"
#include "setpoints.h"

void setpoints_init(setpoints *left, setpoints *right)
{
	left->position = 0;
	right->position = 0;

	left->velocity = 0;
	right->velocity = 0;

	left->current = 0;
	right->current = 0;
}

void setpoints_set_position(setpoints *setpoint, float position)
{
	setpoint->position = position;	
}

void setpoints_set_velocity(setpoints *setpoint, float velocity)
{
	setpoint->velocity = velocity;
}

void setpoints_set_current(setpoints *setpoint, float current)
{
	setpoint->current = current;	
}


float setpoints_error_position(setpoints *setpoint, float* position)
{
	float error = 0;
	if (setpoint->position == *position) {
		setpoint->position = 0;
		*position = 0;
	}
	error = setpoint->position - *position;
	return error;	
}

float setpoints_error_velocity(setpoints *setpoint, float velocity)
{
	float error = 0;
	error = setpoint->velocity - velocity;
	return error;
}

float setpoints_error_current(setpoints *setpoint, float current)
{
	float error = 0;
	error = setpoint->current - current;
	return error;	
}
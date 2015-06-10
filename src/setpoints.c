#include "ch.h"
#include "hal.h"
#include "setpoints.h"

void setpoints_init(setpoints *setpoints)
{
	setpoints->position = 0;
	setpoints->velocity = 0;
	setpoints->current = 0;
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
	error = velocity - setpoint->velocity;
	return error;
}

float setpoints_error_current(setpoints *setpoint, float current)
{
	float error = 0;
	error = current - setpoint->current;
	return error;	
}
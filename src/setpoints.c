#include "ch.h"
#include "hal.h"
#include "setpoints.h"

void setpoints_init(setpoints *setpoints)
{
	setpoints->position = 0;
	setpoints->velocity = 0;
	setpoints->current = 0;
	setpoints->setpt_mode = SETPT_MODE_CUR;
}

void setpoints_set_position(setpoints *setpoint, float position)
{
	setpoint->position = position;
	setpoint->setpt_mode = SETPT_MODE_POS;
}

void setpoints_set_velocity(setpoints *setpoint, float velocity)
{
	setpoint->velocity = velocity;
	setpoint->setpt_mode = SETPT_MODE_VEL;
}

void setpoints_set_current(setpoints *setpoint, float current)
{
	setpoint->current = current;
	setpoint->setpt_mode = SETPT_MODE_CUR;
}


float setpoints_error_position(setpoints *setpoint, float position)
{
	float error = 0;
	error = position - setpoint->position;
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

int setpoint_get_mode(setpoints *setpoint)
{
	return setpoint->setpt_mode;
}

#ifndef SETPOINTS_H
#define SETPOINTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

typedef struct setpoints {
	float position;
	float velocity;
	float current;

	float max_position;
	float max_velocity;
	float max_current;
}setpoints;

void setpoints_init(setpoints *left, setpoints *right);
void setpoints_set_position(setpoints *motor, float position);
void setpoints_set_velocity(setpoints *motor, float velocity);
void setpoints_set_current(setpoints *motor, float current);

float setpoints_error_position(setpoints *setpoint, float* position);
float setpoints_error_velocity(setpoints *setpoint, float velocity);
float setpoints_error_current(setpoints *setpoint, float current);


#ifdef __cplusplus
}
#endif

#endif /* SETPOINTS_H */
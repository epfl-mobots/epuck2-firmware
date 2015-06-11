#ifndef SETPOINTS_H
#define SETPOINTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#define SETPT_MODE_POS      0
#define SETPT_MODE_VEL      1
#define SETPT_MODE_CUR      2

typedef struct setpoints {
	float position;
	float velocity;
	float current;

	float max_position;
	float max_velocity;
	float max_current;

    int setpt_mode;
}setpoints;

void setpoints_init(setpoints *setpoints);
void setpoints_set_position(setpoints *motor, float position);
void setpoints_set_velocity(setpoints *motor, float velocity);
void setpoints_set_current(setpoints *motor, float current);

float setpoints_error_position(setpoints *setpoint, float position);
float setpoints_error_velocity(setpoints *setpoint, float velocity);
float setpoints_error_current(setpoints *setpoint, float current);

int setpoint_get_mode(setpoints *setpoint);

#ifdef __cplusplus
}
#endif

#endif /* SETPOINTS_H */
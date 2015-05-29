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

void setpoints_update(setpoints *left, setpoints *right, float encoder_speed_left, float encoder_speed_right);



#ifdef __cplusplus
}
#endif

#endif /* SETPOINTS_H */
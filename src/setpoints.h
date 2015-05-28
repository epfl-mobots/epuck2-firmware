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

void setpoints_get(setpoints *left, setpoints *right);


#ifdef __cplusplus
}
#endif

#endif /* SETPOINTS_H */
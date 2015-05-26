#ifndef FEEDBACK_H
#define FEEDBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

struct feedback {
	float position;
	float velocity;
	float current;
};

void feedback_get (struct feedback left, struct feedback right);


#ifdef __cplusplus
}
#endif

#endif /* FEEDBACK_H */
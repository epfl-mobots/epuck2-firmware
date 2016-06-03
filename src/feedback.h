#ifndef FEEDBACK_H
#define FEEDBACK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct feedback {
    float position;
    float velocity;
    float current;
    int32_t previous_tick;
} feedback;

void feedback_get(feedback *left, feedback *right);


#ifdef __cplusplus
}
#endif

#endif /* FEEDBACK_H */

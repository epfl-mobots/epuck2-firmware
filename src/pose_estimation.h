#ifndef POSE_ESTIMATION_H
#define POSE_ESTIMATION_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float x;
    float y;
    float heading;
    float vel_x;
    float vel_y;
    float vel_heading;
    float right_wheel_tick_to_meter;
    float left_wheel_tick_to_meter;
    float wheelbase;
} pose_estim_base_t;

void pose_estim_init(pose_estim_base_t *est);
void pose_estim_update(pose_estim_base_t *est,
                       uint32_t encoder_right,
                       uint32_t encoder_left,
                       float delta_t);

#ifdef __cplusplus
}
#endif

#endif /* POSE_ESTIMATION_H */

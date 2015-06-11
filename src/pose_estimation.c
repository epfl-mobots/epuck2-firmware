#include "math.h"
#include "pose_estimation.h"
#include "sensors/encoder.h"

#define WHEEL_RADIUS_RIGHT  0.075f
#define WHEEL_RADIUS_LEFT   0.075f
#define WHEELBASE           0.0725f
#define TICKS_PER_TURN      2630

static uint32_t enc_right[2];
static uint32_t enc_left[2];

void pose_estim_init(pose_estim_base_t *est)
{
    // Initialise encoder samples
    enc_right[0] = 0;
    enc_right[1] = 0;
    enc_left[0] = 0;
    enc_left[1] = 0;

    // Initialise pose
    est->x = 0.f;
    est->y = 0.f;
    est->heading = 0.f;
    est->speed_forward = 0.f;
    est->speed_rotation = 0.f;

    // Set
    est->right_wheel_tick_to_meter = (2 * M_PI * WHEEL_RADIUS_RIGHT / TICKS_PER_TURN);
    est->left_wheel_tick_to_meter = (2 * M_PI * WHEEL_RADIUS_LEFT / TICKS_PER_TURN);
    est->wheelbase = WHEELBASE;
}

void pose_estim_update(pose_estim_base_t *est,
                       uint32_t encoder_right,
                       uint32_t encoder_left,
                       float delta_t)
{
    enc_right[0] = enc_right[1];
    enc_left[0] = enc_left[1];
    enc_right[1] = encoder_right;
    enc_left[1] = encoder_left;

    float delta_right = encoder_tick_diff(enc_right[0], enc_right[1])
                        * est->right_wheel_tick_to_meter;
    float delta_left = encoder_tick_diff(enc_left[0], enc_left[1])
                       * est->left_wheel_tick_to_meter;

    float delta_fwd = 0.5f * (delta_right + delta_left);
    float delta_rot = (delta_right - delta_left) / est->wheelbase;

    float cos_theta = cosf(est->heading + 0.5f * delta_rot);
    float sin_theta = sinf(est->heading + 0.5f * delta_rot);

    est->speed_forward = delta_fwd / delta_t;
    est->speed_rotation = delta_rot / delta_t;

    est->x += delta_fwd * cos_theta;
    est->y += delta_fwd * sin_theta;
    est->heading += delta_rot;
}

float pose_estim_get_forward_speed(pose_estim_base_t *est)
{
    return est->speed_forward;
}

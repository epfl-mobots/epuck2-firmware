#ifndef ENCODER_H
#define ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    int32_t left;
    int32_t right;
} encoders_msg_t;

typedef struct {
    float left;
    float right;
} wheel_pos_msg_t;

typedef struct {
    float left;
    float right;
} wheel_velocities_msg_t;

/*Encoder uses Timer 3 & 4*/
void encoder_start(void);
uint32_t encoder_get_right(void);
uint32_t encoder_get_left(void);

/**
 * For two encoder values, returns the minimal signed difference
 * while considering an overflow or underflow.
 */
int encoder_tick_diff(uint32_t enc_old, uint32_t enc_new);



#ifdef __cplusplus
}
#endif

#endif /* ENCODER_H */

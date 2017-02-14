#ifndef MOTOR_CURRENT_H
#define MOTOR_CURRENT_H

#ifdef __cplusplus
extern "C" {
#endif

/** Structure holding a current measurement message. All values are in amperes. */
typedef struct {
    float left;
    float right;
} motor_current_msg_t;

typedef struct {
    adcsample_t samples[128];
} microphone_msg_t;

/** Starts the motor current acquisition. */
void motor_current_start(void);

#ifdef __cplusplus
}
#endif

#endif

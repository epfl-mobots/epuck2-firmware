#ifndef ATTITUDE_ESTIMATION_H
#define ATTITUDE_ESTIMATION_H

#include "parameter/parameter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float theta;
    parameter_t compl_flt_gain_param;
    float compl_flt_gain;
} att_estim_t;

void att_estim_init(att_estim_t *est, parameter_namespace_t *ns);
void att_estim_update(att_estim_t *est, float gyro, float acc, float delta_t);
float att_estim_get_theta(att_estim_t *est);

#ifdef __cplusplus
}
#endif

#endif /* ATTITUDE_ESTIMATION_H */
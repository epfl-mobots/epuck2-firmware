#include "attitude_estimation.h"

#define GRAVITY 9.81f

void att_estim_init(att_estim_t *est, parameter_namespace_t *ns)
{
    est->theta = 0;
    est->compl_flt_gain = 0;
    parameter_scalar_declare(&est->compl_flt_gain_param, ns, "compl_gain");
}

void att_estim_update(att_estim_t *est, float gyro, float acc, float delta_t)
{
    if (parameter_changed(&est->compl_flt_gain_param)) {
        est->compl_flt_gain = parameter_scalar_get(&est->compl_flt_gain_param);
    }
    est->theta = est->compl_flt_gain * (est->theta + gyro * delta_t)
                + (1 - est->compl_flt_gain) * (acc / GRAVITY);
}

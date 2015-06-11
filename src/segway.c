#include "parameter/parameter.h"
#include "communication.h"
#include "attitude_estimation.h"
#include "sensors/imu.h"
#include "control.h"

#include "segway.h"

#define ACC_AXIS 0
#define GYRO_AXIS 1

#define SEGWAY_CONTROL_FREQ 100

static parameter_namespace_t segway_ns;
att_estim_t segway_att_estim;
pid_ctrl_t advance_ctrl;  // controls the advance (forward/reverse) speed of the segway
pid_ctrl_t attitude_ctrl; // controls the attitude of the segway
struct pid_parameter_s advance_ctrl_param;
struct pid_parameter_s attitude_ctrl_param;


static THD_WORKING_AREA(segway_thd_wa, 512);
static THD_FUNCTION(segway_thd, arg)
{
    (void)arg;
    static float acc[3];
    static float gyro[3];
    float delta_t = 1.f/SEGWAY_CONTROL_FREQ;

    while (1) {
        imu_get_gyro(gyro);
        imu_get_acc(acc);

        att_estim_update(&segway_att_estim, gyro[GYRO_AXIS], acc[ACC_AXIS], delta_t);

        update_pid_parameters(&advance_ctrl, &advance_ctrl_param);
        update_pid_parameters(&attitude_ctrl, &attitude_ctrl_param);

        float speed_setpt = 0; // todo
        float speed_meas = 0; // todo

        float attitude_setpt = pid_process(&advance_ctrl, speed_meas - speed_setpt);
        float attitude_meas = att_estim_get_theta(&segway_att_estim);

        float wheel_speed_fwd = pid_process(&attitude_ctrl, attitude_meas - attitude_setpt);

        chThdSleepMilliseconds(1000/SEGWAY_CONTROL_FREQ);
    }
    return 0;
}


void segway_control_init(void)
{
    pid_init(&advance_ctrl);
    pid_init(&attitude_ctrl);
    parameter_namespace_declare(&segway_ns, &parameter_root, "segway");
    pid_register(&advance_ctrl_param, &segway_ns, "advance_pid");
    pid_register(&attitude_ctrl_param, &segway_ns, "attitude_pid");
    att_estim_init(&segway_att_estim, &segway_ns);
}


void segway_control_start(void)
{
    chThdCreateStatic(segway_thd_wa, sizeof(segway_thd_wa), NORMALPRIO, segway_thd, NULL);
}

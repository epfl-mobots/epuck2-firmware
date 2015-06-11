#include "parameter/parameter.h"
#include "communication.h"
#include "attitude_estimation.h"
#include "sensors/imu.h"

#include "segway.h"

#define ACC_AXIS 0
#define GYRO_AXIS 0

#define SEGWAY_CONTROL_FREQ 100

static parameter_namespace_t segway_ns;
static att_estim_t att_estim;


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

        att_estim_update(&att_estim, gyro[GYRO_AXIS], acc[ACC_AXIS], delta_t);

        chThdSleepMilliseconds(1000/SEGWAY_CONTROL_FREQ);
    }
    return 0;
}


void segway_control_init(void)
{
    parameter_namespace_declare(&segway_ns, &parameter_root, "segway");
    att_estim_init(&att_estim, &segway_ns);
}


void segway_control_start(void)
{
    chThdCreateStatic(segway_thd_wa, sizeof(segway_thd_wa), NORMALPRIO, segway_thd, NULL);
}

#include <ch.h>
#include <string.h>
#include "cmp_mem_access/cmp_mem_access.h"
#include "serial-datagram/serial_datagram.h"
#include "sensors/imu.h"

static int send_imu(cmp_ctx_t *cmp)
{
    float gyro[3];
    float acc[3];
    float t = (float)chVTGetSystemTimeX() / CH_CFG_ST_FREQUENCY;
    imu_get_gyro(gyro);
    imu_get_acc(acc);
    bool err = false;
    err = err || !cmp_write_map(cmp, 3);
    const char *gyro_id = "gyro";
    err = err || !cmp_write_str(cmp, gyro_id, strlen(gyro_id));
    err = err || !cmp_write_array(cmp, 3);
    err = err || !cmp_write_float(cmp, gyro[0]);
    err = err || !cmp_write_float(cmp, gyro[1]);
    err = err || !cmp_write_float(cmp, gyro[2]);
    const char *acc_id = "acc";
    err = err || !cmp_write_str(cmp, acc_id, strlen(acc_id));
    err = err || !cmp_write_array(cmp, 3);
    err = err || !cmp_write_float(cmp, acc[0]);
    err = err || !cmp_write_float(cmp, acc[1]);
    err = err || !cmp_write_float(cmp, acc[2]);
    const char *time_id = "time";
    err = err || !cmp_write_str(cmp, time_id, strlen(time_id));
    err = err || !cmp_write_float(cmp, t);
    return err;
}



static void _stream_imu_values_sndfn(void *arg, const void *p, size_t len)
{
    if (len > 0) {
        chSequentialStreamWrite((BaseSequentialStream*)arg, (const uint8_t*)p, len);
    }
}



static THD_WORKING_AREA(periodic_comm_wa, 512);
static THD_FUNCTION(periodic_comm, arg)
{
    BaseSequentialStream *out = (BaseSequentialStream*)arg;
    chRegSetThreadName("periodic-comm");

    static char dtgrm[100];
    static cmp_mem_access_t mem;
    static cmp_ctx_t cmp;
    while (1) {

        cmp_mem_access_init(&cmp, &mem, dtgrm, sizeof(dtgrm));
        if (send_imu(&cmp) == 0) {
            serial_datagram_send(dtgrm, cmp_mem_access_get_pos(&mem), _stream_imu_values_sndfn, out);
        }

    }
    return 0;
}



void communication_start(BaseSequentialStream *out)
{
    chThdCreateStatic(periodic_comm_wa, sizeof(periodic_comm_wa), LOWPRIO, periodic_comm, out);
}

#include <hal.h>
#include <ch.h>
#include <string.h>
#include "cmp_mem_access/cmp_mem_access.h"
#include "serial-datagram/serial_datagram.h"
#include "datagram_dispatcher.h"
#include "sensors/imu.h"
#include "parameter/parameter_msgpack.h"
#include "analogic.h"
#include "setpoints.h"
#include "control.h"
#include "sensors/encoder.h"



 
parameter_namespace_t parameter_root;

static mutex_t send_lock;

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

static int send_current(cmp_ctx_t *cmp)
{
    int32_t motor_current[2];
    float t = (float)chVTGetSystemTimeX() / CH_CFG_ST_FREQUENCY;

    analog_get_motor(motor_current);

    bool err = false;

    err = err || !cmp_write_map(cmp, 2);
    const char *current_id = "current";
    err = err || !cmp_write_str(cmp, current_id, strlen(current_id));
    err = err || !cmp_write_array(cmp, 2);
    err = err || !cmp_write_float(cmp, motor_current[0]);
    err = err || !cmp_write_float(cmp, motor_current[1]);
    const char *time_id = "time";
    err = err || !cmp_write_str(cmp, time_id, strlen(time_id));
    err = err || !cmp_write_float(cmp, t);
    return err;
}

static int send_velocity(cmp_ctx_t *cmp)
{
    float t = (float)chVTGetSystemTimeX() / CH_CFG_ST_FREQUENCY;

    bool err = false;

    err = err || !cmp_write_map(cmp, 2);
    const char *velocity_id = "velocity";
    err = err || !cmp_write_str(cmp, velocity_id, strlen(velocity_id));
    err = err || !cmp_write_array(cmp, 2);
    err = err || !cmp_write_float(cmp, left.feedback.velocity);
    err = err || !cmp_write_float(cmp, right.feedback.velocity);
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


static THD_WORKING_AREA(comm_tx_stream_wa, 1024);
static THD_FUNCTION(comm_tx_stream, arg)
{
    BaseSequentialStream *out = (BaseSequentialStream*)arg;

    static char dtgrm[100];
    static cmp_mem_access_t mem;
    static cmp_ctx_t cmp;
    while (1) {

        cmp_mem_access_init(&cmp, &mem, dtgrm, sizeof(dtgrm));
        if (send_imu(&cmp) == 0) {
            chMtxLock(&send_lock);
            serial_datagram_send(dtgrm, cmp_mem_access_get_pos(&mem), _stream_imu_values_sndfn, out);
            chMtxUnlock(&send_lock);
        }

        cmp_mem_access_init(&cmp, &mem, dtgrm, sizeof(dtgrm));
        if (send_current(&cmp) == 0) {
            chMtxLock(&send_lock);
            serial_datagram_send(dtgrm, cmp_mem_access_get_pos(&mem), _stream_imu_values_sndfn, out);
            chMtxUnlock(&send_lock);
        }

        cmp_mem_access_init(&cmp, &mem, dtgrm, sizeof(dtgrm));
        if (send_velocity(&cmp) == 0) {
            chMtxLock(&send_lock);
            serial_datagram_send(dtgrm, cmp_mem_access_get_pos(&mem), _stream_imu_values_sndfn, out);
            chMtxUnlock(&send_lock);
        }
    }
    return 0;
}



static char reply_buf[100];
static cmp_mem_access_t reply_mem;
static cmp_ctx_t reply_cmp;

int ping_cb(cmp_ctx_t *cmp, void *arg)
{
    (void)cmp;
    if (!cmp_read_nil(cmp)) {
        return -1;
    }
    cmp_mem_access_init(&reply_cmp, &reply_mem, reply_buf, sizeof(reply_buf));
    const char *ping_resp = "ping";
    if (cmp_write_str(cmp, ping_resp, strlen(ping_resp))) {
        chMtxLock(&send_lock);
        serial_datagram_send(reply_buf, cmp_mem_access_get_pos(&reply_mem), _stream_imu_values_sndfn, arg);
        chMtxUnlock(&send_lock);
    }
    return 0;
}



int parameter_set_cb(cmp_ctx_t *cmp, void *arg)
{
    (void)arg;
    int ret = parameter_msgpack_read_cmp(&parameter_root, cmp, NULL, NULL);
    return ret;
}

int velocity_cb(cmp_ctx_t *cmp, void *arg)
{
    (void)arg;
    uint32_t size;
    float velocity;
    bool motor;

    if(cmp_read_array(cmp, &size) && size == 2 && cmp_read_bool(cmp, &motor) && cmp_read_float(cmp, &velocity)) {
        if(!motor) {
            setpoints_set_velocity(&(left.setpoints),velocity);
        }
        else {
            setpoints_set_velocity(&(right.setpoints),velocity);
        }
        return 0;
    }
    return -1;
}

int position_cb(cmp_ctx_t *cmp, void *arg)
{
    (void)arg;
    uint32_t size;
    float position;
    bool motor;

    if(cmp_read_array(cmp, &size) && size == 2 && cmp_read_bool(cmp, &motor) && cmp_read_float(cmp, &position)) {
        if(!motor) {
            setpoints_set_position(&(left.setpoints),position);
        }
        else {
            setpoints_set_position(&(right.setpoints),position);
        }
        return 0;
    }
    return -1;
}

int current_cb(cmp_ctx_t *cmp, void *arg)
{
    (void)arg;
    uint32_t size;
    float current;
    bool motor;

    if(cmp_read_array(cmp, &size) && size == 2 && cmp_read_bool(cmp, &motor) && cmp_read_float(cmp, &current)) {
        if(!motor) {
            setpoints_set_current(&(left.setpoints),current);
        }
        else {
            setpoints_set_current(&(right.setpoints),current);
        }
        return 0;
    }
    return -1;
}


static THD_WORKING_AREA(comm_rx_wa, 1024);
static THD_FUNCTION(comm_rx, arg)
{
    struct dispatcher_entry_s dispatcher_table[] = {
        {"ping", ping_cb, arg},
        {"parameter_set", parameter_set_cb, NULL},
        {"position_set", position_cb, NULL},
        {"velocity_set", velocity_cb, NULL},
        {"current_set", current_cb, NULL},
        {NULL, NULL, NULL}
    };
    static serial_datagram_rcv_handler_t rcv_handler;
    static char rcv_buffer[2000];

    chRegSetThreadName("comm rx");

    BaseSequentialStream *in = (BaseSequentialStream*)arg;
    serial_datagram_rcv_handler_init(&rcv_handler,rcv_buffer,
            sizeof(rcv_buffer), datagram_dispatcher_cb, dispatcher_table);
    while (1) {
        char c = chSequentialStreamGet(in);
        serial_datagram_receive(&rcv_handler, &c, 1);
    }
    return 0;
}


void communication_start(BaseSequentialStream *out)
{
    parameter_namespace_declare(&parameter_root, NULL, NULL);
    chMtxObjectInit(&send_lock);
    chThdCreateStatic(comm_tx_stream_wa, sizeof(comm_tx_stream_wa), LOWPRIO, comm_tx_stream, out);
    chThdCreateStatic(comm_rx_wa, sizeof(comm_rx_wa), LOWPRIO, comm_rx, out);
}

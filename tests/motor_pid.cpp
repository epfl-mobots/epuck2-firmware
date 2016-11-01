#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "parameter/parameter.h"
#include "motor_pid.h"
#include "pid/pid.h"

/* Private functions, not hidden by static because used in testing */
extern "C" {
void pid_param_update(struct pid_param_s *p, pid_ctrl_t *ctrl);
}

TEST_GROUP(PIDConfigTestGroup)
{
    parameter_namespace_t ns;
    motor_pid_t motor_pid;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_pid_init(&motor_pid, &ns);
    }
};

TEST(PIDConfigTestGroup, CanConfigure)
{

    /* Check that the parameters were all correctly declared. */
    CHECK_TRUE(parameter_find(&ns, "/control/velocity_limit"));
    CHECK_TRUE(parameter_find(&ns, "/control/torque_limit"));
    CHECK_TRUE(parameter_find(&ns, "/control/acceleration_limit"));

    CHECK_TRUE(parameter_find(&ns, "/control/position/kp"));
    CHECK_TRUE(parameter_find(&ns, "/control/position/ki"));
    CHECK_TRUE(parameter_find(&ns, "/control/position/kd"));
    CHECK_TRUE(parameter_find(&ns, "/control/position/i_limit"));

    CHECK_TRUE(parameter_find(&ns, "/control/velocity/kp"));
    CHECK_TRUE(parameter_find(&ns, "/control/velocity/ki"));
    CHECK_TRUE(parameter_find(&ns, "/control/velocity/kd"));
    CHECK_TRUE(parameter_find(&ns, "/control/velocity/i_limit"));

    CHECK_TRUE(parameter_find(&ns, "/control/current/kp"));
    CHECK_TRUE(parameter_find(&ns, "/control/current/ki"));
    CHECK_TRUE(parameter_find(&ns, "/control/current/kd"));
    CHECK_TRUE(parameter_find(&ns, "/control/current/i_limit"));
}

TEST(PIDConfigTestGroup, CanChangeGains)
{
    pid_ctrl_t pid;

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    pid_param_update(&motor_pid.params_vel_pid, &pid);

    CHECK_EQUAL(12, pid.kp);
}

TEST(PIDConfigTestGroup, ChangingGainResetsIntegrator)
{
    pid_ctrl_t pid;

    pid.integrator = 100;

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    pid_param_update(&motor_pid.params_vel_pid, &pid);

    CHECK_EQUAL(0, pid.integrator);
}

static float mock_get_current(void *param)
{
    (void) param;
    return mock().actualCall("get_current").returnDoubleValue();
}

static float mock_get_speed(void *param)
{
    (void) param;
    return mock().actualCall("get_speed").returnDoubleValue();
}

static float mock_get_pos(void *param)
{
    (void) param;
    return mock().actualCall("get_pos").returnDoubleValue();
}

TEST_GROUP(MotorMockTestGroup)
{
};

TEST(MotorMockTestGroup, MockCurrentWorks)
{
    mock().expectOneCall("get_current").andReturnValue(12.);

    auto current = mock_get_current(NULL);
    CHECK_EQUAL(12, current);
}

TEST(MotorMockTestGroup, MockSpeedWorks)
{
    mock().expectOneCall("get_speed").andReturnValue(12.);

    auto speed = mock_get_speed(NULL);
    CHECK_EQUAL(12, speed);
}

TEST(MotorMockTestGroup, MockPositionWorks)
{
    mock().expectOneCall("get_pos").andReturnValue(12.);

    auto pos = mock_get_pos(NULL);
    CHECK_EQUAL(12, pos);
}

TEST_GROUP(ProcessReconfigures)
{
    motor_pid_t pid;
    parameter_namespace_t ns;
    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_pid_init(&pid, &ns);
    }
};

TEST(ProcessReconfigures, ProcessUpdatesVelocityParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);
    motor_pid_process(&pid);
    CHECK_EQUAL(12, pid.vel_pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesPositionParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 12);
    motor_pid_process(&pid);
    CHECK_EQUAL(12, pid.pos_pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesCurrentParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 12);
    motor_pid_process(&pid);
    CHECK_EQUAL(12, pid.cur_pid.kp);
}

TEST_GROUP(Process)
{
    motor_pid_t pid;
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_pid_init(&pid, &ns);
    }
};

TEST(Process, CurrentControl)
{
    pid.callbacks.get_current.fn = mock_get_current;
    pid.callbacks.get_current.arg = NULL;

    pid.mode = motor_pid_t::MOTOR_PID_CURRENT_CONTROL;

    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 10);
    mock().expectOneCall("get_current").andReturnValue(1.);
    pid.cur_setpoint = 2;

    auto voltage = motor_pid_process(&pid);

    CHECK_EQUAL(10, voltage);
}

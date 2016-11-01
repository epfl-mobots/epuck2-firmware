#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "parameter/parameter.h"
#include "motor_controller.h"
#include "pid/pid.h"

/* Private functions, not hidden by static because used in testing */
extern "C" {
void pid_param_update(struct pid_param_s *p, pid_ctrl_t *ctrl);
}

TEST_GROUP(PIDConfigTestGroup)
{
    parameter_namespace_t ns;
    motor_controller_t controller;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);
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

    pid_param_update(&controller.params_vel_pid, &pid);

    CHECK_EQUAL(12, pid.kp);
}

TEST(PIDConfigTestGroup, ChangingGainResetsIntegrator)
{
    pid_ctrl_t pid;

    pid.integrator = 100;

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    pid_param_update(&controller.params_vel_pid, &pid);

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
    motor_controller_t controller;
    parameter_namespace_t ns;
    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);
    }
};

TEST(ProcessReconfigures, ProcessUpdatesVelocityParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);
    motor_controller_process(&controller);
    CHECK_EQUAL(12, controller.vel_pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesPositionParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 12);
    motor_controller_process(&controller);
    CHECK_EQUAL(12, controller.pos_pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesCurrentParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 12);
    motor_controller_process(&controller);
    CHECK_EQUAL(12, controller.cur_pid.kp);
}

TEST_GROUP(Process)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);
    }
};

TEST(Process, CurrentControl)
{
    controller.callbacks.get_current.fn = mock_get_current;
    controller.callbacks.get_current.arg = NULL;

    controller.mode = motor_controller_t::MOTOR_CONTROLLER_CURRENT;

    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 10);
    mock().expectOneCall("get_current").andReturnValue(1.);
    controller.cur_setpoint = 2;

    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(10, voltage);
}

TEST(Process, VelocityControl)
{
    controller.callbacks.get_velocity.fn = mock_get_speed;
    controller.callbacks.get_velocity.arg = NULL;
    controller.mode = motor_controller_t::MOTOR_CONTROLLER_VELOCITY;

    controller.vel_setpoint = 2.;
    mock().expectOneCall("get_speed").andReturnValue(1.);

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 20);
    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 1);
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(20, voltage);
}

TEST(Process, PositionControl)
{
    controller.callbacks.get_position.fn = mock_get_pos;
    controller.callbacks.get_position.arg = NULL;
    controller.mode = motor_controller_t::MOTOR_CONTROLLER_POSITION;

    controller.pos_setpoint = 2.;
    mock().expectOneCall("get_pos").andReturnValue(1.);

    parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 30);
    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 1);
    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 1);

    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(30, voltage);
}

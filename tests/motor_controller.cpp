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
    CHECK_TRUE(parameter_find(&ns, "/control/limits/velocity"));
    CHECK_TRUE(parameter_find(&ns, "/control/limits/current"));
    CHECK_TRUE(parameter_find(&ns, "/control/limits/acceleration"));

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

    pid_param_update(&controller.velocity.params, &pid);

    CHECK_EQUAL(12, pid.kp);
}

TEST(PIDConfigTestGroup, ChangingGainResetsIntegrator)
{
    pid_ctrl_t pid;

    pid.integrator = 100;

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    pid_param_update(&controller.velocity.params, &pid);

    CHECK_EQUAL(0, pid.integrator);
}

TEST(PIDConfigTestGroup, CanSetFrequency)
{
    motor_controller_set_frequency(&controller, 100);
    CHECK_EQUAL(100, controller.position.pid.frequency);
    CHECK_EQUAL(100, controller.velocity.pid.frequency);
    CHECK_EQUAL(100, controller.current.pid.frequency);
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
    CHECK_EQUAL(12, controller.velocity.pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesPositionParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 12);
    motor_controller_process(&controller);
    CHECK_EQUAL(12, controller.position.pid.kp);
}

TEST(ProcessReconfigures, ProcessUpdatesCurrentParameters)
{
    parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 12);
    motor_controller_process(&controller);
    CHECK_EQUAL(12, controller.current.pid.kp);
}

TEST_GROUP(CurrentControl)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void mock_set_current(float current)
    {
        mock().expectOneCall("get_current").andReturnValue(current);
    }

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);

        controller.current.get = mock_get_current;
        controller.current.get_arg = NULL;

        controller.mode = motor_controller_t::MOTOR_CONTROLLER_CURRENT;
        parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 10);
    }
};

TEST(CurrentControl, ErrorIsProcessed)
{
    controller.current.setpoint = 2;
    mock_set_current(1.);
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(10, voltage);
}

TEST(CurrentControl, ErrorIsSet)
{
    controller.current.setpoint = 2;
    mock_set_current(1.);
    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.current.error);
}

TEST(CurrentControl, LimitIsEnforced)
{
    parameter_scalar_set(parameter_find(&ns, "/control/limits/current"), 1.5);
    controller.current.setpoint = 2;
    mock_set_current(1.);
    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(5, voltage);
}

TEST(CurrentControl, NegativeValuesAreClampedToo)
{
    parameter_scalar_set(parameter_find(&ns, "/control/limits/current"), 1.5);
    controller.current.setpoint = -2;
    mock_set_current(-1.);
    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(-5, voltage);

}

TEST_GROUP(VelocityControl)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void mock_set_speed(float speed)
    {
        mock().expectOneCall("get_speed").andReturnValue(speed);
    }

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);

        parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 20);
        parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 1);

        controller.velocity.get = mock_get_speed;
        controller.velocity.get_arg = NULL;
        controller.mode = motor_controller_t::MOTOR_CONTROLLER_VELOCITY;

    }
};

TEST(VelocityControl, ErrorIsProcessed)
{
    mock_set_speed(1.);
    controller.velocity.setpoint = 2.;
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(20, voltage);
}

TEST(VelocityControl, ErrorIsSet)
{
    mock_set_speed(1.);
    controller.velocity.setpoint = 2.;

    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.velocity.error);
}

TEST(VelocityControl, MaxVelocityIsEnforced)
{
    mock_set_speed(1.);
    controller.velocity.setpoint = 2.;
    parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 1.5);

    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(10, voltage);
}

TEST(VelocityControl, NegativeVelocitiesAreCappedToo)
{
    mock_set_speed(-1.);
    controller.velocity.setpoint = -2.;
    parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 1.5);

    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(-10, voltage);
}

TEST_GROUP(PositionControl)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);

        controller.position.get = mock_get_pos;
        controller.position.get_arg = NULL;
        controller.mode = motor_controller_t::MOTOR_CONTROLLER_POSITION;

        controller.position.setpoint = 2.;
        mock().expectOneCall("get_pos").andReturnValue(1.);

        parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 30);
        parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 1);
        parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 1);
    }
};

TEST(PositionControl, ErrorIsProcessed)
{
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(30, voltage);
}

TEST(PositionControl, ErrorIsSet)
{
    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.position.error);
}

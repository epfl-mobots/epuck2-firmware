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

TEST_GROUP(SetMode)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);

        controller.current.get = mock_get_current;
        controller.current.get_arg = NULL;
        controller.velocity.get = mock_get_speed;
        controller.velocity.get_arg = NULL;
        controller.position.get = mock_get_pos;
        controller.position.get_arg = NULL;

        controller.current.setpoint = 1.0;
        controller.velocity.setpoint = 2.0;
        controller.position.setpoint = 3.0;
    }
};

TEST(SetMode, FromPositionToCurrent)
{
    controller.mode = MOTOR_CONTROLLER_POSITION;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_CURRENT);

    CHECK_EQUAL(MOTOR_CONTROLLER_CURRENT, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(2.0, controller.velocity.setpoint);
    CHECK_EQUAL(3.0, controller.position.setpoint);
}

TEST(SetMode, FromPositionToVelocity)
{
    controller.mode = MOTOR_CONTROLLER_POSITION;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_VELOCITY);

    CHECK_EQUAL(MOTOR_CONTROLLER_VELOCITY, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(2.0, controller.velocity.setpoint);
    CHECK_EQUAL(3.0, controller.position.setpoint);
}

TEST(SetMode, FromVelocityToPosition)
{
    mock().expectOneCall("get_pos").andReturnValue(42.);

    controller.mode = MOTOR_CONTROLLER_VELOCITY;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_POSITION);

    CHECK_EQUAL(MOTOR_CONTROLLER_POSITION, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(2.0, controller.velocity.setpoint);
    CHECK_EQUAL(42.0, controller.position.setpoint);
}

TEST(SetMode, FromCurrentToVelocity)
{
    mock().expectOneCall("get_speed").andReturnValue(32.);

    controller.mode = MOTOR_CONTROLLER_CURRENT;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_VELOCITY);

    CHECK_EQUAL(MOTOR_CONTROLLER_VELOCITY, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(32.0, controller.velocity.setpoint);
    CHECK_EQUAL(3.0, controller.position.setpoint);
}

TEST(SetMode, FromCurrentToPosition)
{
    mock().expectOneCall("get_pos").andReturnValue(42.);
    mock().expectOneCall("get_speed").andReturnValue(32.);

    controller.mode = MOTOR_CONTROLLER_CURRENT;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_POSITION);

    CHECK_EQUAL(MOTOR_CONTROLLER_POSITION, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(32.0, controller.velocity.setpoint);
    CHECK_EQUAL(42.0, controller.position.setpoint);
}

TEST(SetMode, NoChange)
{
    controller.mode = MOTOR_CONTROLLER_POSITION;

    motor_controller_set_mode(&controller, MOTOR_CONTROLLER_POSITION);

    CHECK_EQUAL(MOTOR_CONTROLLER_POSITION, controller.mode)
    CHECK_EQUAL(1.0, controller.current.setpoint);
    CHECK_EQUAL(2.0, controller.velocity.setpoint);
    CHECK_EQUAL(3.0, controller.position.setpoint);
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

        motor_controller_set_mode(&controller, MOTOR_CONTROLLER_CURRENT);
        parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 10);
    }
};

TEST(CurrentControl, ErrorIsProcessed)
{
    controller.current.target_setpoint = 2;
    mock_set_current(1.);
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(10, voltage);
}

TEST(CurrentControl, ErrorIsSet)
{
    controller.current.target_setpoint = 2;
    mock_set_current(1.);
    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.current.error);
}

TEST(CurrentControl, LimitIsEnforced)
{
    parameter_scalar_set(parameter_find(&ns, "/control/limits/current"), 1.5);
    controller.current.target_setpoint = 2;
    mock_set_current(1.);
    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(5, voltage);
}

TEST(CurrentControl, NegativeValuesAreClampedToo)
{
    parameter_scalar_set(parameter_find(&ns, "/control/limits/current"), 1.5);
    controller.current.target_setpoint = -2;
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

        mock().expectOneCall("get_speed").andReturnValue(0.0);
        motor_controller_set_mode(&controller, MOTOR_CONTROLLER_VELOCITY);

    }
};

TEST(VelocityControl, ErrorIsProcessed)
{
    mock_set_speed(1.);
    controller.velocity.target_setpoint = 2.;
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(2., controller.velocity.setpoint);
    CHECK_EQUAL(20, voltage);
}

TEST(VelocityControl, ErrorIsSet)
{
    mock_set_speed(1.);
    controller.velocity.target_setpoint = 2.;

    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.velocity.error);
}

TEST(VelocityControl, MaxVelocityIsEnforced)
{
    mock_set_speed(1.);
    controller.velocity.target_setpoint = 2.;
    parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 1.5);

    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(10, voltage);
}

TEST(VelocityControl, NegativeVelocitiesAreCappedToo)
{
    mock_set_speed(-1.);
    controller.velocity.target_setpoint = -2.;
    parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 1.5);

    auto voltage = motor_controller_process(&controller);
    CHECK_EQUAL(-10, voltage);
}

TEST(VelocityControl, SetpointRamping)
{
    parameter_scalar_set(parameter_find(&ns, "/control/limits/acceleration"), 1.);
    mock_set_speed(1.);
    controller.velocity.setpoint = 1.;
    controller.velocity.target_setpoint = 5.;
    motor_controller_process(&controller);
    CHECK_EQUAL(5., controller.velocity.target_setpoint);
    CHECK_EQUAL(2., controller.velocity.setpoint);
}

TEST_GROUP(PositionControl)
{
    motor_controller_t controller;
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        motor_controller_init(&controller, &ns);


        controller.velocity.get = mock_get_speed;
        controller.velocity.get_arg = NULL;
        controller.position.get = mock_get_pos;
        controller.position.get_arg = NULL;
        mock().expectOneCall("get_speed").andReturnValue(1.);
        mock().expectOneCall("get_pos").andReturnValue(1.);
        motor_controller_set_mode(&controller, MOTOR_CONTROLLER_POSITION);

        parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 1.);
        parameter_scalar_set(parameter_find(&ns, "/control/limits/acceleration"), 1.);
        parameter_scalar_set(parameter_find(&ns, "/control/position/kp"), 30);
        parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 1);
        parameter_scalar_set(parameter_find(&ns, "/control/current/kp"), 1);

        mock().expectOneCall("get_pos").andReturnValue(1.);
        mock().expectOneCall("get_speed").andReturnValue(1.);
    }
};

TEST(PositionControl, ErrorIsProcessed)
{
    controller.position.target_setpoint = 2.;
    auto voltage = motor_controller_process(&controller);

    CHECK_EQUAL(30, voltage);
}

TEST(PositionControl, ErrorIsSet)
{
    controller.position.target_setpoint = 2.;
    motor_controller_process(&controller);
    CHECK_EQUAL(-1, controller.position.error);
}

TEST(PositionControl, SetpointRamping)
{
    controller.position.target_setpoint = 5.;
    parameter_scalar_set(parameter_find(&ns, "/control/limits/velocity"), 10.);
    motor_controller_process(&controller);
    CHECK_EQUAL(5.0, controller.position.target_setpoint)
    CHECK_EQUAL(2.5, controller.position.setpoint)
}

TEST_GROUP(LimitSymmetric)
{
    float limit = 100;
};

TEST(LimitSymmetric, InsideLimitPositive)
{
    float value = 42;

    DOUBLES_EQUAL(value, motor_controller_limit_symmetric(value, limit), 1.0e-9);
}

TEST(LimitSymmetric, InsideLimitNegative)
{
    float value = -42;

    DOUBLES_EQUAL(value, motor_controller_limit_symmetric(value, limit), 1.0e-9);
}

TEST(LimitSymmetric, OutsideLimit)
{
    float value = 250;

    DOUBLES_EQUAL(limit, motor_controller_limit_symmetric(value, limit), 1.0e-9);
}

TEST(LimitSymmetric, OutsideLimitNegative)
{
    float value = -250;

    DOUBLES_EQUAL(-limit, motor_controller_limit_symmetric(value, limit), 1.0e-9);
}

TEST_GROUP(SetpointInterpolation)
{

};

TEST(SetpointInterpolation, PositionConstant)
{
    float pos = 1;
    float vel = 0;
    float acc = 0;
    float delta_t = 0.1;

    DOUBLES_EQUAL(pos,
                  motor_controller_pos_setpt_interpolation(pos, vel, acc, delta_t),
                  1.0e-7);
}

TEST(SetpointInterpolation, PositionConstantVel)
{
    float pos = 1;
    float vel = 2;
    float acc = 0;
    float delta_t = 0.1;

    DOUBLES_EQUAL(1.2, pos + vel * delta_t, 1.0e-7);
    DOUBLES_EQUAL(pos + vel * delta_t,
                  motor_controller_pos_setpt_interpolation(pos, vel, acc, delta_t),
                  1.0e-7);
}

TEST(SetpointInterpolation, PositionStart)
{
    float pos = 1;
    float vel = 0;
    float acc = 2;
    float delta_t = 0.1;

    DOUBLES_EQUAL(1.01, pos + acc * delta_t * delta_t / 2, 1.0e-7);
    DOUBLES_EQUAL(pos + acc * delta_t * delta_t / 2,
                  motor_controller_pos_setpt_interpolation(pos, vel, acc, delta_t),
                  1.0e-7);
}

TEST(SetpointInterpolation, Position)
{
    float pos = 1;
    float vel = 2;
    float acc = 2;
    float delta_t = 0.1;

    DOUBLES_EQUAL(1.21,
                  pos + vel * delta_t + acc * delta_t * delta_t / 2,
                  1.0e-7);
    DOUBLES_EQUAL(pos + vel * delta_t + acc * delta_t * delta_t / 2,
                  motor_controller_pos_setpt_interpolation(pos, vel, acc, delta_t),
                  1.0e-7);
}


TEST(SetpointInterpolation, Velocity)
{
    float vel = 1;
    float acc = 2;
    float delta_t = 0.1;


    DOUBLES_EQUAL(1.2, vel + acc * delta_t, 1.0e-7);
    DOUBLES_EQUAL(vel + acc * delta_t,
                  motor_controller_vel_setpt_interpolation(vel, acc, delta_t),
                  1.0e-7);
}

TEST_GROUP(SetpointVelocityRamp)
{

};

TEST(SetpointVelocityRamp, Static)
{
    float pos = 1;
    float vel = 0;
    float target_pos = pos;
    float delta_t = 0.1;
    float max_vel = 3;
    float max_acc = 2;

    DOUBLES_EQUAL(0,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, Cruising)
{
    float pos = 1;
    float vel = 3;
    float target_pos = 4;
    float delta_t = 0.1;
    float max_vel = 3;
    float max_acc = 2;

    DOUBLES_EQUAL(0,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, Start)
{
    float pos = 1;
    float vel = 0;
    float target_pos = 2;
    float delta_t = 0.1;
    float max_vel = 3;
    float max_acc = 2;

    DOUBLES_EQUAL(max_acc,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, Stopping)
{
    float pos = 1.9;
    float vel = 2;
    float target_pos = 2;
    float delta_t = 0.1;
    float max_vel = 3;
    float max_acc = 2;

    DOUBLES_EQUAL(-max_acc,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, VeryClose)
{
    float pos = 0;
    float delta_t = 0.1;
    float max_acc = 2;
    float vel = max_acc * delta_t / 2;
    float target_pos = pos + max_acc * delta_t * delta_t / 2;
    float max_vel = 3;

    DOUBLES_EQUAL(- max_acc / 2,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, GoingTheWrongWay)
{
    float pos = 1;
    float vel = -2;
    float target_pos = 2;
    float delta_t = 0.1;
    float max_vel = 3;
    float max_acc = 2;

    DOUBLES_EQUAL(max_acc,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, Overshot)
{
    float target_pos = 1;
    float delta_t = 0.1;
    float max_acc = 2;
    float pos = target_pos + max_acc * delta_t * delta_t / 2 * 0.9;
    float vel = max_acc * delta_t / 2;
    float max_vel = 3;

    DOUBLES_EQUAL(- max_acc / 2,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

TEST(SetpointVelocityRamp, TooFast)
{
    float target_pos = 2;
    float delta_t = 0.1;
    float max_acc = 2;
    float pos = 1;
    float vel = 4;
    float max_vel = 3;

    DOUBLES_EQUAL(- max_acc,
                  motor_controller_vel_ramp(pos, vel, target_pos, delta_t, max_vel, max_acc),
                  1.0e-7);
}

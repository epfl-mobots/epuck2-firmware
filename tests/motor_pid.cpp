#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "parameter/parameter.h"
#include "motor_pid.h"
#include "pid/pid.h"

TEST_GROUP(PIDConfigTestGroup)
{
    parameter_namespace_t ns;

    void setup()
    {
        parameter_namespace_declare(&ns, NULL, "root");
        declare_parameters(&ns);
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
    pid_param_s params;

    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    /* TODO Ugly hack */
    params.kp = *parameter_find(&ns, "/control/velocity/kp");
    pid_param_update(&params, &pid);

    CHECK_EQUAL(12, pid.kp);
}

TEST(PIDConfigTestGroup, ChangingGainResetsIntegrator)
{
    pid_ctrl_t pid;
    pid_param_s params;

    pid.integrator = 100;

    declare_parameters(&ns);
    parameter_scalar_set(parameter_find(&ns, "/control/velocity/kp"), 12);

    params.kp = *parameter_find(&ns, "/control/velocity/kp");

    pid_param_update(&params, &pid);

    CHECK_EQUAL(0, pid.integrator);
}

#include "ch.h"
#include "hal.h"

#include "common/consts.h"
#include "common/productids.h"
#include "vm/vm.h"
#include "aseba_vm/skel.h"
#include "aseba_vm/skel_user.h"
#include "aseba_vm/aseba_node.h"

#include "sensors/range.h"


void update_aseba_variables_read(void);
void update_aseba_variables_write(void);

static THD_WORKING_AREA(aseba_vm_thd_wa, 1024);
static THD_FUNCTION(aseba_vm_thd, arg)
{
    (void)arg;

    AsebaVMSetupEvent(&vmState, ASEBA_EVENT_INIT);

    while (TRUE) {
        // Don't spin too fast to avoid consuming all CPU time
        chThdSleepMilliseconds(10);

        // Sync Aseba with the state of the Microcontroller
        update_aseba_variables_read();

        // Run VM for some time
        AsebaVMRun(&vmState, 1000);
        AsebaProcessIncomingEvents(&vmState);

        // Sync the Microcontroller with the state of Aseba
        update_aseba_variables_write();
    }
    return 0;
}

void aseba_vm_init(void)
{
    vmState.nodeId = 3;

    AsebaVMInit(&vmState);
    vmVariables.id = vmState.nodeId;
    vmVariables.productId = ASEBA_PID_UNDEFINED;
    vmVariables.fwversion[0] = 0;
    vmVariables.fwversion[1] = 1;

    vmVariables.range = 0;

    chThdSleepMilliseconds(500);

    AsebaVMSetupEvent(&vmState, ASEBA_EVENT_INIT);
}

void aseba_vm_start(void)
{
    chThdCreateStatic(aseba_vm_thd_wa, sizeof(aseba_vm_thd_wa), LOWPRIO, aseba_vm_thd, NULL);
}

// This function must update the variable to match the microcontroller state
void update_aseba_variables_read(void)
{
    static float range;
    range_get_range(&range);
    vmVariables.range = (int16_t)(range * 1000.0f);
}

// This function must update the microcontrolleur state to match the variables
void update_aseba_variables_write(void)
{

}

#include "ch.h"
#include "hal.h"

#include "vm/natives.h"
#include "skel_user.h"

/*
 * Descriptors
 */
const AsebaVMDescription vmDescription = {
    "Epuck2",
    {
        // {Number of element in array, Name displayed in aseba studio}
        {1, "_id"},
        {1, "event.source"},
        {VM_VARIABLES_ARG_SIZE, "event.args"},
        {2, "_fwversion"},
        {1, "_productId"},

        {1, "range"},

        {0, NULL}
    }
};

// Event descriptions
const AsebaLocalEventDescription localEvents[] = {
    {NULL, NULL}
};

// Native functions
static AsebaNativeFunctionDescription AsebaNativeDescription__system_reboot =
{
    "_system.reboot",
    "Reboot the microcontroller",
    {
        {0,0}
    }
};

void AsebaNative__system_reboot(AsebaVMState *vm)
{
    (void) vm;
    NVIC_SystemReset();
}

// Native function descriptions
const AsebaNativeFunctionDescription* nativeFunctionsDescription[] = {
    &AsebaNativeDescription__system_reboot,
    ASEBA_NATIVES_STD_DESCRIPTIONS,
    0
};

// Native function pointers
const AsebaNativeFunctionPointer nativeFunctions[] = {
    AsebaNative__system_reboot,
    ASEBA_NATIVES_STD_FUNCTIONS,
};

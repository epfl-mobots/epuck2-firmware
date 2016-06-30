#ifndef SKEL_USER_H
#define SKEL_USER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/types.h"
#include "vm/vm.h"
#include "vm/natives.h"
#include "parameter/parameter.h"
#include "sensors/proximity.h"


/** Number of variables usable by the Aseba script. */
#define VM_VARIABLES_FREE_SPACE 256

/** Maximum number of args an Aseba event can use. */
#define VM_VARIABLES_ARG_SIZE 32

#define SETTINGS_COUNT 32

/** Enum containing all the possible events. */
enum AsebaLocalEvents {
    EVENT_BUTTON=0, // Button click
    EVENT_RANGE, // New range measurement
    EVENT_PROXIMITY, // New proximity sensor measurement
};


/** Struct type defining the variables to expose to the VM.
 *
 * @note This should be kept in sync with the variable descriptions.
 */
struct _vmVariables {
    sint16 id;                          // NodeID
    sint16 source;                      // Source
    sint16 args[VM_VARIABLES_ARG_SIZE]; // Args
    sint16 fwversion[2];                // Firmware version
    sint16 productId;                   // Product ID

    // Variables
    uint16 battery_mv;
    uint16 range;
    sint16 proximity_delta[PROXIMITY_NB_CHANNELS];
    sint16 proximity_ambient[PROXIMITY_NB_CHANNELS];
    sint16 proximity_reflected[PROXIMITY_NB_CHANNELS];

    sint16 motor_left_pwm;
    sint16 motor_right_pwm;

    /* 32 bit encoders are stored in an MSB, LSB tuple. */
    sint16 motor_left_enc[2];
    sint16 motor_right_enc[2];

    // Free space
    sint16 freeSpace[VM_VARIABLES_FREE_SPACE];
};

/** Declares the parameters and variables required by the Aseba application. */
void aseba_variables_init(parameter_namespace_t *aseba_ns);

/** Updates the Aseba variables from the system. */
void aseba_read_variables_from_system(AsebaVMState *vm);

/** Updates the system from the Aseba variables. */
void aseba_write_variables_to_system(AsebaVMState *vm);

void accelerometer_cb(void);
void button_cb(void);

extern struct _vmVariables vmVariables;

extern const AsebaVMDescription vmDescription;
extern const AsebaLocalEventDescription localEvents[];

extern AsebaNativeFunctionPointer nativeFunctions[];
extern const AsebaNativeFunctionDescription* nativeFunctionsDescription[];
extern const int nativeFunctions_length;

#ifdef __cplusplus
}
#endif

#endif /* SKEL_USER_H */

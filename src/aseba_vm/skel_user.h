#ifndef SKEL_USER_H
#define SKEL_USER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Send queue size may be smaller (64 is the smallest value) something like 70-80 is better
 * 128 is luxury
 */
#define SEND_QUEUE_SIZE 128

/*
 * Warning, at least an aseba message MUST be able to fit into this RECV_QUEUE_SIZE buffer
 * 256 is IMHO the minimum, but maybe it can be lowered with a lot of caution.
 * The bigger you have, the best it is. Fill the empty ram with it :)
 */
#define RECV_QUEUE_SIZE 756

/*
 * This is the number of "private" variable the aseba script can have
 */
#define VM_VARIABLES_FREE_SPACE 256

/*
 * This is the maximum number of argument an aseba event can receive
 */
#define VM_VARIABLES_ARG_SIZE 32

/*
 * The number of opcode an aseba script can have
 */
#define VM_BYTECODE_SIZE (766 + 768)  // PUT HERE 766 + 768 * a, where a is >= 0
#define VM_STACK_SIZE 128


struct _vmVariables {
	sint16 id; 							// NodeID
	sint16 source; 						// Source
	sint16 args[VM_VARIABLES_ARG_SIZE]; // Args
	sint16 fwversion[2];				// Firmware version
	sint16 productId;					// Product ID

	sint16 range;

	// Free space
	sint16 freeSpace[VM_VARIABLES_FREE_SPACE];
};


enum Events
{
	EVENTS_COUNT   // Do not touch
};

// The content of this structure is implementation-specific.
// The glue provide a way to store and retrive it from flash.
// The only way to write it is to do it from inside the VM (native function)
// The native function access it as a integer array. So, use only int inside this structure
struct private_settings {
	/* ADD here the settings to save into flash */
	/* The minimum size is one integer, the maximum size is 95 integer (Check done at compilation) */
	int settings[95];
};


#ifdef __cplusplus
}
#endif

#endif /* SKEL_USER_H */

#ifndef BATTERY_PROTECTION_H
#define BATTERY_PROTECTION_H

/** Starts the battery protection service.
 *
 * @warning If the "Force on" jumper is placed, the robot cannot shut itself
 * down and it will only warn the user before shutting down. */
void battery_protection_start(void);

#endif

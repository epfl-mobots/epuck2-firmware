#ifndef BATTERY_LEVEL_H
#define BATTERY_LEVEL_H

/** Message reprensenting a measurement of the battery level. */
typedef struct {
    float voltage;
} battery_msg_t;

/** Starts the battery measurement service. */
void battery_level_start(void);

#endif

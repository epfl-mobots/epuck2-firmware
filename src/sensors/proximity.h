#ifndef PROXIMITY_H
#define PROXIMITY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROXIMITY_NB_CHANNELS 13

/** Struct containing a proximity measurment message. */
typedef struct {
    /** Ambient light level (LED is OFF). */
    unsigned int ambient[PROXIMITY_NB_CHANNELS];

    /** Reflected light level (LED is ON). */
    unsigned int reflected[PROXIMITY_NB_CHANNELS];

    /** Difference between ambient and reflected. */
    unsigned int delta[PROXIMITY_NB_CHANNELS];
} proximity_msg_t;


void proximity_start(void);


#ifdef __cplusplus
}
#endif

#endif /* PROXIMITY_H */

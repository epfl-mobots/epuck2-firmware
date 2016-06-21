#ifndef PROXIMITY_H
#define PROXIMITY_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROXIMITY_NB_CHANNELS 13

/** Struct containing a proximity measurment message. */
typedef struct {
    unsigned int values[PROXIMITY_NB_CHANNELS];
} proximity_msg_t;


void proximity_start(void);


#ifdef __cplusplus
}
#endif

#endif /* PROXIMITY_H */

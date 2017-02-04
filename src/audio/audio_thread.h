#ifndef AUDIO_H
#define AUDIO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_OK = 0,
    AUDIO_FILE_NOT_FOUND,
} audio_play_status_t;

typedef struct {
    char path[64];
} audio_play_request_t;

typedef struct {
    audio_play_status_t status;
} audio_play_result_t;

/** Starts the audio services thread. */
void audio_start(void);

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_H */

#ifndef FILE_H
#define FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <hal.h>

extern bool fatfs_mounted;

void sd_init(void);
void sdcard_mount(void);
void sdcard_unmount(void);
int sound_file_open(const char *path);
void sound_file_close(void);
bool sdc_callback(void *arg, dacsample_t *buffer, size_t len, size_t *samples_written);

int sound_file_read(void *, size_t);

void sd_test(void);

#ifdef __cplusplus
}
#endif

#endif /* FILE_H */

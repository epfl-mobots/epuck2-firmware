#ifndef SDCARD_H
#define SDCARD_H

#ifdef __cplusplus
extern "C" {
#endif

extern bool sdcard_mounted;

void sdcard_start(void);
void sdcard_mount(void);
void sdcard_unmount(void);
void sdcard_automount(void);
void sdcard_activity(void);

#ifdef __cplusplus
}
#endif

#endif /* SDCARD_H */

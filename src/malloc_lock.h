#ifdef __cplusplus
extern "C" {
#endif

#ifndef MALLOC_LOCK_H
#define MALLOC_LOCK_H

/** Creates the malloc mutex.
 *
 * @note Must be called after chSysInit but before using malloc in various
 * threads.
 */
void malloc_lock_init(void);

#endif


#ifdef __cplusplus
}
#endif

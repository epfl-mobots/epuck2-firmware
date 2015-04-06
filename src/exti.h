#ifndef EXTI_H
#define EXTI_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#define EXTI_EVENT_MPU6000_INT     5

extern event_source_t exti_events;

void exti_setup(void);


#endif /* EXTI_H */
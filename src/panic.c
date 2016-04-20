#include <ch.h>
#include <hal.h>

void panic_handler(const char *reason)
{
    (void) reason;

    palSetPad(GPIOD, GPIOD_LED_ERROR);

    while (1) {
    }
}

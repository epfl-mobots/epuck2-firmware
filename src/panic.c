#include <ch.h>
#include <hal.h>

void panic_handler(const char *reason)
{
    (void)reason;

    //Turns on LED_ERROR
    palSetPad(GPIOD, GPIOD_LED_ERROR);

    // static volatile uint32_t ipsr;
    // static volatile const char *msg;
    // msg = reason;
    // ipsr = __get_IPSR();

    // (void)msg;
    // (void)ipsr;

    // strncpy(panic_buffer, reason, sizeof(panic_buffer));

    // panic_buffer[sizeof(panic_buffer) - 1] = '\0';
    // panic_buffer_crc = crc32(PANIC_CRC_INIT, panic_buffer, sizeof(panic_buffer));

// #ifdef DEBUG // debug builds block to be able to connect a debugger
//     while (1) {
//         // todo print error over uart
//     }

// #else // non-debug builds reboot in safemode
//     reboot_in_safemode();
// #endif
}
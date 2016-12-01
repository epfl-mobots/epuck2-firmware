
#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include "file.h"

#include <ff.h>
FATFS MMC_FS;
MMCDriver MMCD1;
bool fatfs_mounted = false;
extern BaseSequentialStream *stdout;

static SPIConfig spi1_hscfg = { NULL, GPIOC, 4, 0 };
static SPIConfig spi1_lscfg = { NULL, GPIOC, 4, SPI_CR1_BR_2 | SPI_CR1_BR_1 };

const MMCConfig mmccfg = {
    .spip = &SPID1,
    .lscfg = &spi1_lscfg,
    .hscfg = &spi1_hscfg
};

void sd_init(void)
{
    // setup pads to SPI1 function (connect these pads to your SD card accordingly)
    palSetPadMode(GPIOC, 4, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); // NSS
    palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); // SCK
    palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5)); // MISO
    palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5) | PAL_STM32_OSPEED_HIGHEST); // MOSI
    palSetPad(GPIOC, 4); // set NSS high

    mmcObjectInit(&MMCD1);
    mmcStart(&MMCD1, &mmccfg);
}

void sdcard_mount(void)
{
    FRESULT err;
    fatfs_mounted = false;
    if(mmcConnect(&MMCD1)) {
        chprintf(stdout, "SD: Failed to connect to card\r\n");
        return;
    } else {
        chprintf(stdout, "SD: Connected to card\r\n");
    }
    err = f_mount(&MMC_FS, "", 0);
    if(err != FR_OK) {
        chprintf(stdout, "SD: f_mount() failed %d\r\n", err);
        mmcDisconnect(&MMCD1);
        return;
    } else {
        chprintf(stdout, "SD: File system mounted\r\n");
        fatfs_mounted = true;
    }
}

void sdcard_unmount(void)
{
    f_mount(NULL, "", 0); // unmount
    mmcDisconnect(&MMCD1);
    fatfs_mounted = false;
}

FIL sound_file;
int sound_file_open(const char *path)
{
    FRESULT res = f_open(&sound_file, path, FA_READ);
    if (res != FR_OK) {
        return -1;
    }
    return 0;
}

int sound_file_read(void *buffer, size_t len)
{
    UINT bytes_read;
    FRESULT res = f_read(&sound_file, buffer, len, &bytes_read);
    if (res != FR_OK) {
        return -1;
    }
    return bytes_read;
}

void sound_file_close(void) {
    f_close(&sound_file);
}

bool sdc_callback(void *arg, dacsample_t *buffer, size_t len, size_t *samples_written)
{
    (void)arg;
    int n = sound_file_read(buffer, len*2);
    if (n < 0) {
        /* read error, stop conversion */
        *samples_written = 0;
        return true;
    }
    int i;
    for (i = 0; i < n/2; i++) {
        buffer[i] += -INT16_MIN;
    }
    *samples_written = n/2;
    if (n != (int)len*2) {
        /* end of file */
        return true;
    } else {
        return false;
    }
}

void sd_test(void)
{
    int res;
    sd_init();
    sdcard_mount();
    const char *file = "/sound.wav";
    res = sound_file_open(file);
    if (res) {
        chprintf(stdout, "cannot open file\n");
    } else {
        chprintf(stdout, "open %s\n", file);
        static uint8_t buffer[100];
        res = sound_file_read(buffer, 100);
        chprintf(stdout, "read %d bytes\n");
        chprintf(stdout, "close file\n");
        sound_file_close();
    }
    chprintf(stdout, "SD: unmount\n");
    sdcard_unmount();
}

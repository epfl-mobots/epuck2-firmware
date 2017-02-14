#include <string.h>
#include "audio_wav.h"

#define WAV_HEADER_LEN 44

static uint32_t u32_little_endian(uint8_t *buf)
{
    return (uint32_t) buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

static uint16_t u16_little_endian(uint8_t *buf)
{
    return (uint16_t) buf[0] | buf[1] << 8;
}

int wav_read_header(struct wav_data *d, FIL *file)
{
    static uint8_t buf[WAV_HEADER_LEN];

    d->file = file;

    UINT n;
    FRESULT res = f_read(file, buf, WAV_HEADER_LEN, &n);
    if (res != FR_OK || n != WAV_HEADER_LEN) {
        return -1;
    }

    /* file type */
    if (memcmp(&buf[0], "RIFF", 4) ||
        memcmp(&buf[8], "WAVE", 4)) {
        return -1;
    }

    /* subchunk ID */
    if (memcmp(&buf[12], "fmt ", 4)) {
        return -1;
    }

    /* subchunk size */
    if (u32_little_endian(&buf[16]) != 16) {
        return -1;
    }

    /* audio format, 1 = PCM */
    if (u16_little_endian(&buf[20]) != 1) {
        return -1;
    }

    /* number of channels, only support mono */
    if (u16_little_endian(&buf[22]) != 1) {
        return -1;
    }

    d->sample_rate = u32_little_endian(&buf[24]);

    d->sample_size = u16_little_endian(&buf[34]) / 8;
    if (d->sample_size != 2) {
        return -1;
    }

    /* check for data chunk */
    if (memcmp(&buf[36], "data", 4)) {
        return -1;
    }

    d->data_len = u32_little_endian(&buf[40]);
    d->data_pos = 0;

    return 0;
}

bool wav_read_cb(void *arg, audio_sample_t *buffer, size_t buf_len, size_t *samples_written)
{
    struct wav_data *wav = (struct wav_data *)arg;

    /* Determine bytes to be read */
    size_t len = buf_len * sizeof(audio_sample_t);
    if (len > wav->data_len - wav->data_pos) {
        len = wav->data_len - wav->data_pos;
    }

    UINT n;
    FRESULT res = f_read(wav->file, buffer, len, &n);
    /* If read error then stop the conversion. */
    if (res != FR_OK) {
        *samples_written = 0;
        return true;
    }

    /* Add offset to obtain positive integer for DAC.  */
    size_t i;
    for (i = 0; i < n / sizeof(audio_sample_t); i++) {
        buffer[i] += -INT16_MIN;
    }

    wav->data_pos += n;
    *samples_written = n / sizeof(audio_sample_t);
    if (n != len || wav->data_pos == wav->data_len) {
        /* end of file */
        return true;
    } else {
        return false;
    }
}

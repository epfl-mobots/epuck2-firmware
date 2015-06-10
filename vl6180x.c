#include "vl6180x.h"
#include "vl6180x_registers.h"
#include <ch.h>

void vl6180x_init(vl6180x_t *dev, I2CDriver *i2c_dev, uint8_t address)
{
    dev->i2c = i2c_dev;
    dev->address = address;
}

uint8_t vl6180x_measure_distance(vl6180x_t *dev, uint8_t *out_mm)
{
    uint8_t status, mm;

    /* Wait for device ready. */
    do {
        status = vl6180x_read_register(dev, VL6180X_RESULT_RANGE_STATUS);
    } while ((status & (1 << 0)) == 0);

    /* Start measurement. */
    vl6180x_write_register(dev, VL6180X_SYSRANGE_START, 0x01);

    /* Wait for measurement ready. */
    // do {
    //     status = vl6180x_read_register(dev, VL6180X_RESULT_INTERRUPT_STATUS_GPIO);
    // } while ((status & (1 << 2)) == 0);
    chThdSleepMilliseconds(100);

    /* Read result. */
    mm = vl6180x_read_register(dev, VL6180X_RESULT_RANGE_VAL);
    *out_mm = mm;

    /* Clear interrupt flags. */
    vl6180x_write_register(dev, VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);

    /* Wait for device ready. */
    do {
        status = vl6180x_read_register(dev, VL6180X_RESULT_RANGE_STATUS);
    } while ((status & (1 << 0)) == 0);

    /* Return error code. */
    return (status >> 4);
}

void vl6180x_configure(vl6180x_t *dev)
{
    while (vl6180x_read_register(dev, 0x16) != 0x01) {
    }

    /* Mandatory: Private registers. */
    vl6180x_write_register(dev, 0x0207, 0x01);
    vl6180x_write_register(dev, 0x0208, 0x01);
    vl6180x_write_register(dev, 0x0096, 0x00);
    vl6180x_write_register(dev, 0x0097, 0xfd);
    vl6180x_write_register(dev, 0x00e3, 0x00);
    vl6180x_write_register(dev, 0x00e4, 0x04);
    vl6180x_write_register(dev, 0x00e5, 0x02);
    vl6180x_write_register(dev, 0x00e6, 0x01);
    vl6180x_write_register(dev, 0x00e7, 0x03);
    vl6180x_write_register(dev, 0x00f5, 0x02);
    vl6180x_write_register(dev, 0x00d9, 0x05);
    vl6180x_write_register(dev, 0x00db, 0xce);
    vl6180x_write_register(dev, 0x00dc, 0x03);
    vl6180x_write_register(dev, 0x00dd, 0xf8);
    vl6180x_write_register(dev, 0x009f, 0x00);
    vl6180x_write_register(dev, 0x00a3, 0x3c);
    vl6180x_write_register(dev, 0x00b7, 0x00);
    vl6180x_write_register(dev, 0x00bb, 0x3c);
    vl6180x_write_register(dev, 0x00b2, 0x09);
    vl6180x_write_register(dev, 0x00ca, 0x09);
    vl6180x_write_register(dev, 0x0198, 0x01);
    vl6180x_write_register(dev, 0x01b0, 0x17);
    vl6180x_write_register(dev, 0x01ad, 0x00);
    vl6180x_write_register(dev, 0x00ff, 0x05);
    vl6180x_write_register(dev, 0x0100, 0x05);
    vl6180x_write_register(dev, 0x0199, 0x05);
    vl6180x_write_register(dev, 0x01a6, 0x1b);
    vl6180x_write_register(dev, 0x01ac, 0x3e);
    vl6180x_write_register(dev, 0x01a7, 0x1f);
    vl6180x_write_register(dev, 0x0030, 0x00);

    /* Recommended : Public registers - See data sheet for more detail */

    /* Enables polling for New Sample ready when measurement completes */
    vl6180x_write_register(dev, 0x0011, 0x10);
    /* Set the averaging sample period (compromise between lower noise and
     * increased execution time) */
    vl6180x_write_register(dev, 0x010a, 0x30);
    /* Sets the light and dark gain (upper nibble). Dark gain should not be
     * changed.*/
    vl6180x_write_register(dev, 0x003f, 0x46);
    /* sets the # of range measurements after which auto calibration of system
     * is performed */
    vl6180x_write_register(dev, 0x0031, 0xFF);
    /* Set ALS integration time to 100ms */
    vl6180x_write_register(dev, 0x0040, 0x63);
    /* perform a single temperature calibration of the ranging sensor */
    vl6180x_write_register(dev, 0x002e, 0x01);
}


#ifdef HAL_USE_I2C
void vl6180x_write_register(vl6180x_t *dev, uint16_t reg, uint8_t val)
{
    uint8_t buf[] = {(reg >> 8), reg & 0xff, val};
    i2cMasterTransmit(dev->i2c, dev->address, buf, 3, NULL, 0);
}

uint8_t vl6180x_read_register(vl6180x_t *dev, uint16_t reg)
{
    uint8_t ret;
    uint8_t buf[] = {(reg >> 8), reg & 0xff};
    i2cMasterTransmit(dev->i2c, dev->address, buf, 2, &ret, 1);
    return ret;
}
#endif

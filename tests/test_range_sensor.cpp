#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "../vl6180x.h"
#include <cstring>
#include "../vl6180x_registers.h"

uint8_t vl6180x_read_register(vl6180x_t *dev, uint16_t reg)
{
    return mock().actualCall(__FUNCTION__)
                 .withIntParameter("reg", reg)
                 .returnIntValue();
}

void vl6180x_write_register(vl6180x_t *dev, uint16_t reg, uint8_t val)
{
    mock().actualCall(__FUNCTION__)
          .withIntParameter("reg", reg)
          .withIntParameter("val", val);
}

TEST_GROUP(VL6180XRegisterTestGroup)
{
    vl6180x_t dev;
    I2CDriver i2c;

    void setup(void)
    {
        vl6180x_init(&dev, &i2c, VL6180X_DEFAULT_ADDRESS);
    }

    void teardown(void)
    {
        mock().checkExpectations();
        mock().clear();
    }

    void expect_write(uint16_t reg, uint8_t val)
    {
        mock().expectOneCall("vl6180x_write_register")
            .withIntParameter("reg", reg)
            .withIntParameter("val", val);
    }

    void expect_read(uint16_t reg, uint8_t val)
    {
        mock().expectOneCall("vl6180x_read_register")
            .withIntParameter("reg", reg)
            .andReturnValue(val);
    }
};

TEST(VL6180XRegisterTestGroup, CanInitDriver)
{
    POINTERS_EQUAL(&i2c, dev.i2c);
    CHECK_EQUAL(0x29, dev.address);
}

TEST(VL6180XRegisterTestGroup, CanReadRegister)
{
    uint8_t ret;

    expect_read(0xaabb, 42);

    ret = vl6180x_read_register(&dev, 0xaabb);

    CHECK_EQUAL(ret, 42);
}

TEST(VL6180XRegisterTestGroup, CanWriteRegister)
{

    expect_write(0xaabb, 0xff);

    vl6180x_write_register(&dev, 0xaabb, 0xff);
}

TEST(VL6180XRegisterTestGroup, CanReadDistance)
{
    /* See section 2.4.1 of the datasheet for information on the single shot
     * measure sequence.
     * AN4545 is also very useful. */
    uint8_t mm, ret;

    /* Start of measurement. */
    expect_write(VL6180X_SYSRANGE_START, 0x01);

    /* Wait for measurement to be ready. */
    expect_read(VL6180X_RESULT_INTERRUPT_STATUS_GPIO, 0);
    expect_read(VL6180X_RESULT_INTERRUPT_STATUS_GPIO, 0);
    expect_read(VL6180X_RESULT_INTERRUPT_STATUS_GPIO, (1 << 2));

    /* Measurement result. */
    expect_read(VL6180X_RESULT_RANGE_VAL, 18);

    /* Interrupt clear. */
    expect_write(VL6180X_SYSTEM_INTERRUPT_CLEAR, 0x07);

    /* Wait for sensor to be ready, return error code 0b1001 (unused). */
    uint8_t err_code = (0x09 << 4);
    expect_read(VL6180X_RESULT_RANGE_STATUS, err_code | 0);
    expect_read(VL6180X_RESULT_RANGE_STATUS, err_code | 0);
    expect_read(VL6180X_RESULT_RANGE_STATUS, err_code | 0);
    expect_read(VL6180X_RESULT_RANGE_STATUS, err_code | 1);

    ret = vl6180x_measure_distance(&dev, &mm);

    CHECK_EQUAL(ret, 0x09);
}

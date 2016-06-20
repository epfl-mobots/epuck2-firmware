#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>
#include "config_flash_storage.h"
#include "config_flash_storage_private.h"
#include "parameter/parameter_msgpack.h"
#include <cstdio>
#include <cstring>
#include "crc/crc32.h"

TEST_GROUP(ConfigSaveTestCase)
{
    uint8_t data[128];
    parameter_namespace_t ns;

    void setup()
    {
        mock("flash").ignoreOtherCalls();
        parameter_namespace_declare(&ns, NULL, NULL);
    }
};

TEST(ConfigSaveTestCase, SavingConfigErasesPages)
{
    // In case we cannot find a valid block we erase the sector, just in case
    mock("flash").expectOneCall("erase").withParameter("sector", data);

    config_save(data, sizeof(data), &ns);
}

TEST(ConfigSaveTestCase, SavingConfigLockUnlock)
{
    mock("flash").strictOrder();

    mock("flash").expectOneCall("unlock");
    mock("flash").expectOneCall("erase").withParameter("sector", data);
    mock("flash").expectOneCall("write"); // data
    mock("flash").expectOneCall("write"); // CRC(len)
    mock("flash").expectOneCall("write"); // len
    mock("flash").expectOneCall("write"); // crc(data)
    mock("flash").expectOneCall("lock");

    config_save(data, sizeof(data), &ns);

    // We need to manually check expectations since we tuned the flash mock to preserve ordering.
    mock("flash").checkExpectations();
}

static void err_cb(void *p, const char *id, const char *err)
{
    (void) p;
    (void) id;
    (void) err;
    char msg[128];
    snprintf(msg, sizeof msg, "MessagePack error on item \"%s\": \"%s\"", id, err);

    FAIL(msg);
}

TEST(ConfigSaveTestCase, SavingConfigWorks)
{
    // Declare a parameter and gives it a value.
    parameter_t p;
    parameter_integer_declare(&p, &ns, "foo");
    parameter_integer_set(&p, 10);

    // Check that the flash writer is used
    // Number of expected written bytes is implementation-dependent
    // Change if if necessary
    for (int i = 0; i < 8; i++) {
        mock("flash").expectOneCall("write");
    }

    // Saves the parameter, then change its value
    config_save(data, sizeof(data), &ns);
    parameter_integer_set(&p, 20);
    CHECK_EQUAL(20, parameter_integer_get(parameter_find(&ns, "/foo")));

    // Loads the config back from saved state
    // We add an offset to skip the CRC and the block length
    parameter_msgpack_read(&ns,
                           (char *)(&data[CONFIG_HEADER_SIZE]),
                           sizeof(data) - CONFIG_HEADER_SIZE,
                           err_cb, NULL);

    // Check that the parameter has the same value as saved
    CHECK_EQUAL(10, parameter_integer_get(parameter_find(&ns, "/foo")));
}

TEST_GROUP(ConfigLoadTestCase)
{
    uint8_t data[128];
    parameter_namespace_t ns;
    parameter_t foo;

    void setup()
    {
        mock("flash").ignoreOtherCalls();
        parameter_namespace_declare(&ns, NULL, NULL);
        parameter_integer_declare(&foo, &ns, "foo");
    }
};

TEST(ConfigLoadTestCase, SimpleLoad)
{
    // Set a value, then save it
    parameter_integer_set(&foo, 20);
    config_save(data, sizeof(data), &ns);

    // Change the value
    parameter_integer_set(&foo, 10);

    // Load the tree
    auto res = config_load(&ns, data);

    // Value should be back to what it was
    CHECK_TRUE(res);
    CHECK_EQUAL(20, parameter_integer_get(&foo));
}

TEST(ConfigLoadTestCase, CRCIsChecked)
{
    // Set a value, then save it
    parameter_integer_set(&foo, 20);
    config_save(data, sizeof(data), &ns);

    // Change the value
    parameter_integer_set(&foo, 10);

    // Corrupt data
    data[0] ^= 0x40;

    // Load the tree
    auto res = config_load(&ns, data);

    // Value should not have changed
    CHECK_FALSE(res);
    CHECK_EQUAL(10, parameter_integer_get(&foo));
}

TEST(ConfigLoadTestCase, InvalidConfigReturnsFalse)
{
    // This test checks that loading an invalid config fails, even if the
    // checksum is correct.
    // This might be the case if the layout of the parameter tree changes for
    // example.
    parameter_integer_set(&foo, 20);
    config_save(data, sizeof(data), &ns);

    // Create a new different tree
    parameter_namespace_declare(&ns, NULL, NULL);
    parameter_integer_declare(&foo, &ns, "bar");

    // Try to load it
    auto res = config_load(&ns, data);

    CHECK_FALSE(res);
}

TEST_GROUP(BlockValidityTestGroup)
{
    uint8_t block[256 + CONFIG_HEADER_SIZE];

    void setup()
    {
        memset(block, 0, sizeof(block));

        mock("flash").disable();
        config_write_block_header(block, sizeof(block) - CONFIG_HEADER_SIZE);
        mock("flash").enable();
    }

};

TEST(BlockValidityTestGroup, DefaultBlockIsValid)
{
    CHECK_TRUE(config_block_is_valid(block));
}

TEST(BlockValidityTestGroup, InvalidLengthChecksumMakesItInvalid)
{
    block[0] ^= 0xaa;
    CHECK_FALSE(config_block_is_valid(block));
}

TEST(BlockValidityTestGroup, InvalidLengthIsInvalid)
{
    block[5] ^= 0xaa;
    CHECK_FALSE(config_block_is_valid(block));
}

TEST(BlockValidityTestGroup, InvalidDataCheckSumIsInvalid)
{
    block[127] ^= 0xaa;
    CHECK_FALSE(config_block_is_valid(block));
}

TEST(BlockValidityTestGroup, FreshFlashIsInvalid)
{
    /* Checks that a fresh flash page (all 0xff) is invalid. */
    memset(block, 0xff, sizeof(block));
    CHECK_FALSE(config_block_is_valid(block));
}

TEST_GROUP(BlockGetLengthTestCase)
{
    uint8_t block[10 + CONFIG_HEADER_SIZE];
};

TEST(BlockGetLengthTestCase, CanGetBlockLength)
{
    mock("flash").disable();
    config_write_block_header(block, 10);
    mock("flash").enable();

    auto res = config_block_get_length(block);

    CHECK_EQUAL(10, res);
}

TEST_GROUP(FindFirstValidBlockTestGroup)
{
    uint8_t block[256];
};

TEST(FindFirstValidBlockTestGroup, ReturnsFirstPartofTheBlock)
{
    auto first_block = config_block_find_first_free(block);
    POINTERS_EQUAL(&block[0], first_block);
}

TEST(FindFirstValidBlockTestGroup, SkipsAlreadyUsedBlocks)
{
    mock("flash").disable();
    config_write_block_header(block, 10);
    auto first_block = config_block_find_first_free(block);

    POINTERS_EQUAL(&block[10 + CONFIG_HEADER_SIZE], first_block);
}

TEST(FindFirstValidBlockTestGroup, FindNoUsedBlockReturnsNULL)
{
    auto last_block = config_block_find_last_used(block);
    POINTERS_EQUAL(NULL, last_block);
}

TEST(FindFirstValidBlockTestGroup, FindLastUsedBlock)
{
    mock("flash").disable();
    config_write_block_header(block, 10);

    POINTERS_EQUAL(block, config_block_find_last_used(block));
}

TEST_GROUP(ConfigLoadBalancingTestGroup)
{
    parameter_namespace_t ns;
    uint8_t block[256];

    void setup()
    {
        mock("flash").ignoreOtherCalls();
        parameter_namespace_declare(&ns, NULL, NULL);
    }

};

TEST(ConfigLoadBalancingTestGroup, ConfigSavingIsLoadBalanced)
{
    // Checks that saving a config declares a new block at the end of the flash
    // config page.

    // First, declare a parameter
    parameter_t p;
    parameter_integer_declare(&p, &ns, "foo");
    parameter_integer_set(&p, 10);

    // Checks that the first save is done on the first block
    config_save(block, sizeof(block), &ns);
    POINTERS_EQUAL(block, config_block_find_last_used(block));

    // Checks that the second save is on the second block
    auto second_block = config_block_find_first_free(block);
    config_save(block, sizeof(block), &ns);
    POINTERS_EQUAL(second_block, config_block_find_last_used(block));
}

TEST(ConfigLoadBalancingTestGroup, CanLoadCorrectBlock)
{
    // Checks that config page loading chooses the latest version correctly

    // First, declare a parameter
    parameter_t p;
    parameter_integer_declare(&p, &ns, "foo");

    // Give it a value then save
    parameter_integer_set(&p, 1);
    config_save(block, sizeof(block), &ns);

    // Give it a value again then save
    parameter_integer_set(&p, 2);
    config_save(block, sizeof(block), &ns);

    // Finally load it from flash and see if latest version was used
    parameter_integer_set(&p, 3);

    auto res = config_load(&ns, block);
    CHECK_TRUE(res);
    CHECK_EQUAL(2, parameter_integer_get(&p));
}

TEST(ConfigLoadBalancingTestGroup, CanErasePageIfNotenoughSpaceLeft)
{
    // Checks that we erase the whole block if we don't have enough space left
    // to write the config header

    uint8_t block[30];

    parameter_t p;
    parameter_integer_declare(&p, &ns, "foo");

    // Give it a value then save
    parameter_integer_set(&p, 1);

    config_save(block, sizeof(block), &ns);

    // We used 10 + CONFIG_HEADER_SIZE bytes, so there is not enough free space
    // available for the header
    CHECK_EQUAL(10, config_block_get_length(block));

    mock("flash").expectOneCall("erase").withParameter("sector", block);
    config_save(block, sizeof(block), &ns);
}

TEST(ConfigSaveTestCase, CanErasePageIfNotenoughSpaceLeftForData)
{
    uint8_t block[36];

    parameter_t p;
    parameter_integer_declare(&p, &ns, "foo");

    // Give it a value then save
    parameter_integer_set(&p, 1);

    config_save(block, sizeof(block), &ns);

    // We used 10 + CONFIG_HEADER_SIZE bytes, so there is not enough free space
    // available
    CHECK_EQUAL(10, config_block_get_length(block));

    mock("flash").expectOneCall("erase").withParameter("sector", block);
    mock("flash").expectOneCall("erase").withParameter("sector", block);
    config_save(block, sizeof(block), &ns);
}

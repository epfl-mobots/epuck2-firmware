/*
    ChibiOS - Copyright (C) 2006..2015 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

/*
 * Setup for STM32F407ZGT6
 */

/*
 * Board identifier.
 */
#define BOARD_STM32F407_E_PUCK_2_V1
#define BOARD_NAME     "Epuck 2"

/*
 * Board oscillators-related settings.
 */
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                0
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                8000000
#endif


/*
 *  Define for functional usb without VBUS detection
 */
#define BOARD_OTG_NOVBUSSENS 1

/*
 * Board voltages.
 * Required for performance limits calculation.
 */
#define STM32_VDD                   330

/*
 * MCU type as defined in the ST header.
 */
#define STM32F407xx

/*
 * IO pins assignments.
 */
#define GPIOA_MOT0_CURRENT          0
#define GPIOA_MOT1_QEA              1
#define GPIOA_MOT1_CURRENT          2
#define GPIOA_VSYS_ADC              3
#define GPIOA_DCMI_HSYNC            4
#define GPIOA_AUDIO_SPEAKER         5
#define GPIOA_DCMI_PIXCLK           6
#define GPIOA_AUDIO_MIC2            7
#define GPIOA_CAM_CLK               8
#define GPIOA_PIN9                  9
#define GPIOA_PIN10                 10
#define GPIOA_OTG_FS_DM             11
#define GPIOA_OTG_FS_DP             12
#define GPIOA_SWDIO                 13
#define GPIOA_SWCLK                 14
#define GPIOA_MOT1_QEB              15

#define GPIOB_PWM0_PHASE_A          0
#define GPIOB_PWM0_PHASE_B          1
#define GPIOB_BOOT1                 2
#define GPIOB_SPI_CLK               3
#define GPIOB_SPI_MISO              4
#define GPIOB_SPI_MOSI              5
#define GPIOB_DCMI_D5               6
#define GPIOB_DCMI_VSYNC            7
#define GPIOB_I2C_SCLK              8
#define GPIOB_I2C_SDA               9
#define GPIOB_PIN10                 10
#define GPIOB_PIN11                 11
#define GPIOB_PIN12                 12
#define GPIOB_PIN13                 13
#define GPIOB_IR_PWM                14
#define GPIOB_PIN15                 15

#define GPIOC_IR_AN8                0
#define GPIOC_IR_AN9                1
#define GPIOC_IR_AN10               2
#define GPIOC_IR_AN11               3
#define GPIOC_IR_AN12               4
#define GPIOC_AUDIO_MIC1            5
#define GPIOC_DCMI_D0               6
#define GPIOC_DCMI_D1               7
#define GPIOC_SDIO_D0               8
#define GPIOC_SDIO_D1               9
#define GPIOC_SDIO_D2               10
#define GPIOC_SDIO_D3               11
#define GPIOC_SDIO_CLK              12
#define GPIOC_CAM_STROBE            13
#define GPIOC_CAM_RST               14
#define GPIOC_CAM_PWDN              15

#define GPIOD_CAN_RX                0
#define GPIOD_CAN_TX                1
#define GPIOD_SDIO_CMD              2
#define GPIOD_2V8_ON                3
#define GPIOD_PIN4                  4
#define GPIOD_CAM_GPIO_0            5
#define GPIOD_CAM_GPIO_1            6
#define GPIOD_CAM_GPIO_2            7
#define GPIOD_CAM_EXPST             8
#define GPIOD_CAM_FREX              9
#define GPIOD_PIN10                 10
#define GPIOD_PIN11                 11
#define GPIOD_PWM1_PHASE_A          12
#define GPIOD_PWM1_PHASE_B          13
#define GPIOD_LED_DRIVER_RST        14
#define GPIOD_LED_ERROR             15

#define GPIOE_DCMI_D2               0
#define GPIOE_DCMI_D3               1
#define GPIOE_PIN2                  2
#define GPIOE_PIN3                  3
#define GPIOE_DCMI_D4               4
#define GPIOE_DCMI_D6               5
#define GPIOE_DCMI_D7               6
#define GPIOE_LED_HEARTBEAT         7
#define GPIOE_LED_STATUS            8
#define GPIOE_MOT0_QEA              9
#define GPIOE_LED_SD                10
#define GPIOE_MOT0_QEB              11
#define GPIOE_RF_GPIO0_1            12
#define GPIOE_RF_GPIO1_1            13
#define GPIOE_RF_GPIO0_2            14
#define GPIOE_RF_GPIO1_2            15

#define GPIOF_SD_DETECT             0
#define GPIOF_POWERON               1
#define GPIOF_PIN2                  2
#define GPIOF_IR_AN0                3
#define GPIOF_IR_AN1                4
#define GPIOF_IR_AN2                5
#define GPIOF_IR_AN3                6
#define GPIOF_IR_AN4                7
#define GPIOF_IR_AN5                8
#define GPIOF_IR_AN6                9
#define GPIOF_IR_AN7                10
#define GPIOF_IMU_INT               11
#define GPIOF_MPU_FSYNC             12
#define GPIOF_MPU_CS                13
#define GPIOF_POWER_BUTTON          14
#define GPIOF_PIN15                 15

#define GPIOG_PIN0                  0
#define GPIOG_PIN1                  1
#define GPIOG_PIN2                  2
#define GPIOG_PIN3                  3
#define GPIOG_PIN4                  4
#define GPIOG_PIN5                  5
#define GPIOG_PIN6                  6
#define GPIOG_PIN7                  7
#define GPIOG_PIN8                  8
#define GPIOG_UART_RX               9
#define GPIOG_PIN10                 10
#define GPIOG_PIN11                 11
#define GPIOG_UART_RTS              12
#define GPIOG_UART_CTS              13
#define GPIOG_UART_TX               14
#define GPIOG_PIN15                 15

#define GPIOH_OSC_IN                0
#define GPIOH_OSC_OUT               1
#define GPIOH_PIN2                  2
#define GPIOH_PIN3                  3
#define GPIOH_PIN4                  4
#define GPIOH_PIN5                  5
#define GPIOH_PIN6                  6
#define GPIOH_PIN7                  7
#define GPIOH_PIN8                  8
#define GPIOH_PIN9                  9
#define GPIOH_PIN10                 10
#define GPIOH_PIN11                 11
#define GPIOH_PIN12                 12
#define GPIOH_PIN13                 13
#define GPIOH_PIN14                 14
#define GPIOH_PIN15                 15

#define GPIOI_PIN0                  0
#define GPIOI_PIN1                  1
#define GPIOI_PIN2                  2
#define GPIOI_PIN3                  3
#define GPIOI_PIN4                  4
#define GPIOI_PIN5                  5
#define GPIOI_PIN6                  6
#define GPIOI_PIN7                  7
#define GPIOI_PIN8                  8
#define GPIOI_PIN9                  9
#define GPIOI_PIN10                 10
#define GPIOI_PIN11                 11
#define GPIOI_PIN12                 12
#define GPIOI_PIN13                 13
#define GPIOI_PIN14                 14
#define GPIOI_PIN15                 15

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * Please refer to the STM32 Reference Manual for details.
 */
#define PIN_MODE_INPUT(n)           (0U << ((n) * 2))
#define PIN_MODE_OUTPUT(n)          (1U << ((n) * 2))
#define PIN_MODE_ALTERNATE(n)       (2U << ((n) * 2))
#define PIN_MODE_ANALOG(n)          (3U << ((n) * 2))
#define PIN_ODR_LOW(n)              (0U << (n))
#define PIN_ODR_HIGH(n)             (1U << (n))
#define PIN_OTYPE_PUSHPULL(n)       (0U << (n))
#define PIN_OTYPE_OPENDRAIN(n)      (1U << (n))
#define PIN_OSPEED_2M(n)            (0U << ((n) * 2))
#define PIN_OSPEED_25M(n)           (1U << ((n) * 2))
#define PIN_OSPEED_50M(n)           (2U << ((n) * 2))
#define PIN_OSPEED_100M(n)          (3U << ((n) * 2))
#define PIN_PUPDR_FLOATING(n)       (0U << ((n) * 2))
#define PIN_PUPDR_PULLUP(n)         (1U << ((n) * 2))
#define PIN_PUPDR_PULLDOWN(n)       (2U << ((n) * 2))
#define PIN_AFIO_AF(n, v)           ((v ## U) << ((n % 8) * 4))

/*
 * GPIOA setup:
 *
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ANALOG(GPIOA_MOT0_CURRENT) |         \
                                     PIN_MODE_ALTERNATE(GPIOA_MOT1_QEA) |           \
                                     PIN_MODE_ANALOG(GPIOA_MOT1_CURRENT) |           \
                                     PIN_MODE_ANALOG(GPIOA_VSYS_ADC) |           \
                                     PIN_MODE_INPUT(GPIOA_DCMI_HSYNC) |       \
                                     PIN_MODE_INPUT(GPIOA_AUDIO_SPEAKER) |        \
                                     PIN_MODE_INPUT(GPIOA_DCMI_PIXCLK) |        \
                                     PIN_MODE_INPUT(GPIOA_AUDIO_MIC2) |        \
                                     PIN_MODE_INPUT(GPIOA_CAM_CLK) |           \
                                     PIN_MODE_INPUT(GPIOA_PIN9) |        \
                                     PIN_MODE_INPUT(GPIOA_PIN10) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DM) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_OTG_FS_DP) |  \
                                     PIN_MODE_ALTERNATE(GPIOA_SWDIO) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_SWCLK) |      \
                                     PIN_MODE_ALTERNATE(GPIOA_MOT1_QEB))
#define VAL_GPIOA_OTYPER            (PIN_OTYPE_OPENDRAIN(GPIOA_MOT0_CURRENT) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MOT1_QEA) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOA_MOT1_CURRENT) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOA_VSYS_ADC) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_DCMI_HSYNC) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AUDIO_SPEAKER) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_DCMI_PIXCLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AUDIO_MIC2) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOA_CAM_CLK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN9) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOA_PIN10) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DM) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_OTG_FS_DP) |  \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWDIO) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SWCLK) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOA_MOT1_QEB))
#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_100M(GPIOA_MOT0_CURRENT) |        \
                                     PIN_OSPEED_100M(GPIOA_MOT1_QEA) |          \
                                     PIN_OSPEED_100M(GPIOA_MOT1_CURRENT) |          \
                                     PIN_OSPEED_100M(GPIOA_VSYS_ADC) |          \
                                     PIN_OSPEED_100M(GPIOA_DCMI_HSYNC) |          \
                                     PIN_OSPEED_50M(GPIOA_AUDIO_SPEAKER) |            \
                                     PIN_OSPEED_50M(GPIOA_DCMI_PIXCLK) |            \
                                     PIN_OSPEED_50M(GPIOA_AUDIO_MIC2) |            \
                                     PIN_OSPEED_100M(GPIOA_CAM_CLK) |          \
                                     PIN_OSPEED_100M(GPIOA_PIN9) |       \
                                     PIN_OSPEED_100M(GPIOA_PIN10) |     \
                                     PIN_OSPEED_100M(GPIOA_OTG_FS_DM) |     \
                                     PIN_OSPEED_100M(GPIOA_OTG_FS_DP) |     \
                                     PIN_OSPEED_100M(GPIOA_SWDIO) |         \
                                     PIN_OSPEED_100M(GPIOA_SWCLK) |         \
                                     PIN_OSPEED_100M(GPIOA_MOT1_QEB))
#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_MOT0_CURRENT) |     \
                                     PIN_PUPDR_FLOATING(GPIOA_MOT1_QEA) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_MOT1_CURRENT) |         \
                                     PIN_PUPDR_FLOATING(GPIOA_VSYS_ADC) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_DCMI_HSYNC) |       \
                                     PIN_PUPDR_PULLUP(GPIOA_AUDIO_SPEAKER) |        \
                                     PIN_PUPDR_PULLUP(GPIOA_DCMI_PIXCLK) |        \
                                     PIN_PUPDR_PULLUP(GPIOA_AUDIO_MIC2) |        \
                                     PIN_PUPDR_PULLUP(GPIOA_CAM_CLK) |         \
                                     PIN_PUPDR_PULLUP(GPIOA_PIN9) |    \
                                     PIN_PUPDR_PULLUP(GPIOA_PIN10) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DM) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_OTG_FS_DP) |  \
                                     PIN_PUPDR_FLOATING(GPIOA_SWDIO) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_SWCLK) |      \
                                     PIN_PUPDR_FLOATING(GPIOA_MOT1_QEB))
#define VAL_GPIOA_ODR               (PIN_ODR_LOW(GPIOA_MOT0_CURRENT) |           \
                                     PIN_ODR_HIGH(GPIOA_MOT1_QEA) |             \
                                     PIN_ODR_LOW(GPIOA_MOT1_CURRENT) |             \
                                     PIN_ODR_HIGH(GPIOA_VSYS_ADC) |             \
                                     PIN_ODR_HIGH(GPIOA_DCMI_HSYNC) |             \
                                     PIN_ODR_HIGH(GPIOA_AUDIO_SPEAKER) |              \
                                     PIN_ODR_HIGH(GPIOA_DCMI_PIXCLK) |              \
                                     PIN_ODR_HIGH(GPIOA_AUDIO_MIC2) |              \
                                     PIN_ODR_HIGH(GPIOA_CAM_CLK) |             \
                                     PIN_ODR_HIGH(GPIOA_PIN9) |          \
                                     PIN_ODR_HIGH(GPIOA_PIN10) |        \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DM) |        \
                                     PIN_ODR_HIGH(GPIOA_OTG_FS_DP) |        \
                                     PIN_ODR_HIGH(GPIOA_SWDIO) |            \
                                     PIN_ODR_HIGH(GPIOA_SWCLK) |            \
                                     PIN_ODR_HIGH(GPIOA_MOT1_QEB))
#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_MOT0_CURRENT, 0) |         \
                                     PIN_AFIO_AF(GPIOA_MOT1_QEA, 1) |           \
                                     PIN_AFIO_AF(GPIOA_MOT1_CURRENT, 0) |           \
                                     PIN_AFIO_AF(GPIOA_VSYS_ADC, 0) |           \
                                     PIN_AFIO_AF(GPIOA_DCMI_HSYNC, 6) |           \
                                     PIN_AFIO_AF(GPIOA_AUDIO_SPEAKER, 5) |            \
                                     PIN_AFIO_AF(GPIOA_DCMI_PIXCLK, 5) |            \
                                     PIN_AFIO_AF(GPIOA_AUDIO_MIC2, 5))
#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_CAM_CLK, 0) |           \
                                     PIN_AFIO_AF(GPIOA_PIN9, 0) |        \
                                     PIN_AFIO_AF(GPIOA_PIN10, 10) |     \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DM, 10) |     \
                                     PIN_AFIO_AF(GPIOA_OTG_FS_DP, 10) |     \
                                     PIN_AFIO_AF(GPIOA_SWDIO, 0) |          \
                                     PIN_AFIO_AF(GPIOA_SWCLK, 0) |          \
                                     PIN_AFIO_AF(GPIOA_MOT1_QEB, 1))

/*
 * GPIOB setup:
 *
 */
#define VAL_GPIOB_MODER             (PIN_MODE_ALTERNATE(GPIOB_PWM0_PHASE_A) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_PWM0_PHASE_B) |           \
                                     PIN_MODE_INPUT(GPIOB_BOOT1) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI_CLK) |        \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI_MISO) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_SPI_MOSI) |           \
                                     PIN_MODE_INPUT(GPIOB_DCMI_D5) |        \
                                     PIN_MODE_INPUT(GPIOB_DCMI_VSYNC) |           \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C_SCLK) | \
                                     PIN_MODE_ALTERNATE(GPIOB_I2C_SDA) |  \
                                     PIN_MODE_INPUT(GPIOB_PIN10) |         \
                                     PIN_MODE_INPUT(GPIOB_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOB_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOB_PIN13) |          \
                                     PIN_MODE_ALTERNATE(GPIOB_IR_PWM) |          \
                                     PIN_MODE_INPUT(GPIOB_PIN15))
#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_PWM0_PHASE_A) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PWM0_PHASE_B) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BOOT1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI_CLK) |        \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI_MISO) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SPI_MOSI) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_DCMI_D5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOB_DCMI_VSYNC) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C_SCLK) | \
                                     PIN_OTYPE_OPENDRAIN(GPIOB_I2C_SDA) | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN10) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_IR_PWM) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN15))
#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_100M(GPIOB_PWM0_PHASE_A) |          \
                                     PIN_OSPEED_100M(GPIOB_PWM0_PHASE_B) |          \
                                     PIN_OSPEED_100M(GPIOB_BOOT1) |          \
                                     PIN_OSPEED_100M(GPIOB_SPI_CLK) |           \
                                     PIN_OSPEED_100M(GPIOB_SPI_MISO) |          \
                                     PIN_OSPEED_100M(GPIOB_SPI_MOSI) |          \
                                     PIN_OSPEED_100M(GPIOB_DCMI_D5) |           \
                                     PIN_OSPEED_100M(GPIOB_DCMI_VSYNC) |          \
                                     PIN_OSPEED_100M(GPIOB_I2C_SCLK) |          \
                                     PIN_OSPEED_100M(GPIOB_I2C_SDA) |           \
                                     PIN_OSPEED_100M(GPIOB_PIN10) |        \
                                     PIN_OSPEED_100M(GPIOB_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOB_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOB_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOB_IR_PWM) |         \
                                     PIN_OSPEED_100M(GPIOB_PIN15))
#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_PWM0_PHASE_A) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_PWM0_PHASE_B) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_BOOT1) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI_CLK) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI_MISO) |         \
                                     PIN_PUPDR_FLOATING(GPIOB_SPI_MOSI) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_DCMI_D5) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_DCMI_VSYNC) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C_SCLK) |         \
                                     PIN_PUPDR_PULLUP(GPIOB_I2C_SDA) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN10) |       \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN11) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN12) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN13) |        \
                                     PIN_PUPDR_FLOATING(GPIOB_IR_PWM) |        \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN15))
#define VAL_GPIOB_ODR               (PIN_ODR_LOW(GPIOB_PWM0_PHASE_A) |             \
                                     PIN_ODR_LOW(GPIOB_PWM0_PHASE_B) |             \
                                     PIN_ODR_HIGH(GPIOB_BOOT1) |             \
                                     PIN_ODR_HIGH(GPIOB_SPI_CLK) |              \
                                     PIN_ODR_HIGH(GPIOB_SPI_MISO) |             \
                                     PIN_ODR_HIGH(GPIOB_SPI_MOSI) |             \
                                     PIN_ODR_HIGH(GPIOB_DCMI_D5) |              \
                                     PIN_ODR_HIGH(GPIOB_DCMI_VSYNC) |             \
                                     PIN_ODR_HIGH(GPIOB_I2C_SCLK) |             \
                                     PIN_ODR_HIGH(GPIOB_I2C_SDA) |              \
                                     PIN_ODR_HIGH(GPIOB_PIN10) |           \
                                     PIN_ODR_HIGH(GPIOB_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOB_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOB_PIN13) |            \
                                     PIN_ODR_LOW(GPIOB_IR_PWM) |            \
                                     PIN_ODR_HIGH(GPIOB_PIN15))
#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_PWM0_PHASE_A, 2) |           \
                                     PIN_AFIO_AF(GPIOB_PWM0_PHASE_B, 2) |           \
                                     PIN_AFIO_AF(GPIOB_BOOT1, 0) |           \
                                     PIN_AFIO_AF(GPIOB_SPI_CLK, 5) |            \
                                     PIN_AFIO_AF(GPIOB_SPI_MISO, 5) |           \
                                     PIN_AFIO_AF(GPIOB_SPI_MOSI, 5) |           \
                                     PIN_AFIO_AF(GPIOB_DCMI_D5, 4) |            \
                                     PIN_AFIO_AF(GPIOB_DCMI_VSYNC, 0))
#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_I2C_SCLK, 4) |           \
                                     PIN_AFIO_AF(GPIOB_I2C_SDA, 4) |            \
                                     PIN_AFIO_AF(GPIOB_PIN10, 0) |         \
                                     PIN_AFIO_AF(GPIOB_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOB_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOB_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOB_IR_PWM, 3) |          \
                                     PIN_AFIO_AF(GPIOB_PIN15, 0))

/*
 * GPIOC setup:
 *
 */
#define VAL_GPIOC_MODER             (PIN_MODE_ANALOG(GPIOC_IR_AN8) | \
                                     PIN_MODE_ANALOG(GPIOC_IR_AN9) |           \
                                     PIN_MODE_ANALOG(GPIOC_IR_AN10) |           \
                                     PIN_MODE_ANALOG(GPIOC_IR_AN11) |        \
                                     PIN_MODE_ANALOG(GPIOC_IR_AN12) |           \
                                     PIN_MODE_INPUT(GPIOC_AUDIO_MIC1) |           \
                                     PIN_MODE_INPUT(GPIOC_DCMI_D0) |           \
                                     PIN_MODE_INPUT(GPIOC_DCMI_D1) |       \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D0) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D1) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D2) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_D3) |      \
                                     PIN_MODE_ALTERNATE(GPIOC_SDIO_CLK) |     \
                                     PIN_MODE_INPUT(GPIOC_CAM_STROBE) |          \
                                     PIN_MODE_INPUT(GPIOC_CAM_RST) |          \
                                     PIN_MODE_INPUT(GPIOC_CAM_PWDN))
#define VAL_GPIOC_OTYPER            (PIN_OTYPE_OPENDRAIN(GPIOC_IR_AN8) | \
                                     PIN_OTYPE_OPENDRAIN(GPIOC_IR_AN9) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOC_IR_AN10) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOC_IR_AN11) |    \
                                     PIN_OTYPE_OPENDRAIN(GPIOC_IR_AN12) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_AUDIO_MIC1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_DCMI_D0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_DCMI_D1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_D3) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_SDIO_CLK) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOC_CAM_STROBE) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_CAM_RST) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOC_CAM_PWDN))
#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_100M(GPIOC_IR_AN8) | \
                                     PIN_OSPEED_100M(GPIOC_IR_AN9) |          \
                                     PIN_OSPEED_100M(GPIOC_IR_AN10) |          \
                                     PIN_OSPEED_100M(GPIOC_IR_AN11) |       \
                                     PIN_OSPEED_100M(GPIOC_IR_AN12) |          \
                                     PIN_OSPEED_100M(GPIOC_AUDIO_MIC1) |          \
                                     PIN_OSPEED_100M(GPIOC_DCMI_D0) |          \
                                     PIN_OSPEED_100M(GPIOC_DCMI_D1) |          \
                                     PIN_OSPEED_100M(GPIOC_SDIO_D0) |          \
                                     PIN_OSPEED_100M(GPIOC_SDIO_D1) |          \
                                     PIN_OSPEED_100M(GPIOC_SDIO_D2) |          \
                                     PIN_OSPEED_100M(GPIOC_SDIO_D3) |         \
                                     PIN_OSPEED_100M(GPIOC_SDIO_CLK) |          \
                                     PIN_OSPEED_100M(GPIOC_CAM_STROBE) |         \
                                     PIN_OSPEED_100M(GPIOC_CAM_RST) |         \
                                     PIN_OSPEED_100M(GPIOC_CAM_PWDN))
#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_IR_AN8) | \
                                     PIN_PUPDR_FLOATING(GPIOC_IR_AN9) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_IR_AN10) |         \
                                     PIN_PUPDR_FLOATING(GPIOC_IR_AN11) |      \
                                     PIN_PUPDR_FLOATING(GPIOC_IR_AN12) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_AUDIO_MIC1) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_DCMI_D0) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_DCMI_D1) |       \
                                     PIN_PUPDR_PULLUP(GPIOC_SDIO_D0) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SDIO_D1) |         \
                                     PIN_PUPDR_PULLUP(GPIOC_SDIO_D2) |       \
                                     PIN_PUPDR_PULLUP(GPIOC_SDIO_D3) |        \
                                     PIN_PUPDR_PULLUP(GPIOC_SDIO_CLK) |       \
                                     PIN_PUPDR_PULLUP(GPIOC_CAM_STROBE) |        \
                                     PIN_PUPDR_PULLUP(GPIOC_CAM_RST) |        \
                                     PIN_PUPDR_PULLUP(GPIOC_CAM_PWDN))
#define VAL_GPIOC_ODR               (PIN_ODR_LOW(GPIOC_IR_AN8) |  \
                                     PIN_ODR_LOW(GPIOC_IR_AN9) |             \
                                     PIN_ODR_LOW(GPIOC_IR_AN10) |             \
                                     PIN_ODR_LOW(GPIOC_IR_AN11) |          \
                                     PIN_ODR_LOW(GPIOC_IR_AN12) |             \
                                     PIN_ODR_HIGH(GPIOC_AUDIO_MIC1) |             \
                                     PIN_ODR_HIGH(GPIOC_DCMI_D0) |             \
                                     PIN_ODR_HIGH(GPIOC_DCMI_D1) |             \
                                     PIN_ODR_HIGH(GPIOC_SDIO_D0) |             \
                                     PIN_ODR_HIGH(GPIOC_SDIO_D1) |             \
                                     PIN_ODR_HIGH(GPIOC_SDIO_D2) |             \
                                     PIN_ODR_HIGH(GPIOC_SDIO_D3) |            \
                                     PIN_ODR_HIGH(GPIOC_SDIO_CLK) |             \
                                     PIN_ODR_HIGH(GPIOC_CAM_STROBE) |            \
                                     PIN_ODR_HIGH(GPIOC_CAM_RST) |            \
                                     PIN_ODR_HIGH(GPIOC_CAM_PWDN))
#define VAL_GPIOC_AFRL              (PIN_AFIO_AF(GPIOC_IR_AN8, 0) | \
                                     PIN_AFIO_AF(GPIOC_IR_AN9, 0) |           \
                                     PIN_AFIO_AF(GPIOC_IR_AN10, 0) |           \
                                     PIN_AFIO_AF(GPIOC_IR_AN11, 0) |        \
                                     PIN_AFIO_AF(GPIOC_IR_AN12, 0) |           \
                                     PIN_AFIO_AF(GPIOC_AUDIO_MIC1, 0) |           \
                                     PIN_AFIO_AF(GPIOC_DCMI_D0, 0) |           \
                                     PIN_AFIO_AF(GPIOC_DCMI_D1, 6))
#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_SDIO_D0, 12) |           \
                                     PIN_AFIO_AF(GPIOC_SDIO_D1, 12) |           \
                                     PIN_AFIO_AF(GPIOC_SDIO_D2, 12) |           \
                                     PIN_AFIO_AF(GPIOC_SDIO_D3, 12) |          \
                                     PIN_AFIO_AF(GPIOC_SDIO_CLK, 12) |           \
                                     PIN_AFIO_AF(GPIOC_CAM_STROBE, 0) |          \
                                     PIN_AFIO_AF(GPIOC_CAM_RST, 0) |          \
                                     PIN_AFIO_AF(GPIOC_CAM_PWDN, 0))

/*
 * GPIOD setup:
 *
 */
#define VAL_GPIOD_MODER             (PIN_MODE_ALTERNATE(GPIOD_CAN_RX) | \
                                     PIN_MODE_ALTERNATE(GPIOD_CAN_TX) | \
                                     PIN_MODE_ALTERNATE(GPIOD_SDIO_CMD) |           \
                                     PIN_MODE_OUTPUT(GPIOD_2V8_ON) |           \
                                     PIN_MODE_INPUT(GPIOD_PIN4) |         \
                                     PIN_MODE_INPUT(GPIOD_CAM_GPIO_0) |   \
                                     PIN_MODE_INPUT(GPIOD_CAM_GPIO_1) |           \
                                     PIN_MODE_INPUT(GPIOD_CAM_GPIO_2) |           \
                                     PIN_MODE_INPUT(GPIOD_CAM_EXPST) |           \
                                     PIN_MODE_INPUT(GPIOD_CAM_FREX) |           \
                                     PIN_MODE_INPUT(GPIOD_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOD_PIN11) |          \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM1_PHASE_A) |  \
                                     PIN_MODE_ALTERNATE(GPIOD_PWM1_PHASE_B) |  \
                                     PIN_MODE_INPUT(GPIOD_LED_DRIVER_RST) |          \
                                     PIN_MODE_OUTPUT(GPIOD_LED_ERROR))
#define VAL_GPIOD_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOD_CAN_RX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAN_TX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_SDIO_CMD) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_2V8_ON) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN4) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAM_GPIO_0) | \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAM_GPIO_1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAM_GPIO_2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAM_EXPST) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_CAM_FREX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM1_PHASE_A) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_PWM1_PHASE_B) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_LED_DRIVER_RST) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOD_LED_ERROR))
#define VAL_GPIOD_OSPEEDR           (PIN_OSPEED_100M(GPIOD_CAN_RX) |          \
                                     PIN_OSPEED_100M(GPIOD_CAN_TX) |          \
                                     PIN_OSPEED_100M(GPIOD_SDIO_CMD) |          \
                                     PIN_OSPEED_100M(GPIOD_2V8_ON) |          \
                                     PIN_OSPEED_100M(GPIOD_PIN4) |         \
                                     PIN_OSPEED_100M(GPIOD_CAM_GPIO_0) |  \
                                     PIN_OSPEED_100M(GPIOD_CAM_GPIO_1) |          \
                                     PIN_OSPEED_100M(GPIOD_CAM_GPIO_2) |          \
                                     PIN_OSPEED_100M(GPIOD_CAM_EXPST) |          \
                                     PIN_OSPEED_100M(GPIOD_CAM_FREX) |          \
                                     PIN_OSPEED_100M(GPIOD_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOD_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOD_PWM1_PHASE_A) |          \
                                     PIN_OSPEED_100M(GPIOD_PWM1_PHASE_B) |          \
                                     PIN_OSPEED_100M(GPIOD_LED_DRIVER_RST) |          \
                                     PIN_OSPEED_100M(GPIOD_LED_ERROR))
#define VAL_GPIOD_PUPDR             (PIN_PUPDR_PULLUP(GPIOD_CAN_RX) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_CAN_TX) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_SDIO_CMD) |         \
                                     PIN_PUPDR_FLOATING(GPIOD_2V8_ON) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_PIN4) |      \
                                     PIN_PUPDR_PULLUP(GPIOD_CAM_GPIO_0) | \
                                     PIN_PUPDR_PULLUP(GPIOD_CAM_GPIO_1) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_CAM_GPIO_2) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_CAM_EXPST) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_CAM_FREX) |         \
                                     PIN_PUPDR_PULLUP(GPIOD_PIN10) |        \
                                     PIN_PUPDR_PULLUP(GPIOD_PIN11) |        \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM1_PHASE_A) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_PWM1_PHASE_B) |       \
                                     PIN_PUPDR_PULLUP(GPIOD_LED_DRIVER_RST) |       \
                                     PIN_PUPDR_FLOATING(GPIOD_LED_ERROR))
#define VAL_GPIOD_ODR               (PIN_ODR_HIGH(GPIOD_CAN_RX) |             \
                                     PIN_ODR_HIGH(GPIOD_CAN_TX) |             \
                                     PIN_ODR_HIGH(GPIOD_SDIO_CMD) |             \
                                     PIN_ODR_LOW(GPIOD_2V8_ON) |             \
                                     PIN_ODR_HIGH(GPIOD_PIN4) |            \
                                     PIN_ODR_HIGH(GPIOD_CAM_GPIO_0) |     \
                                     PIN_ODR_HIGH(GPIOD_CAM_GPIO_1) |             \
                                     PIN_ODR_HIGH(GPIOD_CAM_GPIO_2) |             \
                                     PIN_ODR_HIGH(GPIOD_CAM_EXPST) |             \
                                     PIN_ODR_HIGH(GPIOD_CAM_FREX) |             \
                                     PIN_ODR_HIGH(GPIOD_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOD_PIN11) |            \
                                     PIN_ODR_LOW(GPIOD_PWM1_PHASE_A) |              \
                                     PIN_ODR_LOW(GPIOD_PWM1_PHASE_B) |              \
                                     PIN_ODR_LOW(GPIOD_LED_DRIVER_RST) |              \
                                     PIN_ODR_LOW(GPIOD_LED_ERROR))
#define VAL_GPIOD_AFRL              (PIN_AFIO_AF(GPIOD_CAN_RX, 9) |           \
                                     PIN_AFIO_AF(GPIOD_CAN_TX, 9) |           \
                                     PIN_AFIO_AF(GPIOD_SDIO_CMD, 12) |           \
                                     PIN_AFIO_AF(GPIOD_2V8_ON, 0) |           \
                                     PIN_AFIO_AF(GPIOD_PIN4, 0) |          \
                                     PIN_AFIO_AF(GPIOD_CAM_GPIO_0, 0) |   \
                                     PIN_AFIO_AF(GPIOD_CAM_GPIO_1, 0) |           \
                                     PIN_AFIO_AF(GPIOD_CAM_GPIO_2, 0))
#define VAL_GPIOD_AFRH              (PIN_AFIO_AF(GPIOD_CAM_EXPST, 0) |           \
                                     PIN_AFIO_AF(GPIOD_CAM_FREX, 0) |           \
                                     PIN_AFIO_AF(GPIOD_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOD_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOD_PWM1_PHASE_A, 2) |           \
                                     PIN_AFIO_AF(GPIOD_PWM1_PHASE_B, 2) |           \
                                     PIN_AFIO_AF(GPIOD_LED_DRIVER_RST, 0) |           \
                                     PIN_AFIO_AF(GPIOD_LED_ERROR, 0))

/*
 * GPIOE setup:
 *
 */
#define VAL_GPIOE_MODER             (PIN_MODE_INPUT(GPIOE_DCMI_D2) |           \
                                     PIN_MODE_INPUT(GPIOE_DCMI_D3) |           \
                                     PIN_MODE_INPUT(GPIOE_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOE_PIN3) |        \
                                     PIN_MODE_INPUT(GPIOE_DCMI_D4) |           \
                                     PIN_MODE_INPUT(GPIOE_DCMI_D6) |           \
                                     PIN_MODE_INPUT(GPIOE_DCMI_D7) |           \
                                     PIN_MODE_OUTPUT(GPIOE_LED_HEARTBEAT) |           \
                                     PIN_MODE_OUTPUT(GPIOE_LED_STATUS) |           \
                                     PIN_MODE_ALTERNATE(GPIOE_MOT0_QEA) |           \
                                     PIN_MODE_OUTPUT(GPIOE_LED_SD) |          \
                                     PIN_MODE_ALTERNATE(GPIOE_MOT0_QEB) |          \
                                     PIN_MODE_INPUT(GPIOE_RF_GPIO0_1) |          \
                                     PIN_MODE_INPUT(GPIOE_RF_GPIO1_1) |          \
                                     PIN_MODE_INPUT(GPIOE_RF_GPIO0_2) |          \
                                     PIN_MODE_INPUT(GPIOE_RF_GPIO1_2))
#define VAL_GPIOE_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_PIN3) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_DCMI_D7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_HEARTBEAT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_STATUS) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MOT0_QEA) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOE_LED_SD) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_MOT0_QEB) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RF_GPIO0_1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RF_GPIO1_1) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RF_GPIO0_2) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOE_RF_GPIO1_2))
#define VAL_GPIOE_OSPEEDR           (PIN_OSPEED_100M(GPIOE_DCMI_D2) |          \
                                     PIN_OSPEED_100M(GPIOE_DCMI_D3) |          \
                                     PIN_OSPEED_100M(GPIOE_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOE_PIN3) |        \
                                     PIN_OSPEED_100M(GPIOE_DCMI_D4) |          \
                                     PIN_OSPEED_100M(GPIOE_DCMI_D6) |          \
                                     PIN_OSPEED_100M(GPIOE_DCMI_D7) |          \
                                     PIN_OSPEED_100M(GPIOE_LED_HEARTBEAT) |          \
                                     PIN_OSPEED_100M(GPIOE_LED_STATUS) |          \
                                     PIN_OSPEED_100M(GPIOE_MOT0_QEA) |          \
                                     PIN_OSPEED_100M(GPIOE_LED_SD) |         \
                                     PIN_OSPEED_100M(GPIOE_MOT0_QEB) |         \
                                     PIN_OSPEED_100M(GPIOE_RF_GPIO0_1) |         \
                                     PIN_OSPEED_100M(GPIOE_RF_GPIO1_1) |         \
                                     PIN_OSPEED_100M(GPIOE_RF_GPIO0_2) |         \
                                     PIN_OSPEED_100M(GPIOE_RF_GPIO1_2))
#define VAL_GPIOE_PUPDR             (PIN_PUPDR_PULLUP(GPIOE_DCMI_D2) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_DCMI_D3) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_PIN2) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_PIN3) |     \
                                     PIN_PUPDR_PULLUP(GPIOE_DCMI_D4) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_DCMI_D6) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_DCMI_D7) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_HEARTBEAT) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_STATUS) |       \
                                     PIN_PUPDR_PULLUP(GPIOE_MOT0_QEA) |       \
                                     PIN_PUPDR_FLOATING(GPIOE_LED_SD) |      \
                                     PIN_PUPDR_PULLUP(GPIOE_MOT0_QEB) |      \
                                     PIN_PUPDR_PULLUP(GPIOE_RF_GPIO0_1) |      \
                                     PIN_PUPDR_PULLUP(GPIOE_RF_GPIO1_1) |      \
                                     PIN_PUPDR_PULLUP(GPIOE_RF_GPIO0_2) |      \
                                     PIN_PUPDR_PULLUP(GPIOE_RF_GPIO1_2))
#define VAL_GPIOE_ODR               (PIN_ODR_HIGH(GPIOE_DCMI_D2) |             \
                                     PIN_ODR_HIGH(GPIOE_DCMI_D3) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOE_PIN3) |           \
                                     PIN_ODR_HIGH(GPIOE_DCMI_D4) |             \
                                     PIN_ODR_HIGH(GPIOE_DCMI_D6) |             \
                                     PIN_ODR_HIGH(GPIOE_DCMI_D7) |             \
                                     PIN_ODR_LOW(GPIOE_LED_HEARTBEAT) |             \
                                     PIN_ODR_LOW(GPIOE_LED_STATUS) |             \
                                     PIN_ODR_HIGH(GPIOE_MOT0_QEA) |             \
                                     PIN_ODR_LOW(GPIOE_LED_SD) |            \
                                     PIN_ODR_HIGH(GPIOE_MOT0_QEB) |            \
                                     PIN_ODR_HIGH(GPIOE_RF_GPIO0_1) |            \
                                     PIN_ODR_HIGH(GPIOE_RF_GPIO1_1) |            \
                                     PIN_ODR_HIGH(GPIOE_RF_GPIO0_2) |            \
                                     PIN_ODR_HIGH(GPIOE_RF_GPIO1_2))
#define VAL_GPIOE_AFRL              (PIN_AFIO_AF(GPIOE_DCMI_D2, 0) |           \
                                     PIN_AFIO_AF(GPIOE_DCMI_D3, 0) |           \
                                     PIN_AFIO_AF(GPIOE_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOE_PIN3, 0) |         \
                                     PIN_AFIO_AF(GPIOE_DCMI_D4, 0) |           \
                                     PIN_AFIO_AF(GPIOE_DCMI_D6, 0) |           \
                                     PIN_AFIO_AF(GPIOE_DCMI_D7, 0) |           \
                                     PIN_AFIO_AF(GPIOE_LED_HEARTBEAT, 0))
#define VAL_GPIOE_AFRH              (PIN_AFIO_AF(GPIOE_LED_STATUS, 0) |           \
                                     PIN_AFIO_AF(GPIOE_MOT0_QEA, 1) |           \
                                     PIN_AFIO_AF(GPIOE_LED_SD, 0) |          \
                                     PIN_AFIO_AF(GPIOE_MOT0_QEB, 1) |          \
                                     PIN_AFIO_AF(GPIOE_RF_GPIO0_1, 0) |          \
                                     PIN_AFIO_AF(GPIOE_RF_GPIO1_1, 0) |          \
                                     PIN_AFIO_AF(GPIOE_RF_GPIO0_2, 0) |          \
                                     PIN_AFIO_AF(GPIOE_RF_GPIO1_2, 0))

/*
 * GPIOF setup:
 *
 */
#define VAL_GPIOF_MODER             (PIN_MODE_INPUT(GPIOF_SD_DETECT) |           \
                                     PIN_MODE_OUTPUT(GPIOF_POWERON) |       \
                                     PIN_MODE_INPUT(GPIOF_PIN2) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN0) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN1) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN2) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN3) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN4) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN5) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN6) |           \
                                     PIN_MODE_ANALOG(GPIOF_IR_AN7) |          \
                                     PIN_MODE_INPUT(GPIOF_IMU_INT) |          \
                                     PIN_MODE_OUTPUT(GPIOF_MPU_FSYNC) |          \
                                     PIN_MODE_OUTPUT(GPIOF_MPU_CS) |          \
                                     PIN_MODE_INPUT(GPIOF_POWER_BUTTON) |          \
                                     PIN_MODE_INPUT(GPIOF_PIN15))
#define VAL_GPIOF_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOF_SD_DETECT) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOF_POWERON) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN2) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN0) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN1) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN2) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN3) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN4) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN5) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN6) |       \
                                     PIN_OTYPE_OPENDRAIN(GPIOF_IR_AN7) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_IMU_INT) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_MPU_FSYNC) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_MPU_CS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_POWER_BUTTON) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOF_PIN15))
#define VAL_GPIOF_OSPEEDR           (PIN_OSPEED_100M(GPIOF_SD_DETECT) |          \
                                     PIN_OSPEED_100M(GPIOF_POWERON) |       \
                                     PIN_OSPEED_100M(GPIOF_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN0) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN1) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN2) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN3) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN4) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN5) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN6) |          \
                                     PIN_OSPEED_100M(GPIOF_IR_AN7) |         \
                                     PIN_OSPEED_100M(GPIOF_IMU_INT) |         \
                                     PIN_OSPEED_100M(GPIOF_MPU_FSYNC) |         \
                                     PIN_OSPEED_100M(GPIOF_MPU_CS) |         \
                                     PIN_OSPEED_100M(GPIOF_POWER_BUTTON) |         \
                                     PIN_OSPEED_100M(GPIOF_PIN15))
#define VAL_GPIOF_PUPDR             (PIN_PUPDR_PULLUP(GPIOF_SD_DETECT) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_POWERON) |    \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN0) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN1) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN2) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN3) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN4) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN5) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN6) |       \
                                     PIN_PUPDR_FLOATING(GPIOF_IR_AN7) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_IMU_INT) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_MPU_FSYNC) |      \
                                     PIN_PUPDR_FLOATING(GPIOF_MPU_CS) |      \
                                     PIN_PUPDR_PULLUP(GPIOF_POWER_BUTTON) |      \
                                     PIN_PUPDR_PULLUP(GPIOF_PIN15))
#define VAL_GPIOF_ODR               (PIN_ODR_HIGH(GPIOF_SD_DETECT) |             \
                                     PIN_ODR_HIGH(GPIOF_POWERON) |          \
                                     PIN_ODR_HIGH(GPIOF_PIN2) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN0) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN1) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN2) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN3) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN4) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN5) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN6) |             \
                                     PIN_ODR_LOW(GPIOF_IR_AN7) |            \
                                     PIN_ODR_LOW(GPIOF_IMU_INT) |            \
                                     PIN_ODR_LOW(GPIOF_MPU_FSYNC) |            \
                                     PIN_ODR_HIGH(GPIOF_MPU_CS) |            \
                                     PIN_ODR_HIGH(GPIOF_POWER_BUTTON) |            \
                                     PIN_ODR_HIGH(GPIOF_PIN15))
#define VAL_GPIOF_AFRL              (PIN_AFIO_AF(GPIOF_SD_DETECT, 0) |           \
                                     PIN_AFIO_AF(GPIOF_POWERON, 0) |        \
                                     PIN_AFIO_AF(GPIOF_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN0, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN1, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN2, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN3, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN4, 0))
#define VAL_GPIOF_AFRH              (PIN_AFIO_AF(GPIOF_IR_AN5, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN6, 0) |           \
                                     PIN_AFIO_AF(GPIOF_IR_AN7, 0) |          \
                                     PIN_AFIO_AF(GPIOF_IMU_INT, 0) |          \
                                     PIN_AFIO_AF(GPIOF_MPU_FSYNC, 0) |          \
                                     PIN_AFIO_AF(GPIOF_MPU_CS, 0) |          \
                                     PIN_AFIO_AF(GPIOF_POWER_BUTTON, 0) |          \
                                     PIN_AFIO_AF(GPIOF_PIN15, 0))

/*
 * GPIOG setup:
 *
 */
#define VAL_GPIOG_MODER             (PIN_MODE_INPUT(GPIOG_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN8) |           \
                                     PIN_MODE_ALTERNATE(GPIOG_UART_RX) |           \
                                     PIN_MODE_INPUT(GPIOG_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOG_PIN11) |          \
                                     PIN_MODE_ALTERNATE(GPIOG_UART_RTS) |          \
                                     PIN_MODE_ALTERNATE(GPIOG_UART_CTS) |          \
                                     PIN_MODE_ALTERNATE(GPIOG_UART_TX) |          \
                                     PIN_MODE_INPUT(GPIOG_PIN15))
#define VAL_GPIOG_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOG_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_UART_RX) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_UART_RTS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_UART_CTS) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_UART_TX) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOG_PIN15))
#define VAL_GPIOG_OSPEEDR           (PIN_OSPEED_100M(GPIOG_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOG_UART_RX) |          \
                                     PIN_OSPEED_100M(GPIOG_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOG_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOG_UART_RTS) |         \
                                     PIN_OSPEED_100M(GPIOG_UART_CTS) |         \
                                     PIN_OSPEED_100M(GPIOG_UART_TX) |         \
                                     PIN_OSPEED_100M(GPIOG_PIN15))
#define VAL_GPIOG_PUPDR             (PIN_PUPDR_PULLUP(GPIOG_PIN0) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN1) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN2) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN3) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN4) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN5) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN6) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN7) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN8) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_UART_RX) |       \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN10) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN11) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_UART_RTS) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_UART_CTS) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_UART_TX) |      \
                                     PIN_PUPDR_PULLUP(GPIOG_PIN15))
#define VAL_GPIOG_ODR               (PIN_ODR_HIGH(GPIOG_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOG_UART_RX) |             \
                                     PIN_ODR_HIGH(GPIOG_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOG_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOG_UART_RTS) |            \
                                     PIN_ODR_HIGH(GPIOG_UART_CTS) |            \
                                     PIN_ODR_HIGH(GPIOG_UART_TX) |            \
                                     PIN_ODR_HIGH(GPIOG_PIN15))
#define VAL_GPIOG_AFRL              (PIN_AFIO_AF(GPIOG_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOG_PIN7, 0))
#define VAL_GPIOG_AFRH              (PIN_AFIO_AF(GPIOG_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOG_UART_RX, 8) |           \
                                     PIN_AFIO_AF(GPIOG_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOG_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOG_UART_RTS, 8) |          \
                                     PIN_AFIO_AF(GPIOG_UART_CTS, 8) |          \
                                     PIN_AFIO_AF(GPIOG_UART_TX, 8) |          \
                                     PIN_AFIO_AF(GPIOG_PIN15, 0))

/*
 * GPIOH setup:
 *
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN) |         \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT) |        \
                                     PIN_MODE_INPUT(GPIOH_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOH_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOH_PIN15))
#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN) |     \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT) |    \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOH_PIN15))
#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_100M(GPIOH_OSC_IN) |        \
                                     PIN_OSPEED_100M(GPIOH_OSC_OUT) |       \
                                     PIN_OSPEED_100M(GPIOH_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOH_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOH_PIN15))
#define VAL_GPIOH_PUPDR             (PIN_PUPDR_PULLUP(GPIOH_OSC_IN) |     \
                                     PIN_PUPDR_PULLUP(GPIOH_OSC_OUT) |    \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN2) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN3) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN4) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN5) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN6) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN7) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN8) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN9) |       \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN10) |      \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN11) |      \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN12) |      \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN13) |      \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN14) |      \
                                     PIN_PUPDR_PULLUP(GPIOH_PIN15))
#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN) |           \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT) |          \
                                     PIN_ODR_HIGH(GPIOH_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOH_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOH_PIN15))
#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0) |         \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0) |        \
                                     PIN_AFIO_AF(GPIOH_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN7, 0))
#define VAL_GPIOH_AFRH              (PIN_AFIO_AF(GPIOH_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOH_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOH_PIN15, 0))

/*
 * GPIOI setup:
 *
 */
#define VAL_GPIOI_MODER             (PIN_MODE_INPUT(GPIOI_PIN0) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN1) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN2) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN3) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN4) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN5) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN6) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN7) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN8) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN9) |           \
                                     PIN_MODE_INPUT(GPIOI_PIN10) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN11) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN12) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN13) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN14) |          \
                                     PIN_MODE_INPUT(GPIOI_PIN15))
#define VAL_GPIOI_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOI_PIN0) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN1) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN2) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN3) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN4) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN5) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN6) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN7) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN8) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN9) |       \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN10) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN11) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN12) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN13) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN14) |      \
                                     PIN_OTYPE_PUSHPULL(GPIOI_PIN15))
#define VAL_GPIOI_OSPEEDR           (PIN_OSPEED_100M(GPIOI_PIN0) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN1) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN2) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN3) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN4) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN5) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN6) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN7) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN8) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN9) |          \
                                     PIN_OSPEED_100M(GPIOI_PIN10) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN11) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN12) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN13) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN14) |         \
                                     PIN_OSPEED_100M(GPIOI_PIN15))
#define VAL_GPIOI_PUPDR             (PIN_PUPDR_PULLUP(GPIOI_PIN0) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN1) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN2) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN3) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN4) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN5) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN6) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN7) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN8) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN9) |       \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN10) |      \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN11) |      \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN12) |      \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN13) |      \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN14) |      \
                                     PIN_PUPDR_PULLUP(GPIOI_PIN15))
#define VAL_GPIOI_ODR               (PIN_ODR_HIGH(GPIOI_PIN0) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN1) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN2) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN3) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN4) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN5) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN6) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN7) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN8) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN9) |             \
                                     PIN_ODR_HIGH(GPIOI_PIN10) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN11) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN12) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN13) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN14) |            \
                                     PIN_ODR_HIGH(GPIOI_PIN15))
#define VAL_GPIOI_AFRL              (PIN_AFIO_AF(GPIOI_PIN0, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN1, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN2, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN3, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN4, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN5, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN6, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN7, 0))
#define VAL_GPIOI_AFRH              (PIN_AFIO_AF(GPIOI_PIN8, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN9, 0) |           \
                                     PIN_AFIO_AF(GPIOI_PIN10, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN11, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN12, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN13, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN14, 0) |          \
                                     PIN_AFIO_AF(GPIOI_PIN15, 0))


#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
void boardInit(void);

/** Shutdowns the board. */
void board_shutdown(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */

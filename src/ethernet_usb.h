/**
 * @file    ethernet_usb.h
 * @brief   ethernet over USB Driver macros and structures.
 *
 * @addtogroup ETHERNET_USB
 * @{
 */

#ifndef _ETHERNET_USB_H_
#define _ETHERNET_USB_H_

/*===========================================================================*/
/* Driver constants.                                                         */
/*===========================================================================*/

/**
 * @name    CDC specific messages.
 * @{
 */
#define CDC_SEND_ENCAPSULATED_COMMAND       0x00
#define CDC_GET_ENCAPSULATED_RESPONSE       0x01
#define CDC_SET_COMM_FEATURE                0x02
#define CDC_GET_COMM_FEATURE                0x03
#define CDC_CLEAR_COMM_FEATURE              0x04
#define CDC_SET_AUX_LINE_STATE              0x10
#define CDC_SET_HOOK_STATE                  0x11
#define CDC_PULSE_SETUP                     0x12
#define CDC_SEND_PULSE                      0x13
#define CDC_SET_PULSE_TIME                  0x14
#define CDC_RING_AUX_JACK                   0x15
#define CDC_SET_LINE_CODING                 0x20
#define CDC_GET_LINE_CODING                 0x21
#define CDC_SET_CONTROL_LINE_STATE          0x22
#define CDC_SEND_BREAK                      0x23
#define CDC_SET_RINGER_PARMS                0x30
#define CDC_GET_RINGER_PARMS                0x31
#define CDC_SET_OPERATION_PARMS             0x32
#define CDC_GET_OPERATION_PARMS             0x33
/** @} */

/**
 * @name    CDC classes
 * @{
 */
#define CDC_COMMUNICATION_INTERFACE_CLASS   0x02
#define CDC_DATA_INTERFACE_CLASS            0x0A
/** @} */

/**
 * @name    CDC subclasses
 * @{
 */
#define CDC_ABSTRACT_CONTROL_MODEL          0x02
/** @} */

/**
 * @name    CDC descriptors
 * @{
 */
#define CDC_CS_INTERFACE                    0x24
/** @} */

/**
 * @name    CDC subdescriptors
 * @{
 */
#define CDC_HEADER                          0x00
#define CDC_CALL_MANAGEMENT                 0x01
#define CDC_ABSTRACT_CONTROL_MANAGEMENT     0x02
#define CDC_UNION                           0x06
/** @} */

/**
 * @name    Line Control bit definitions.
 * @{
 */
#define LC_STOP_1                           0
#define LC_STOP_1P5                         1
#define LC_STOP_2                           2

#define LC_PARITY_NONE                      0
#define LC_PARITY_ODD                       1
#define LC_PARITY_EVEN                      2
#define LC_PARITY_MARK                      3
#define LC_PARITY_SPACE                     4
/** @} */

/*===========================================================================*/
/* Driver pre-compile time settings.                                         */
/*===========================================================================*/

/**
 * @name    ETHERNET_USB configuration options
 * @{
 */
/**
 * @brief   ethernet over USB buffers size.
 * @details Configuration parameter, the buffer size must be a multiple of
 *          the USB data endpoint maximum packet size.
 * @note    The default is 256 bytes for both the transmission and receive
 *          buffers.
 */
#if !defined(ETHERNET_USB_BUFFERS_SIZE) || defined(__DOXYGEN__)
#define ETHERNET_USB_BUFFERS_SIZE     3000
#endif
/** @} */

/*===========================================================================*/
/* Derived constants and error checks.                                       */
/*===========================================================================*/

#if HAL_USE_USB == FALSE
#error "ethernet over USB Driver requires HAL_USE_USB"
#endif

/*===========================================================================*/
/* Driver data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief Driver state machine possible states.
 */
typedef enum {
    EDU_UNINIT = 0,                 /**< Not initialized.                   */
    EDU_STOP = 1,                   /**< Stopped.                           */
    EDU_READY = 2                   /**< Ready.                             */
} edustate_t;

/**
 * @brief   Structure representing a ethernet over USB driver.
 */
typedef struct EthernetUSBDriver EthernetUSBDriver;

/**
 * @brief   ethernet over USB Driver configuration structure.
 * @details An instance of this structure must be passed to @p eduStart()
 *          in order to configure and start the driver operations.
 */
typedef struct {
    /**
     * @brief   USB driver to use.
     */
    USBDriver                 *usbp;
    /**
     * @brief   Bulk IN endpoint used for outgoing data transfer.
     */
    usbep_t bulk_in;
    /**
     * @brief   Bulk OUT endpoint used for incoming data transfer.
     */
    usbep_t bulk_out;
    /**
     * @brief   Interrupt IN endpoint used for notifications.
     * @note    If set to zero then the INT endpoint is assumed to be not
     *          present, USB descriptors must be changed accordingly.
     */
    usbep_t int_in;
} EthernetUSBConfig;

/**
 * @brief   @p ethernetDriver specific data.
 */
#define _ethernet_usb_driver_data                                             \
    _base_asynchronous_channel_data                                           \
    /* Driver state.*/                                                        \
    edustate_t state;                                          \
    /* Input queue.*/                                                         \
    input_queue_t iqueue;                                         \
    /* Output queue.*/                                                        \
    output_queue_t oqueue;                                         \
    /* Input buffer.*/                                                        \
    uint8_t ib[ETHERNET_USB_BUFFERS_SIZE];                    \
    /* Output buffer.*/                                                       \
    uint8_t ob[ETHERNET_USB_BUFFERS_SIZE];                    \
    /* End of the mandatory fields.*/                                         \
    /* Current configuration data.*/                                          \
    const EthernetUSBConfig     *config;

/**
 * @brief   @p EthernetUSBDriver specific methods.
 */
#define _ethernet_usb_driver_methods                                          \
    _base_asynchronous_channel_methods

/**
 * @extends BaseAsynchronousChannelVMT
 *
 * @brief   @p ethernetDriver virtual methods table.
 */
struct EthernetUSBDriverVMT {
    _ethernet_usb_driver_methods
};

/**
 * @extends BaseAsynchronousChannel
 *
 * @brief   Full duplex ethernet driver class.
 * @details This class extends @p BaseAsynchronousChannel by adding physical
 *          I/O queues.
 */
struct EthernetUSBDriver {
    /** @brief Virtual Methods Table.*/
    const struct EthernetUSBDriverVMT *vmt;
    _ethernet_usb_driver_data
};

/*===========================================================================*/
/* Driver macros.                                                            */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void eduInit(void);
void eduObjectInit(EthernetUSBDriver *edup);
void eduStart(EthernetUSBDriver *edup, const EthernetUSBConfig *config);
void eduStop(EthernetUSBDriver *edup);
void eduConfigureHookI(EthernetUSBDriver *edup);
bool eduRequestsHook(USBDriver *usbp);
void eduDataTransmitted(USBDriver *usbp, usbep_t ep);
void eduDataReceived(USBDriver *usbp, usbep_t ep);
void eduInterruptTransmitted(USBDriver *usbp, usbep_t ep);
#ifdef __cplusplus
}
#endif

#endif /* _ETHERNET_USB_H_ */

/** @} */

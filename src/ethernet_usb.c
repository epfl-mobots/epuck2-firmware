#include <hal.h>
#include "ethernet_usb.h"


static size_t write(void *ip, const uint8_t *bp, size_t n)
{

    return oqWriteTimeout(&((EthernetUSBDriver *)ip)->oqueue, bp,
                          n, TIME_INFINITE);
}

static size_t read(void *ip, uint8_t *bp, size_t n)
{

    return iqReadTimeout(&((EthernetUSBDriver *)ip)->iqueue, bp,
                         n, TIME_INFINITE);
}

static msg_t put(void *ip, uint8_t b)
{

    return oqPutTimeout(&((EthernetUSBDriver *)ip)->oqueue, b, TIME_INFINITE);
}

static msg_t get(void *ip)
{

    return iqGetTimeout(&((EthernetUSBDriver *)ip)->iqueue, TIME_INFINITE);
}

static msg_t putt(void *ip, uint8_t b, systime_t timeout)
{

    return oqPutTimeout(&((EthernetUSBDriver *)ip)->oqueue, b, timeout);
}

static msg_t gett(void *ip, systime_t timeout)
{

    return iqGetTimeout(&((EthernetUSBDriver *)ip)->iqueue, timeout);
}

static size_t writet(void *ip, const uint8_t *bp, size_t n, systime_t timeout)
{

    return oqWriteTimeout(&((EthernetUSBDriver *)ip)->oqueue, bp, n, timeout);
}

static size_t readt(void *ip, uint8_t *bp, size_t n, systime_t timeout)
{

    return iqReadTimeout(&((EthernetUSBDriver *)ip)->iqueue, bp, n, timeout);
}

static const struct EthernetUSBDriverVMT vmt = {
    write, read, put, get,
    putt, gett, writet, readt
};

/**
 * @brief   Notification of data removed from the input queue.
 *
 * @param[in] qp        the queue pointer.
 */
static void inotify(io_queue_t *qp)
{
    size_t n, maxsize;
    EthernetUSBDriver *edup = qGetLink(qp);

    /* If the USB driver is not in the appropriate state then transactions
       must not be started.*/
    if ((usbGetDriverStateI(edup->config->usbp) != USB_ACTIVE) ||
        (edup->state != EDU_READY)) {
        return;
    }

    /* If there is in the queue enough space to hold at least one packet and
       a transaction is not yet started then a new transaction is started for
       the available space.*/
    maxsize = edup->config->usbp->epc[edup->config->bulk_out]->out_maxsize;
    if (!usbGetReceiveStatusI(edup->config->usbp, edup->config->bulk_out)) {
        if ((n = iqGetEmptyI(&edup->iqueue)) >= maxsize) {
            osalSysUnlock();

            n = (n / maxsize) * maxsize;
            usbPrepareQueuedReceive(edup->config->usbp,
                                    edup->config->bulk_out,
                                    &edup->iqueue, n);

            osalSysLock();
            (void) usbStartReceiveI(edup->config->usbp, edup->config->bulk_out);
        }
    }
}

/**
 * @brief   Notification of data inserted into the output queue.
 *
 * @param[in] qp        the queue pointer.
 */
static void onotify(io_queue_t *qp)
{
    size_t n;
    EthernetUSBDriver *edup = qGetLink(qp);

    palTogglePad(GPIOE, GPIOE_LED_HEARTBEAT);

    /* If the USB driver is not in the appropriate state then transactions
       must not be started.*/
    if ((usbGetDriverStateI(edup->config->usbp) != USB_ACTIVE) ||
        (edup->state != EDU_READY)) {
        return;
    }

    /* If there is not an ongoing transaction and the output queue contains
       data then a new transaction is started.*/
    if (!usbGetTransmitStatusI(edup->config->usbp, edup->config->bulk_in)) {
        if ((n = oqGetFullI(&edup->oqueue)) > 0U) {
            osalSysUnlock();

            usbPrepareQueuedTransmit(edup->config->usbp,
                                     edup->config->bulk_in,
                                     &edup->oqueue, n);

            osalSysLock();
            (void) usbStartTransmitI(edup->config->usbp, edup->config->bulk_in);
        }
    }
}

/*===========================================================================*/
/* Driver exported functions.                                                */
/*===========================================================================*/

/**
 * @brief   ethernet Driver initialization.
 * @note    This function is implicitly invoked by @p halInit(), there is
 *          no need to explicitly initialize the driver.
 *
 * @init
 */
void eduInit(void)
{
}

/**
 * @brief   Initializes a generic full duplex driver object.
 * @details The HW dependent part of the initialization has to be performed
 *          outside, usually in the hardware initialization code.
 *
 * @param[out] edup     pointer to a @p EthernetUSBDriver structure
 *
 * @init
 */
void eduObjectInit(EthernetUSBDriver *edup)
{
    edup->vmt = &vmt;
    osalEventObjectInit(&edup->event);
    edup->state = EDU_STOP;
    iqObjectInit(&edup->iqueue, edup->ib, ETHERNET_USB_BUFFERS_SIZE, inotify, edup);
    oqObjectInit(&edup->oqueue, edup->ob, ETHERNET_USB_BUFFERS_SIZE, onotify, edup);
}

/**
 * @brief   Configures and starts the driver.
 *
 * @param[in] edup      pointer to a @p EthernetUSBDriver object
 * @param[in] config    the ethernet over USB driver configuration
 *
 * @api
 */
void eduStart(EthernetUSBDriver *edup, const EthernetUSBConfig *config)
{
    USBDriver *usbp = config->usbp;

    osalDbgCheck(edup != NULL);

    osalSysLock();
    osalDbgAssert((edup->state == EDU_STOP) || (edup->state == EDU_READY),
                  "invalid state");
    usbp->in_params[config->bulk_in - 1U]   = edup;
    usbp->out_params[config->bulk_out - 1U] = edup;
    if (config->int_in > 0U) {
        usbp->in_params[config->int_in - 1U]  = edup;
    }
    edup->config = config;
    edup->state = EDU_READY;
    osalSysUnlock();
}

/**
 * @brief   Stops the driver.
 * @details Any thread waiting on the driver's queues will be awakened with
 *          the message @p Q_RESET.
 *
 * @param[in] edup      pointer to a @p EthernetUSBDriver object
 *
 * @api
 */
void eduStop(EthernetUSBDriver *edup)
{
    USBDriver *usbp = edup->config->usbp;

    osalDbgCheck(edup != NULL);

    osalSysLock();
    osalDbgAssert((edup->state == EDU_STOP) || (edup->state == EDU_READY),
                  "invalid state");

    /* Driver in stopped state.*/
    usbp->in_params[edup->config->bulk_in - 1U]   = NULL;
    usbp->out_params[edup->config->bulk_out - 1U] = NULL;
    if (edup->config->int_in > 0U) {
        usbp->in_params[edup->config->int_in - 1U]  = NULL;
    }
    edup->state = EDU_STOP;

    /* Queues reset in order to signal the driver stop to the application.*/
    chnAddFlagsI(edup, CHN_DISCONNECTED);
    iqResetI(&edup->iqueue);
    iqResetI(&edup->oqueue);
    osalOsRescheduleS();
    osalSysUnlock();
}

/**
 * @brief   USB device configured handler.
 *
 * @param[in] edup      pointer to a @p EthernetUSBDriver object
 *
 * @iclass
 */
void eduConfigureHookI(EthernetUSBDriver *edup)
{
    USBDriver *usbp = edup->config->usbp;

    iqResetI(&edup->iqueue);
    oqResetI(&edup->oqueue);
    chnAddFlagsI(edup, CHN_CONNECTED);

    /* Starts the first OUT transaction immediately.*/
    usbPrepareQueuedReceive(usbp, edup->config->bulk_out, &edup->iqueue,
                            usbp->epc[edup->config->bulk_out]->out_maxsize);
    (void) usbStartReceiveI(usbp, edup->config->bulk_out);
}

/**
 * @brief   Default requests hook.
 * @details Applications wanting to use the ethernet over USB driver can use
 *          this function as requests hook in the USB configuration.
 *          The following requests are emulated:
 *          - CDC_GET_LINE_CODING.
 *          - CDC_SET_LINE_CODING.
 *          - CDC_SET_CONTROL_LINE_STATE.
 *          .
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @return              The hook status.
 * @retval true         Message handled internally.
 * @retval false        Message not handled.
 */
bool eduRequestsHook(USBDriver *usbp)
{

    if ((usbp->setup[0] & USB_RTYPE_TYPE_MASK) == USB_RTYPE_TYPE_CLASS) {
        switch (usbp->setup[1]) {
            default:
                return false;
        }
    }
    return false;
}

/**
 * @brief   Default data transmitted callback.
 * @details The application must use this function as callback for the IN
 *          data endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 */
void eduDataTransmitted(USBDriver *usbp, usbep_t ep)
{
    size_t n;
    EthernetUSBDriver *edup = usbp->in_params[ep - 1U];

    if (edup == NULL) {
        return;
    }

    osalSysLockFromISR();
    chnAddFlagsI(edup, CHN_OUTPUT_EMPTY);

    /*lint -save -e9013 [15.7] There is no else because it is not needed.*/
    if ((n = oqGetFullI(&edup->oqueue)) > 0U) {
        /* The endpoint cannot be busy, we are in the context of the callback,
           so it is safe to transmit without a check.*/
        osalSysUnlockFromISR();

        usbPrepareQueuedTransmit(usbp, ep, &edup->oqueue, n);

        osalSysLockFromISR();
        (void) usbStartTransmitI(usbp, ep);
    } else if ((usbp->epc[ep]->in_state->txsize > 0U) &&
               ((usbp->epc[ep]->in_state->txsize &
                 ((size_t)usbp->epc[ep]->in_maxsize - 1U)) == 0U)) {
        /* Transmit zero sized packet in case the last one has maximum allowed
           size. Otherwise the recipient may expect more data coming soon and
           not return buffered data to app. See section 5.8.3 Bulk Transfer
           Packet Size Constraints of the USB Specification document.*/
        osalSysUnlockFromISR();

        usbPrepareQueuedTransmit(usbp, ep, &edup->oqueue, 0);

        osalSysLockFromISR();
        (void) usbStartTransmitI(usbp, ep);
    }
    /*lint -restore*/

    osalSysUnlockFromISR();
}

/**
 * @brief   Default data received callback.
 * @details The application must use this function as callback for the OUT
 *          data endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 */
void eduDataReceived(USBDriver *usbp, usbep_t ep)
{
    size_t n, maxsize;
    EthernetUSBDriver *edup = usbp->out_params[ep - 1U];

    if (edup == NULL) {
        return;
    }

    osalSysLockFromISR();
    chnAddFlagsI(edup, CHN_INPUT_AVAILABLE);

    /* Writes to the input queue can only happen when there is enough space
       to hold at least one packet.*/
    maxsize = usbp->epc[ep]->out_maxsize;
    if ((n = iqGetEmptyI(&edup->iqueue)) >= maxsize) {
        /* The endpoint cannot be busy, we are in the context of the callback,
           so a packet is in the buffer for sure.*/
        osalSysUnlockFromISR();

        n = (n / maxsize) * maxsize;
        usbPrepareQueuedReceive(usbp, ep, &edup->iqueue, n);

        osalSysLockFromISR();
        (void) usbStartReceiveI(usbp, ep);
    }
    osalSysUnlockFromISR();
}

/**
 * @brief   Default data received callback.
 * @details The application must use this function as callback for the IN
 *          interrupt endpoint.
 *
 * @param[in] usbp      pointer to the @p USBDriver object
 * @param[in] ep        endpoint number
 */
void eduInterruptTransmitted(USBDriver *usbp, usbep_t ep)
{

    (void)usbp;
    (void)ep;
}

/** @} */

/**
  ******************************************************************************
  * @file    usbd_cdc.c
  * @author  MCD Application Team
  * @brief   This file provides the high layer firmware functions to manage the
  *          following functionalities of the USB CDC Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as CDC Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                CDC Class Driver Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as CDC device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *
  *           These aspects may be enriched or modified for a specific user application.
  *
  *            This driver doesn't implement the following aspects of the specification
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"
#include "usbd_ctlreq.h"
#include "usbd_composite.h"
#include "usbd_hid_common.h"


static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev,
                             uint8_t cfgidx);

static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req);

static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev,
                               uint8_t epnum);

static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);


/* CDC interface class callbacks structure */
USBD_ClassTypeDef USBD_CDC =
        {
                USBD_CDC_Init,
                USBD_CDC_DeInit,
                USBD_CDC_Setup,
                NULL,                 /* EP0_TxSent, */
                USBD_CDC_EP0_RxReady,
                USBD_CDC_DataIn,
                USBD_CDC_DataOut
        };

/**
  * @brief  USBD_CDC_Init
  *         Initialize the CDC interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_CDC_HandleTypeDef usbd_cdc_handle;

static uint8_t USBD_CDC_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    uint8_t ret = 0U;
    USBD_CDC_HandleTypeDef *hcdc;
    // static uint32_t mem[(sizeof(USBD_CDC_HandleTypeDef)/4+1)];/* On 32-bit boundary */
    /* Open EP IN */
    USBD_LL_OpenEP(pdev, CDC_IN_EP, USBD_EP_TYPE_BULK,
                   CDC_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, CDC_OUT_EP, USBD_EP_TYPE_BULK,
                   CDC_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 1U;
    /* Open Command IN EP */
    USBD_LL_OpenEP(pdev, CDC_CMD_EP, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
    pdev->ep_in[CDC_CMD_EP & 0xFU].is_used = 1U;
    memset(&usbd_cdc_handle, 0, sizeof(USBD_CDC_HandleTypeDef));
    // pdev->pClassData = USBD_malloc(sizeof(USBD_CDC_HandleTypeDef));
    // pdev->pClassData = (USBD_CDC_HandleTypeDef *)mem;
    pdev->pClassData = &usbd_cdc_handle;

    if (pdev->pClassData == NULL) {
        ret = 1U;
    } else {
        hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;

        /* Init  physical Interface components */
        ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->Init();

        /* Init Xfer states */
        hcdc->TxState = 0U;
        hcdc->RxState = 0U;
        USBD_LL_PrepareReceive(pdev, CDC_OUT_EP, hcdc->RxBuffer,
                               CDC_DATA_FS_OUT_PACKET_SIZE);
    }
    return ret;
}

/**
  * @brief  USBD_CDC_Init
  *         DeInitialize the CDC layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_CDC_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
    uint8_t ret = 0U;

    /* Close EP IN */
    USBD_LL_CloseEP(pdev, CDC_IN_EP);
    pdev->ep_in[CDC_IN_EP & 0xFU].is_used = 0U;

    /* Close EP OUT */
    USBD_LL_CloseEP(pdev, CDC_OUT_EP);
    pdev->ep_out[CDC_OUT_EP & 0xFU].is_used = 0U;

    /* Close Command IN EP */
    USBD_LL_CloseEP(pdev, CDC_CMD_EP);
    pdev->ep_in[CDC_CMD_EP & 0xFU].is_used = 0U;

    /* DeInit  physical Interface components */
    if (pdev->pClassData != NULL) {
        ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->DeInit();
        // USBD_free(pdev->pClassData);
        pdev->pClassData = NULL;
    }

    return ret;
}

/**
  * @brief  USBD_CDC_Setup
  *         Handle the CDC specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_CDC_Setup(USBD_HandleTypeDef *pdev,
                              USBD_SetupReqTypedef *req) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;

    uint8_t ifalt = 0U;
    uint16_t status_info = 0U;
    uint8_t ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        case USB_REQ_TYPE_CLASS :
            if (req->wLength) {
                if (req->bmRequest & 0x80U) {
                    ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->Control(req->bRequest,
                                                                       (uint8_t *) (void *) hcdc->data,
                                                                       req->wLength);

                    USBD_CtlSendData(pdev, (uint8_t *) (void *) hcdc->data, req->wLength);
                } else {
                    hcdc->CmdOpCode = req->bRequest;
                    hcdc->CmdLength = (uint8_t) req->wLength;

                    USBD_CtlPrepareRx(pdev, (uint8_t *) (void *) hcdc->data, req->wLength);
                }
            } else {
                ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->Control(req->bRequest,
                                                                   (uint8_t *) (void *) req, 0U);
            }
            break;

        case USB_REQ_TYPE_STANDARD:
            switch (req->bRequest) {
                case USB_REQ_GET_STATUS:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *) (void *) &status_info, 2U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_GET_INTERFACE:
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, &ifalt, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE:
                    if (pdev->dev_state != USBD_STATE_CONFIGURED) {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
            }
            break;

        default:
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
    }

    return ret;
}

/**
  * @brief  USBD_CDC_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_CDC_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    // USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef *)pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;
    PCD_HandleTypeDef *hpcd = pdev->pData;

    if (pdev->pClassData != NULL) {
        if ((pdev->ep_in[epnum].total_length > 0U) &&
            ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U)) {
            /* Update the packet total length */
            pdev->ep_in[epnum].total_length = 0U;

            /* Send ZLP */
            USBD_LL_Transmit(pdev, epnum, NULL, 0U);
        } else {
            hcdc->TxState = 0U;
        }
        return USBD_OK;
    } else {
        return USBD_FAIL;
    }
}

/**
  * @brief  USBD_CDC_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t USBD_CDC_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;

    /* Get the received data length */
    hcdc->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

    /* USB data will be immediately processed, this allow next USB traffic being
    NAKed till the end of the application Xfer */
    if (pdev->pClassData != NULL) {
        ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->Receive(hcdc->RxBuffer, &hcdc->RxLength);

        return USBD_OK;
    } else {
        return USBD_FAIL;
    }
}

/**
  * @brief  USBD_CDC_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;

    if ((pdev->pUserData != NULL) && (hcdc->CmdOpCode != 0xFFU)) {
        ((USBD_CDC_ItfTypeDef *) pdev->pUserData)->Control(hcdc->CmdOpCode,
                                                           (uint8_t *) (void *) hcdc->data,
                                                           (uint16_t) hcdc->CmdLength);
        hcdc->CmdOpCode = 0xFFU;

    }
    return USBD_OK;
}

/**
  * @brief  USBD_CDC_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetTxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff,
                             uint16_t length) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;

    hcdc->TxBuffer = pbuff;
    hcdc->TxLength = length;

    return USBD_OK;
}


/**
  * @brief  USBD_CDC_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t USBD_CDC_SetRxBuffer(USBD_HandleTypeDef *pdev,
                             uint8_t *pbuff) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;

    hcdc->RxBuffer = pbuff;

    return USBD_OK;
}

/**
  * @brief  USBD_CDC_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_TransmitPacket(USBD_HandleTypeDef *pdev) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;
    pdev->pClassData = pClassDataCDC;
    if (pdev->pClassData != NULL) {
        if (hcdc->TxState == 0U) {
            /* Tx Transfer in progress */
            hcdc->TxState = 1U;

            /* Update the packet total length */
            pdev->ep_in[CDC_IN_EP & 0xFU].total_length = hcdc->TxLength;

            /* Transmit next packet */
            USBD_LL_Transmit(pdev, CDC_IN_EP, hcdc->TxBuffer,
                             (uint16_t) hcdc->TxLength);

            return USBD_OK;
        } else {
            return USBD_BUSY;
        }
    } else {
        return USBD_FAIL;
    }
}


/**
  * @brief  USBD_CDC_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_CDC_ReceivePacket(USBD_HandleTypeDef *pdev) {
    // USBD_CDC_HandleTypeDef   *hcdc = (USBD_CDC_HandleTypeDef *) pdev->pClassData;
    USBD_CDC_HandleTypeDef *hcdc = &usbd_cdc_handle;
    pdev->pClassData = pClassDataCDC;
    /* Suspend or Resume USB Out process */
    if (pdev->pClassData != NULL) {

        USBD_LL_PrepareReceive(pdev,
                               CDC_OUT_EP,
                               hcdc->RxBuffer,
                               CDC_DATA_FS_OUT_PACKET_SIZE);
        return USBD_OK;
    } else {
        return USBD_FAIL;
    }
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

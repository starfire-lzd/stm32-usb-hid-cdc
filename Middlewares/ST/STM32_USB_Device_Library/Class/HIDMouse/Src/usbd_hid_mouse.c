/**
  ******************************************************************************
  * @file    usbd_customhid.c
  * @author  MCD Application Team
  * @brief   This file provides the CUSTOM_HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                CUSTOM_HID Class  Description
  *          ===================================================================
  *           This module manages the CUSTOM_HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (CUSTOM_HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - Usage Page : Generic Desktop
  *             - Usage : Vendor
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
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
#include "usbd_hid_mouse.h"
#include "usbd_ctlreq.h"
#include "usbd_hid_mouse_if.h"


static uint8_t USBD_HID_Mouse_Init(USBD_HandleTypeDef *pdev,
                                    uint8_t cfgidx);

static uint8_t USBD_HID_Mouse_DeInit(USBD_HandleTypeDef *pdev,
                                      uint8_t cfgidx);

static uint8_t USBD_HID_Mouse_Setup(USBD_HandleTypeDef *pdev,
                                     USBD_SetupReqTypedef *req);

static uint8_t USBD_HID_Mouse_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_HID_Mouse_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_HID_Mouse_EP0_RxReady(USBD_HandleTypeDef *pdev);
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_HID_Mouse =
        {
                USBD_HID_Mouse_Init,
                USBD_HID_Mouse_DeInit,
                USBD_HID_Mouse_Setup,
                NULL, /*EP0_TxSent*/
                USBD_HID_Mouse_EP0_RxReady, /*EP0_RxReady*/ /* STATUS STAGE IN */
                USBD_HID_Mouse_DataIn, /*DataIn*/
                USBD_HID_Mouse_DataOut
        };



/* USB HID_Mouse device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_HID_Mouse_Desc[USB_CUSTOM_HID_DESC_SIZ] __ALIGN_END =
        {
                /* 18 */
                0x09,         /*bLength: CUSTOM_HID Descriptor size*/
                CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
                0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
                0x01,
                0x00,         /*bCountryCode: Hardware target country*/
                0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
                0x22,         /*bDescriptorType*/
                USBD_HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
                0x00,
        };

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         Initialize the CUSTOM_HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static USBD_CUSTOM_HID_HandleTypeDef usbd_hid_mouse_handle;

// USBD_CUSTOM_HID_HandleTypeDef usbd_custom_hid_handle;
static uint8_t USBD_HID_Mouse_Init(USBD_HandleTypeDef *pdev,
                                    uint8_t cfgidx) {
    uint8_t ret = 0U;
    USBD_CUSTOM_HID_HandleTypeDef *hhid;

    /* Open EP IN */
    USBD_LL_OpenEP(pdev, HID_MOUSE_EPIN_ADDR, USBD_EP_TYPE_INTR,
                   HID_MOUSE_EPIN_SIZE);

    pdev->ep_in[HID_MOUSE_EPIN_ADDR & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, HID_MOUSE_EPOUT_ADDR, USBD_EP_TYPE_INTR,
                   HID_MOUSE_EPOUT_SIZE);

    pdev->ep_out[HID_MOUSE_EPOUT_ADDR & 0xFU].is_used = 1U;

    // pdev->pClassData = USBD_malloc(sizeof(USBD_CUSTOM_HID_HandleTypeDef));
    memset(&usbd_hid_mouse_handle, 0, sizeof(USBD_CUSTOM_HID_HandleTypeDef));
    // pdev->pClassData = (USBD_CUSTOM_HID_HandleTypeDef *)mem;
    pdev->pClassData = &usbd_hid_mouse_handle;

    if (pdev->pClassData == NULL) {
        ret = 1U;
    } else {
        hhid = (USBD_CUSTOM_HID_HandleTypeDef *) pdev->pClassData;

        hhid->state = CUSTOM_HID_IDLE;
        ((USBD_CUSTOM_HID_ItfTypeDef *) pdev->pUserData)->Init();

        /* Prepare Out endpoint to receive 1st packet */
        USBD_LL_PrepareReceive(pdev, HID_MOUSE_EPOUT_ADDR, hhid->Report_buf,
                               USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);
    }

    return ret;
}

/**
  * @brief  USBD_CUSTOM_HID_Init
  *         DeInitialize the CUSTOM_HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_HID_Mouse_DeInit(USBD_HandleTypeDef *pdev,
                                      uint8_t cfgidx) {
    /* Close CUSTOM_HID EP IN */
    USBD_LL_CloseEP(pdev, HID_MOUSE_EPIN_ADDR);
    pdev->ep_in[HID_MOUSE_EPIN_ADDR & 0xFU].is_used = 0U;

    /* Close CUSTOM_HID EP OUT */
    USBD_LL_CloseEP(pdev, HID_MOUSE_EPOUT_ADDR);
    pdev->ep_out[HID_MOUSE_EPOUT_ADDR & 0xFU].is_used = 0U;

    /* FRee allocated memory */
    if (pdev->pClassData != NULL) {
        ((USBD_CUSTOM_HID_ItfTypeDef *) pdev->pUserData)->DeInit();
        // USBD_free(pdev->pClassData);
        pdev->pClassData = NULL;
    }
    return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_Setup
  *         Handle the CUSTOM_HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_HID_Mouse_Setup(USBD_HandleTypeDef *pdev,
                                     USBD_SetupReqTypedef *req) {
    // USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = &usbd_hid_mouse_handle;

    uint16_t len = 0U;
    uint8_t *pbuf = NULL;
    uint16_t status_info = 0U;
    uint8_t ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_TYPE_MASK) {
        case USB_REQ_TYPE_CLASS :
            switch (req->bRequest) {
                case CUSTOM_HID_REQ_SET_PROTOCOL:
                    hhid->Protocol = (uint8_t) (req->wValue);
                    break;

                case CUSTOM_HID_REQ_GET_PROTOCOL:
                    USBD_CtlSendData(pdev, (uint8_t *) (void *) &hhid->Protocol, 1U);
                    break;

                case CUSTOM_HID_REQ_SET_IDLE:
                    hhid->IdleState = (uint8_t) (req->wValue >> 8);
                    break;

                case CUSTOM_HID_REQ_GET_IDLE:
                    USBD_CtlSendData(pdev, (uint8_t *) (void *) &hhid->IdleState, 1U);
                    break;

                case CUSTOM_HID_REQ_SET_REPORT:
                    hhid->IsReportAvailable = 1U;
                    USBD_CtlPrepareRx(pdev, hhid->Report_buf, req->wLength);
                    break;

                default:
                    USBD_CtlError(pdev, req);
                    ret = USBD_FAIL;
                    break;
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
                case USB_REQ_GET_DESCRIPTOR:
                    if (req->wValue >> 8 == CUSTOM_HID_REPORT_DESC) {
                        len = MIN(USBD_HID_MOUSE_REPORT_DESC_SIZE, req->wLength);
                        pbuf = ((USBD_CUSTOM_HID_ItfTypeDef *) pdev->pUserData)->pReport;
                    } else {
                        if (req->wValue >> 8 == CUSTOM_HID_DESCRIPTOR_TYPE) {
                            pbuf = USBD_HID_Mouse_Desc;
                            len = MIN(USB_CUSTOM_HID_DESC_SIZ, req->wLength);
                        }
                    }
                    USBD_CtlSendData(pdev, pbuf, len);
                    break;

                case USB_REQ_GET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        USBD_CtlSendData(pdev, (uint8_t *) (void *) &hhid->AltSetting, 1U);
                    } else {
                        USBD_CtlError(pdev, req);
                        ret = USBD_FAIL;
                    }
                    break;

                case USB_REQ_SET_INTERFACE :
                    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
                        hhid->AltSetting = (uint8_t) (req->wValue);
                    } else {
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
  * @brief  USBD_CUSTOM_HID_SendReport
  *         Send CUSTOM_HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_Mouse_SendReport(USBD_HandleTypeDef *pdev,
                                   uint8_t *report,
                                   uint16_t len) {
    // USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = &usbd_hid_mouse_handle;

    if (pdev->dev_state == USBD_STATE_CONFIGURED) {
        if (hhid->state == CUSTOM_HID_IDLE) {
            hhid->state = CUSTOM_HID_BUSY;
            USBD_LL_Transmit(pdev, HID_MOUSE_EPIN_ADDR, report, len);
        } else {
            return USBD_BUSY;
        }
    }
    return USBD_OK;
}


/**
  * @brief  USBD_CUSTOM_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_HID_Mouse_DataIn(USBD_HandleTypeDef *pdev,
                                      uint8_t epnum) {
    /* Ensure that the FIFO is empty before a new transfer, this condition could
    be caused by  a new transfer before the end of the previous transfer */
    ((USBD_CUSTOM_HID_HandleTypeDef *) pdev->pClassData)->state = CUSTOM_HID_IDLE;

    return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_HID_Mouse_DataOut(USBD_HandleTypeDef *pdev,
                                       uint8_t epnum) {

    // USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = &usbd_hid_mouse_handle;

    ((USBD_CUSTOM_HID_ItfTypeDef *) pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                               hhid->Report_buf[1]);

    USBD_LL_PrepareReceive(pdev, HID_MOUSE_EPOUT_ADDR, hhid->Report_buf,
                           USBD_CUSTOMHID_OUTREPORT_BUF_SIZE);

    return USBD_OK;
}

/**
  * @brief  USBD_CUSTOM_HID_EP0_RxReady
  *         Handles control request data.
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_HID_Mouse_EP0_RxReady(USBD_HandleTypeDef *pdev) {
    // USBD_CUSTOM_HID_HandleTypeDef     *hhid = (USBD_CUSTOM_HID_HandleTypeDef *)pdev->pClassData;
    USBD_CUSTOM_HID_HandleTypeDef *hhid = &usbd_hid_mouse_handle;

    if (hhid->IsReportAvailable == 1U) {
        ((USBD_CUSTOM_HID_ItfTypeDef *) pdev->pUserData)->OutEvent(hhid->Report_buf[0],
                                                                   hhid->Report_buf[1]);
        hhid->IsReportAvailable = 0U;
    }

    return USBD_OK;
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

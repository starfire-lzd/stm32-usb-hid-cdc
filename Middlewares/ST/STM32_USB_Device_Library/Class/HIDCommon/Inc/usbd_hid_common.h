/**
  ******************************************************************************
  * @file    usbd_customhid.h
  * @author  MCD Application Team
  * @brief   header file for the usbd_customhid.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HID_COMMON_H
#define __USB_HID_COMMON_H

#include  "usbd_ioreq.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#define USB_CUSTOM_HID_DESC_SIZ              9U
#define CUSTOM_HID_DESCRIPTOR_TYPE           0x21U


#ifndef USBD_CUSTOMHID_OUTREPORT_BUF_SIZE
#define USBD_CUSTOMHID_OUTREPORT_BUF_SIZE  0x02U
#endif /* USBD_CUSTOMHID_OUTREPORT_BUF_SIZE */


#define CUSTOM_HID_REPORT_DESC               0x22U

#define CUSTOM_HID_REQ_SET_PROTOCOL          0x0BU
#define CUSTOM_HID_REQ_GET_PROTOCOL          0x03U

#define CUSTOM_HID_REQ_SET_IDLE              0x0AU
#define CUSTOM_HID_REQ_GET_IDLE              0x02U

#define CUSTOM_HID_REQ_SET_REPORT            0x09U
#define CUSTOM_HID_REQ_GET_REPORT            0x01U

#ifndef CUSTOM_HID_FS_BINTERVAL
#define CUSTOM_HID_FS_BINTERVAL            0x05U
#endif /* CUSTOM_HID_FS_BINTERVAL */

// 端点定义
#define HID_MOUSE_EPIN_ADDR                 0x81U  // IN端点1
#define HID_MOUSE_EPIN_SIZE                 0x40U  // 端点大小64字节

#define HID_MOUSE_EPOUT_ADDR                0x01U  // OUT端点1
#define HID_MOUSE_EPOUT_SIZE                0x40U  // 端点大小64字节

#define HID_KEYBOARD_EPIN_ADDR              0x82U  // IN端点2
#define HID_KEYBOARD_EPIN_SIZE              0x40U  // 端点大小64字节

#define HID_KEYBOARD_EPOUT_ADDR             0x02U  // OUT端点2
#define HID_KEYBOARD_EPOUT_SIZE             0x40U  // 端点大小64字节

#define CDC_IN_EP                           0x83U  // IN端点3 (数据)
#define CDC_OUT_EP                          0x03U  // OUT端点3 (数据)
#define CDC_CMD_EP                          0x84U  // IN端点4 (命令)

#define CDC_DATA_FS_MAX_PACKET_SIZE                 64U  /* Endpoint IN & OUT Packet size */
#define CDC_CMD_PACKET_SIZE                         8U  /* Control Endpoint Packet size */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef enum {
    CUSTOM_HID_IDLE = 0U,
    CUSTOM_HID_BUSY,
}
        CUSTOM_HID_StateTypeDef;

typedef struct _USBD_CUSTOM_HID_Itf {
    uint8_t *pReport;

    int8_t (*Init)(void);

    int8_t (*DeInit)(void);

    int8_t (*OutEvent)(uint8_t event_idx, uint8_t state);

} USBD_CUSTOM_HID_ItfTypeDef;

typedef struct {
    uint8_t Report_buf[USBD_CUSTOMHID_OUTREPORT_BUF_SIZE];
    uint32_t Protocol;
    uint32_t IdleState;
    uint32_t AltSetting;
    uint32_t IsReportAvailable;
    CUSTOM_HID_StateTypeDef state;
} USBD_CUSTOM_HID_HandleTypeDef;


#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_COMMON_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

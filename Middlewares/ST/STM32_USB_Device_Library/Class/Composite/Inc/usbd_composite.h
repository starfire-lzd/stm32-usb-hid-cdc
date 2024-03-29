/**
  ******************************************************************************
  * @file    usbd_composite.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_composite.c file.
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
#ifndef __USB_COMPOSITE_H
#define __USB_COMPOSITE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

#define USBD_IAD_DESC_SIZE                        0x08
#define USBD_IAD_DESCRIPTOR_TYPE                  0x0B

#define COMPOSITE_EPIN_ADDR                 0x81
#define COMPOSITE_EPIN_SIZE                 0x10

#define USB_COMPOSITE_CONFIG_DESC_SIZ       139


enum {
    USBD_INTERFACE_CDC_CMD = 0X00,
    USBD_INTERFACE_CDC,
    USBD_INTERFACE_HID_MOUSE,
    USBD_INTERFACE_HID_KEYBOARD,
    USBD_INTERFACE_NUM
};

extern void *pClassDataCDC;
extern void *pClassDataHidMouse;
extern void *pClassDataHidKeyboard;


extern uint8_t data_in_flag;

extern USBD_ClassTypeDef USBD_COMPOSITE_ClassDriver;


#ifdef __cplusplus
}
#endif

#endif  /* __USB_COMPOSITE_CORE_H */

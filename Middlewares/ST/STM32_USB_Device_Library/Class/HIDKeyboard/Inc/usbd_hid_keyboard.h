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
#ifndef __USB_HID_KEYBOARD_H
#define __USB_HID_KEYBOARD_H

#include "usbd_hid_common.h"
#include "usbd_ioreq.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

extern USBD_ClassTypeDef USBD_HID_Keyboard;

uint8_t USBD_HID_Keyboard_SendReport(USBD_HandleTypeDef *pdev,
                                     uint8_t *report,
                                     uint16_t len);



/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_KEYBOARD_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

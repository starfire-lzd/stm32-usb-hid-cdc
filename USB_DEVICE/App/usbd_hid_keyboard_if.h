/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.h
  * @version        : v2.0_Cube
  * @brief          : Header for usbd_custom_hid_if.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_HID_KEYBOARD_IF_H__
#define __USBD_HID_KEYBOARD_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_keyboard.h"



#define USBD_HID_KEYBOARD_REPORT_DESC_SIZE     25

extern USBD_CUSTOM_HID_ItfTypeDef USBD_HidKeyboard_fops_FS;
#define USBD_HID_KEYBOARD_fops_FS USBD_HidKeyboard_fops_FS;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_HID_KEYBOARD_IF_H__ */


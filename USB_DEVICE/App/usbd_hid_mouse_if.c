/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usbd_custom_hid_if.c
 * @version        : v2.0_Cube
 * @brief          : USB Device Custom HID interface file.
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

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid_mouse_if.h"


/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t HID_Mouse_ReportDesc_FS[USBD_HID_MOUSE_REPORT_DESC_SIZE] __ALIGN_END =
        {
                0x05,   0x01,
                0x09,   0x02,
                0xA1,   0x01,
                0x09,   0x01,

                0xA1,   0x00,
                0x05,   0x09,
                0x19,   0x01,
                0x29,   0x03,

                0x15,   0x00,
                0x25,   0x01,
                0x95,   0x03,
                0x75,   0x01,

                0x81,   0x02,
                0x95,   0x01,
                0x75,   0x05,
                0x81,   0x01,

                0x05,   0x01,
                0x09,   0x30,
                0x09,   0x31,
                0x09,   0x38,

                0x15,   0x81,
                0x25,   0x7F,
                0x75,   0x08,
                0x95,   0x03,

                0x81,   0x06,
                0xC0,   0x09,
                0x3c,   0x05,
                0xff,   0x09,

                0x01,   0x15,
                0x00,   0x25,
                0x01,   0x75,
                0x01,   0x95,

                0x02,   0xb1,
                0x22,   0x75,
                0x06,   0x95,
                0x01,   0xb1,

                0x01,   0xc0
        };


extern USBD_HandleTypeDef hUsbDeviceFS;


static int8_t HID_Mouse_Init_FS(void);

static int8_t HID_Mouse_DeInit_FS(void);

static int8_t HID_Mouse_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
 * @}
 */

USBD_CUSTOM_HID_ItfTypeDef USBD_HidMouse_fops_FS =
        {
                HID_Mouse_ReportDesc_FS,
                HID_Mouse_Init_FS,
                HID_Mouse_DeInit_FS,
                HID_Mouse_OutEvent_FS};


/**
 * @brief  Initializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t HID_Mouse_Init_FS(void) {
    /* USER CODE BEGIN 4 */
    return (USBD_OK);
    /* USER CODE END 4 */
}

/**
 * @brief  DeInitializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t HID_Mouse_DeInit_FS(void) {
    /* USER CODE BEGIN 5 */
    return (USBD_OK);
    /* USER CODE END 5 */
}

/**
 * @brief  Manage the CUSTOM HID class events
 * @param  event_idx: Event index
 * @param  state: Event state
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t HID_Mouse_OutEvent_FS(uint8_t event_idx, uint8_t state) {
    /* USER CODE BEGIN 6 */
    switch (event_idx) {
        case 1:
            if ((state & 0x01) == 0x01)
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
            else {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
            }
            if (((state >> 1) & 0x01) == 0x01) {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
            } else {
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
            }
            break;
        default:
            break;
    }

    return (USBD_OK);
    /* USER CODE END 6 */
}


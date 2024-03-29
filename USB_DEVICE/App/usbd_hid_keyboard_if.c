/* USER CODE BEGIN Header */
#include "usbd_hid_keyboard_if.h"


/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t HID_Keyboard_ReportDesc_FS[USBD_HID_KEYBOARD_REPORT_DESC_SIZE] __ALIGN_END =
    {
            /* USER CODE BEGIN 0 */
            0x05, 0x01, // USAGE_PAGE (Generic Desktop)
            0x09, 0x06, // USAGE (HIDKeyboard)
            0xa1, 0x01, // COLLECTION (Application)
            0x85, 0x02, //   REPORT_ID (1)
            0x05, 0x07, // USAGE_PAGE (HIDKeyboard)
            0x19, 0xe0, // USAGE_MINIMUM (HIDKeyboard LeftControl)
            0x29, 0xe7, // USAGE_MAXIMUM (HIDKeyboard Right GUI)
            0x15, 0x00, // LOGICAL_MINIMUM (0)
            0x25, 0x01, // LOGICAL_MAXIMUM (1)

            0x75, 0x01, // REPORT_SIZE (1)
            0x95, 0x08, // REPORT_COUNT (8)
            0x81, 0x02, // INPUT (Data,Var,Abs) - Main keyboard input

            /* USER CODE END 0 */
            0xC0 // END_COLLECTION
};

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
 * @brief Public variables.
 * @{
 */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
 * @}
 */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
 * @brief Private functions declaration.
 * @{
 */

static int8_t HID_Keyboard_Init_FS(void);
static int8_t HID_Keyboard_DeInit_FS(void);
static int8_t HID_Keyboard_OutEvent_FS(uint8_t event_idx, uint8_t state);

/**
 * @}
 */

USBD_CUSTOM_HID_ItfTypeDef USBD_HidKeyboard_fops_FS =
    {
            HID_Keyboard_ReportDesc_FS,
        HID_Keyboard_Init_FS,
        HID_Keyboard_DeInit_FS,
        HID_Keyboard_OutEvent_FS};

/** @defgroup USBD_HID_Keyboard_Private_Functions USBD_HID_Keyboard_Private_Functions
 * @brief Private functions.
 * @{
 */

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Initializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t HID_Keyboard_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
 * @brief  DeInitializes the CUSTOM HID media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t HID_Keyboard_DeInit_FS(void)
{
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
static int8_t HID_Keyboard_OutEvent_FS(uint8_t event_idx, uint8_t state)
{
  /* USER CODE BEGIN 6 */
  switch (event_idx)
  {
  case 1:
    if ((state & 0x01) == 0x01)
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
    else
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_RESET);
    }
    if (((state >> 1) & 0x01) == 0x01)
    {
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
    }
    else
    {
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

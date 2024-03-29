/* Includes ------------------------------------------------------------------*/
#include "usbd_composite.h"
#include "usbd_ctlreq.h"

#include "usbd_cdc.h"
#include "usbd_hid_keyboard_if.h"
#include "usbd_hid_mouse_if.h"

void *pClassDataCDC = NULL;
void *pClassDataHidMouse = NULL;
void *pClassDataHidKeyboard = NULL;

extern USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS;
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

#define USBD_CDC_fops_FS USBD_Interface_fops_FS;
#define USBD_HID_fops_FS USBD_CustomHID_fops_FS;


static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev,
                                   uint8_t cfgidx);

static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev,
                                     uint8_t cfgidx);

static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev,
                                    USBD_SetupReqTypedef *req);

static uint8_t *USBD_COMPOSITE_GetCfgDesc(uint16_t *length);

static uint8_t *USBD_COMPOSITE_GetDeviceQualifierDesc(uint16_t *length);

static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev);


/**
  * @}
  */

/** @defgroup USBD_COMPOSITE_Private_Variables
  * @{
  */

USBD_ClassTypeDef USBD_COMPOSITE_ClassDriver =
        {
                USBD_COMPOSITE_Init,
                USBD_COMPOSITE_DeInit,
                USBD_COMPOSITE_Setup,
                NULL,//USBD_COMPOSITE_EP0_TxReady,
                USBD_COMPOSITE_EP0_RxReady,
                USBD_COMPOSITE_DataIn,
                USBD_COMPOSITE_DataOut,
                NULL,//USBD_COMPOSITE_SOF,
                NULL,//USBD_COMPOSITE_IsoINIncomplete,
                NULL,//USBD_COMPOSITE_IsoOutIncomplete,
                NULL,//USBD_COMPOSITE_GetCfgDesc,
                USBD_COMPOSITE_GetCfgDesc,
                NULL,//USBD_COMPOSITE_GetCfgDesc,
                USBD_COMPOSITE_GetDeviceQualifierDesc,
        };

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
/* USB COMPOSITE device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_COMPOSITE_CfgDesc[USB_COMPOSITE_CONFIG_DESC_SIZ] __ALIGN_END =
        {
                0x09,                        /* bLength: Configuration Descriptor size */
                USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
                USB_COMPOSITE_CONFIG_DESC_SIZ,
                /* wTotalLength: Bytes returned */
                0x00,
                USBD_INTERFACE_NUM, /*bNumInterfaces: 1 interface*/
                0x01,               /*bConfigurationValue: Configuration value*/
                0x00,               /*iConfiguration: Index of string descriptor describing the configuration*/
                0x80,               /*bmAttributes: bus powered */
                0x64,               /*MaxPower 100 mA: this current is used for detecting Vbus*/

                /******** /IAD should be positioned just before the CDC interfaces ******
                          IAD to associate the two CDC interfaces */

                USBD_IAD_DESC_SIZE,       /* bLength */
                USBD_IAD_DESCRIPTOR_TYPE, /* bDescriptorType IAD描述符类型*/
                0x00,                     /* bFirstInterface 接口描述符是要在总的配置描述符中的第几个 0开始数*/
                0x02,                     /* bInterfaceCount 接口描述符数量*/
                0x02,                     /* bFunctionClass 设备中的bDeviceClass*/
                0x02,                     /* bFunctionSubClass 设备符中的bDeviceSubClass*/
                0x01,                     /* bFunctionProtocol 设备符中的bDevicePreotocol*/
                0x00,                     /* iFunction (Index of string descriptor describing this function) */
                /**/

                /*---------------------------------------------------------------------------*/
                /*Interface Descriptor */
                0x09,                    /* bLength: Interface Descriptor size */
                USB_DESC_TYPE_INTERFACE, /* bDescriptorType: Interface */
                /* Interface descriptor type */
                USBD_INTERFACE_CDC_CMD, /* bInterfaceNumber: Number of Interface */
                0x00,                   /* bAlternateSetting: Alternate setting */
                0x01,                   /* bNumEndpoints: One endpoints used */
                0x02,                   /* bInterfaceClass: Communication Interface Class */
                0x02,                   /* bInterfaceSubClass: Abstract Control Model */
                0x01,                   /* bInterfaceProtocol: Common AT commands */
                0x00,                   /* iInterface: */

                /*Header Functional Descriptor*/
                0x05, /* bLength: Endpoint Descriptor size */
                0x24, /* bDescriptorType: CS_INTERFACE */
                0x00, /* bDescriptorSubtype: Header Func Desc */
                0x10, /* bcdCDC: spec release number */
                0x01,

                /*Call Management Functional Descriptor*/
                0x05, /* bFunctionLength */
                0x24, /* bDescriptorType: CS_INTERFACE */
                0x01, /* bDescriptorSubtype: Call Management Func Desc */
                0x00, /* bmCapabilities: D0+D1 */
                0x01, /* bDataInterface: 1 */

                /*ACM Functional Descriptor*/
                0x04, /* bFunctionLength */
                0x24, /* bDescriptorType: CS_INTERFACE */
                0x02, /* bDescriptorSubtype: Abstract Control Management desc */
                0x02, /* bmCapabilities */

                /*Union Functional Descriptor*/
                0x05, /* bFunctionLength */
                0x24, /* bDescriptorType: CS_INTERFACE */
                0x06, /* bDescriptorSubtype: Union func desc */
                0x00, /* bMasterInterface: Communication class interface */
                0x01, /* bSlaveInterface0: Data Class Interface */

                /*Endpoint 2 Descriptor*/
                0x07,                        /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
                CDC_CMD_EP,                  /* bEndpointAddress */
                0x03,                        /* bmAttributes: Interrupt */
                LOBYTE(CDC_CMD_PACKET_SIZE), /* wMaxPacketSize: */
                HIBYTE(CDC_CMD_PACKET_SIZE),
                CDC_HS_BINTERVAL, /* bInterval: */
                /*---------------------------------------------------------------------------*/

                /*Data class interface descriptor*/
                0x09,                    /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_INTERFACE, /* bDescriptorType: */
                USBD_INTERFACE_CDC,      /* bInterfaceNumber: Number of Interface */
                0x00,                    /* bAlternateSetting: Alternate setting */
                0x02,                    /* bNumEndpoints: Two endpoints used */
                0x0A,                    /* bInterfaceClass: CDC */
                0x00,                    /* bInterfaceSubClass: */
                0x00,                    /* bInterfaceProtocol: */
                0x00,                    /* iInterface: */

                /*Endpoint OUT Descriptor*/
                0x07,                                /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
                CDC_OUT_EP,                          /* bEndpointAddress */
                0x02,                                /* bmAttributes: Bulk */
                LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
                HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
                0x00, /* bInterval: ignore for Bulk transfer */

                /*Endpoint IN Descriptor*/
                0x07,                                /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_ENDPOINT,              /* bDescriptorType: Endpoint */
                CDC_IN_EP,                           /* bEndpointAddress */
                0x02,                                /* bmAttributes: Bulk */
                LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
                HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
                0x00, /* bInterval: ignore for Bulk transfer */
                /*107*/

                // HID Mouse
                /************** Descriptor of CUSTOM HID interface ****************/
                /* 09 */
                0x09,         /*bLength: Interface Descriptor size*/
                USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
                USBD_INTERFACE_HID_MOUSE,          /*bInterfaceNumber: Number of Interface*/
                0x00,         /*bAlternateSetting: Alternate setting*/
                0x02,         /*bNumEndpoints*/
                0x03,         /*bInterfaceClass: CUSTOM_HID*/
                0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
                0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
                0,            /*iInterface: Index of string descriptor*/
                /******************** Descriptor of CUSTOM_HID *************************/
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
                /******************** Descriptor of Custom HID endpoints ********************/
                /* 27 */
                0x07,          /*bLength: Endpoint Descriptor size*/
                USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
                HID_MOUSE_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
                0x03,          /*bmAttributes: Interrupt endpoint*/
                HID_MOUSE_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
                0x00,
                CUSTOM_HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
                /* 34 */

                0x07,          /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
                HID_MOUSE_EPOUT_ADDR,  /*bEndpointAddress: Endpoint Address (OUT)*/
                0x03, /* bmAttributes: Interrupt endpoint */
                HID_MOUSE_EPOUT_SIZE,  /* wMaxPacketSize: 2 Bytes max  */
                0x00,
                CUSTOM_HID_FS_BINTERVAL,  /* bInterval: Polling Interval */
                /* 41 */


                // HID Keyboard
                /************** Descriptor of CUSTOM HID interface ****************/
                /* 09 */
                0x09,         /*bLength: Interface Descriptor size*/
                USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
                USBD_INTERFACE_HID_KEYBOARD,          /*bInterfaceNumber: Number of Interface*/
                0x00,         /*bAlternateSetting: Alternate setting*/
                0x02,         /*bNumEndpoints*/
                0x03,         /*bInterfaceClass: CUSTOM_HID*/
                0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
                0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
                0,            /*iInterface: Index of string descriptor*/
                /******************** Descriptor of CUSTOM_HID *************************/
                /* 18 */
                0x09,         /*bLength: CUSTOM_HID Descriptor size*/
                CUSTOM_HID_DESCRIPTOR_TYPE, /*bDescriptorType: CUSTOM_HID*/
                0x11,         /*bCUSTOM_HIDUSTOM_HID: CUSTOM_HID Class Spec release number*/
                0x01,
                0x00,         /*bCountryCode: Hardware target country*/
                0x01,         /*bNumDescriptors: Number of CUSTOM_HID class descriptors to follow*/
                0x22,         /*bDescriptorType*/
                USBD_HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
                0x00,
                /******************** Descriptor of Custom HID endpoints ********************/
                /* 27 */
                0x07,          /*bLength: Endpoint Descriptor size*/
                USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

                HID_KEYBOARD_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
                0x03,          /*bmAttributes: Interrupt endpoint*/
                HID_KEYBOARD_EPIN_SIZE, /*wMaxPacketSize: 2 Byte max */
                0x00,
                CUSTOM_HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
                /* 34 */

                0x07,          /* bLength: Endpoint Descriptor size */
                USB_DESC_TYPE_ENDPOINT, /* bDescriptorType: */
                HID_KEYBOARD_EPOUT_ADDR,  /*bEndpointAddress: Endpoint Address (OUT)*/
                0x03, /* bmAttributes: Interrupt endpoint */
                HID_KEYBOARD_EPOUT_SIZE,  /* wMaxPacketSize: 2 Bytes max  */
                0x00,
                CUSTOM_HID_FS_BINTERVAL,  /* bInterval: Polling Interval */
        };

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment=4
#endif
/* USB Standard Device Descriptor */
static uint8_t USBD_COMPOSITE_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
        {
                USB_LEN_DEV_QUALIFIER_DESC,
                USB_DESC_TYPE_DEVICE_QUALIFIER,
                0x00,
                0x02,
                0x00,
                0x00,
                0x00,
                0x40,
                0x01,
                0x00,
        };


/**
  * @brief  USBD_COMPOSITE_Init
  *         Initialize the COMPOSITE interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_Init(USBD_HandleTypeDef *pdev,
                                   uint8_t cfgidx) {
    uint8_t ret = 0;
    /*CDC*/
    pdev->pUserData = (void *) &USBD_CDC_fops_FS;
    ret += USBD_CDC.Init(pdev, cfgidx);
    pClassDataCDC = pdev->pClassData;

    /*HID Mouse*/
    pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
    ret += USBD_HID_Keyboard.Init(pdev, cfgidx);
    pClassDataHidKeyboard = pdev->pClassData;

    /*HID Keyboard*/
    pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
    ret += USBD_HID_Mouse.Init(pdev, cfgidx);
    pClassDataHidMouse = pdev->pClassData;
    return ret;
}

/**
  * @brief  USBD_COMPOSITE_Init
  *         DeInitialize the COMPOSITE layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DeInit(USBD_HandleTypeDef *pdev,
                                     uint8_t cfgidx) {
    pdev->pClassData = pClassDataCDC;
    pdev->pUserData = (void *) &USBD_CDC_fops_FS;
    USBD_CDC.DeInit(pdev, cfgidx);

    pdev->pClassData = pClassDataHidKeyboard;
    pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
    USBD_HID_Keyboard.DeInit(pdev, cfgidx);

    pdev->pClassData = pClassDataHidMouse;
    pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
    USBD_HID_Mouse.DeInit(pdev, cfgidx);

    return USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_Setup
  *         Handle the COMPOSITE specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t USBD_COMPOSITE_Setup(USBD_HandleTypeDef *pdev,
                                    USBD_SetupReqTypedef *req) {
    USBD_StatusTypeDef ret = USBD_OK;

    switch (req->bmRequest & USB_REQ_RECIPIENT_MASK) {
        case USB_REQ_RECIPIENT_INTERFACE:
            switch (req->wIndex) {
                case USBD_INTERFACE_CDC:
                case USBD_INTERFACE_CDC_CMD:
                    pdev->pClassData = pClassDataCDC;
                    pdev->pUserData = (void *) &USBD_CDC_fops_FS;
                    return (USBD_CDC.Setup(pdev, req));
                case USBD_INTERFACE_HID_MOUSE:
                    pdev->pClassData = pClassDataHidMouse;
                    pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
                    return (USBD_HID_Mouse.Setup(pdev, req));
                case USBD_INTERFACE_HID_KEYBOARD:
                    pdev->pClassData = pClassDataHidKeyboard;
                    pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
                    return (USBD_HID_Keyboard.Setup(pdev, req));
                default:
                    break;
            }
            break;
        case USB_REQ_RECIPIENT_ENDPOINT:
            switch (req->wIndex) {
                case CDC_IN_EP:
                case CDC_OUT_EP:
                case CDC_CMD_EP:
                    pdev->pClassData = pClassDataCDC;
                    pdev->pUserData = (void *) &USBD_CDC_fops_FS;
                    return (USBD_CDC.Setup(pdev, req));
                case HID_MOUSE_EPIN_ADDR:
                case HID_MOUSE_EPOUT_ADDR:
                    pdev->pClassData = pClassDataHidMouse;
                    pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
                    return (USBD_HID_Mouse.Setup(pdev, req));
                case HID_KEYBOARD_EPIN_ADDR:
                case HID_KEYBOARD_EPOUT_ADDR:
                    pdev->pClassData = pClassDataHidKeyboard;
                    pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
                    return (USBD_HID_Keyboard.Setup(pdev, req));

                default:
                    break;
            }
            break;
    }
    return ret;
}


/**
  * @brief  USBD_COMPOSITE_GetCfgDesc
  *         return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_COMPOSITE_GetCfgDesc(uint16_t *length) {
    *length = sizeof(USBD_COMPOSITE_CfgDesc);
    return USBD_COMPOSITE_CfgDesc;
}


/**
  * @brief  USBD_COMPOSITE_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DataIn(USBD_HandleTypeDef *pdev,
                                     uint8_t epnum) {
    switch (epnum) {
        case (CDC_IN_EP & 0x0f):
            pdev->pClassData = pClassDataCDC;
            pdev->pUserData = (void *) &USBD_CDC_fops_FS;
            USBD_CDC.DataIn(pdev, epnum);
            break;
        case (HID_MOUSE_EPIN_ADDR & 0x0f):
            pdev->pClassData = pClassDataHidMouse;
            pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
            USBD_HID_Mouse.DataIn(pdev, epnum);
            break;
        case (HID_KEYBOARD_EPIN_ADDR & 0x0f):
            pdev->pClassData = pClassDataHidKeyboard;
            pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
            USBD_HID_Keyboard.DataIn(pdev, epnum);
            break;
        default:
            break;
    }
    return USBD_FAIL;
}

/**
  * @brief  USBD_COMPOSITE_EP0_RxReady
  *         handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t USBD_COMPOSITE_EP0_RxReady(USBD_HandleTypeDef *pdev) {

    pdev->pClassData = pClassDataCDC;
    pdev->pUserData = (void *) &USBD_CDC_fops_FS;
    return USBD_CDC.EP0_RxReady(pdev);
    // return USBD_OK;
}

/**
  * @brief  USBD_COMPOSITE_DataOut
  *         handle data OUT Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t USBD_COMPOSITE_DataOut(USBD_HandleTypeDef *pdev,
                                      uint8_t epnum) {
    switch (epnum) {
        case (CDC_OUT_EP & 0x0f):
        case (CDC_CMD_EP & 0x0f):
            pdev->pClassData = pClassDataCDC;
            pdev->pUserData = (void *) &USBD_Interface_fops_FS;
            return (USBD_CDC.DataOut(pdev, epnum));
        case (HID_MOUSE_EPIN_ADDR & 0x0f):
            pdev->pClassData = pClassDataHidMouse;
            pdev->pUserData = (void *) &USBD_HID_MOUSE_fops_FS;
            USBD_HID_Mouse.DataOut(pdev, epnum);
            break;
        case (HID_KEYBOARD_EPIN_ADDR & 0x0f):
            pdev->pClassData = pClassDataHidKeyboard;
            pdev->pUserData = (void *) &USBD_HID_KEYBOARD_fops_FS;
            USBD_HID_Keyboard.DataOut(pdev, epnum);
            break;
        default:
            break;
    }
    return USBD_FAIL;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t *USBD_COMPOSITE_GetDeviceQualifierDesc(uint16_t *length) {
    *length = sizeof(USBD_COMPOSITE_DeviceQualifierDesc);
    return USBD_COMPOSITE_DeviceQualifierDesc;
}

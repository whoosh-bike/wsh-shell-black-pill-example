#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

#include "usbd_cdc.h"

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
uint32_t CDC_GetRxBufferBytesAvailable_FS(void);
uint8_t CDC_ReadRxBuffer_FS(uint8_t* out, uint32_t len);

#endif /* __USBD_CDC_IF_H__ */

#include "main.h"
// #include "usb_device.h"
// #include "usbd_cdc_if.h"

// #include "shell.h"

int _read(int file, char* ptr, int len) {
    // char c;
    // while (CDC_GetRxBufferBytesAvailable_FS() == 0) {
    // }

    // if (CDC_ReadRxBuffer_FS((uint8_t*)&c, 1) == USB_CDC_RX_BUFFER_OK) {
    //     *ptr++ = c;
    //     return 1;
    // }

    return -1;
}

int _write(int file, char* ptr, int len) {
    (void)file;
    // for (int i = 0; i < len; i++) {
    //     char c = (char)(*ptr++);
    //     while (CDC_Transmit_FS((uint8_t*)&c, 1) == USBD_BUSY) {
    //     }
    // }

    return len;
}

void SystemClock_Config(void) {
}

int main(void) {
    HAL_Init();

    assert_param(0);

    while (1)
        ;
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
        __BKPT();
    }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
    Error_Handler();
}
#endif /* USE_FULL_ASSERT */

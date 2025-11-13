#include "main.h"
#include "shell.h"
#include "usb_device.h"

int _read(int file, char* ptr, int len) {
    char c;
    while (CDC_GetRxBufferBytesAvailable_FS() == 0) {
    }

    if (CDC_ReadRxBuffer_FS((uint8_t*)&c, 1) != 0) {
        *ptr++ = c;
        return 1;
    }

    return -1;
}

int _write(int file, char* ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        char c = (char)(*ptr++);
        while (CDC_Transmit_FS((uint8_t*)&c, 1) == USBD_BUSY) {
        }
    }

    return len;
}

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Power config: scale1 for >84 MHz */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* HSE on, PLL on */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;

    /* For HSE = 25 MHz:
       PLLM = 25 -> 25/25 = 1 MHz input to PLL
       PLLN = 192 -> VCO = 192 MHz
       PLLP = 2 -> SYSCLK = 192/2 = 96 MHz
       PLLQ = 4 -> USB = 192/4 = 48 MHz (exact)
    */
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /* Bus clocks */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1; /* HCLK = 96 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;   /* PCLK1 = 48 MHz (<=50) */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;   /* PCLK2 = 96 MHz */

    /* Flash latency: 3 wait states for 96 MHz on F4 family */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }

    SystemCoreClockUpdate();

    /* Enable used GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

int main(void) {
    HAL_Init();
    SystemClock_Config();
    UsbCdcDevice_Init();
    HAL_Delay(2000);

    if (!Shell_Init("black-pill"))
        Error_Handler();

    while (1) {
        int symbol = getchar();
        // printf("key: %c / %d / 0x%02X\r\n", symbol, (unsigned char)symbol);

        if (symbol == EOF)
            continue;

        Shell_SendChar((char)symbol);
    }
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

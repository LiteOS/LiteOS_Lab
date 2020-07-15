#include <stdint.h>

#include "./main.c"

int HardWare_Init (void)
    {
    SystemInit ();

    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_LPUART1_UART_Init();
    MX_USART3_UART_Init();

    return 0;
    }

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "rtc.h"
#include "gpio.h"
#include "stdio.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "bmp.h"
#include "dht11.h"
#include "sys_init.h"
#include "mem.h"

#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"
#include <stimer.h>
#include <shell.h>

#if defined (__CC_ARM)
extern char Image$$RW_IRAM1$$ZI$$Limit [];
extern char Image$$ARM_LIB_STACKHEAP$$Base [];
#elif defined (__GNUC__)
extern char __los_heap_addr_start__ [];
extern char __los_heap_addr_end__ [];
#else
#error "fix me"
#endif

const struct phys_mem system_phys_mem [] =
    {
#if defined (__CC_ARM)
        { Image$$RW_IRAM1$$ZI$$Limit, Image$$ARM_LIB_STACKHEAP$$Base },
#elif defined (__GNUC__)
        {(unsigned long) __los_heap_addr_start__,(unsigned long) __los_heap_addr_end__ },
#else
#error "fix me"
#endif
        { 0, 0 }
    };



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
VOID HardWare_Init(VOID);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
DHT11_Data_TypeDef dht11;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* shell 命令 */
static int shell_Temp(int argc,const char *argv[]){
    printf("temp = %.2f ; humi = %.2f\r\n",dht11.temperature,dht11.humidity);
    return 0;
}
OSSHELL_EXPORT_CMD(shell_Temp,"readTemp","readTemp");


/* 温度检测相关 */
static void DHT11timer_callback(void *arg)
{
  DHT11_Read_TempAndHumidity(&dht11);
}


/* 该任务用于初始化所有外设以及NBIOT
 *
 */
int task1(void *args)
{
  
  osal_init();  //如果不调用该函数，printf无法使用

/* 初始化软件定时器，用于DHT11采集温湿度信息
 *
 */
#if CONFIG_STIMER_ENABLE
    
    stimer_init();

#endif

/* 初始化shell，可以自定义各种指令，推荐使用MobaXterm_Personal_10.4软件，普通的串口助手会乱码
 *
 */
#if CONFIG_SHELL_ENABLE

    shell_init();
#endif

#define CONFIG_DRIVER_ENABLE 1
/* 初始化设备注册框架用于注册at指令使用的串口
 *
 */
#if CONFIG_DRIVER_ENABLE
    #include <driver.h>
    ///< install the driver framework for the link
    los_driv_init();
#endif

#define CONFIG_AT_ENABLE 1
/* 初始化AT框架用于NBiot模块
 *
 */
#if CONFIG_AT_ENABLE
    #include <at.h>
    #include <iot_link_config.h>
    extern bool_t uart_at_init(int baud);


    #ifndef CONFIG_AT_BAUDRATE
    #define CONFIG_AT_BAUDRATE  9600
    #endif

    #ifndef CONFIG_AT_DEVICENAME
    #define CONFIG_AT_DEVICENAME  "atdev"
    #endif


    ///< install the at framework for the link
    uart_at_init(CONFIG_AT_BAUDRATE);
    at_init(CONFIG_AT_DEVICENAME);
#endif
  

////////////////////////////  OC LWM2M ///////     /////////////////////////////
#if CONFIG_OC_LWM2M_ENABLE
    #include <oc_lwm2m_al.h>
    oc_lwm2m_init();

    #if CONFIG_OC_LWM2M_AGENT_ENABLE
        #include <agent_lwm2m.h>
        oc_lwm2m_install_agent();
    #endif

    #if CONFIG_OC_LWM2M_BOUDICA150_ENABLE
        #include <boudica150_oc.h>
        #define cn_app_bands    "5"  //bc95电信卡5
        boudica150_init(NULL,NULL,cn_app_bands);
    #endif

    #if CONFIG_OC_LWM2M_BOUDICA120_ENABLE
        #include <boudica120_oc.h>
        #define cn_app_bands    "5"  //bc95电信卡5
        oc_lwm2m_imp_init();
    #endif
#endif

//////////////////////////  COAP PROTOCOL  /////////////////////////////////
#ifdef CONFIG_OCCOAP_ENABLE
	#include <oc_coap_al.h>
    oc_coap_init();
#endif

#if CONFIG_OC_COAP_BOUDICA120_ENABLE
	#include <boudica120_oc.h>
	oc_coap_imp_init();
#endif
 
  /* 创建一个定时器，用于DHT11采集温湿度
   *
   */
  stimer_create("DHT11timer",DHT11timer_callback,NULL,1*1000,cn_stimer_flag_start); 

  printf("task1 init succsee!\r\n");
  printf("start init task2\r\n");
  int Creat_task2();
  Creat_task2();
  while (1)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    LOS_TaskDelay(2000);
    
  }
}

static int Creat_task1()
{
    int ret = -1;
    UINT32 uwRet = LOS_OK;
    UINT32  handle;
    TSK_INIT_PARAM_S task1_init_param;

    memset (&task1_init_param, 0, sizeof (TSK_INIT_PARAM_S));
    task1_init_param.uwArg = (unsigned int)NULL;
    task1_init_param.usTaskPrio = 2;
    task1_init_param.pcName =(char *) "task1";
    task1_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)task1;
    task1_init_param.uwStackSize = 0x1000;
    uwRet = LOS_TaskCreate(&handle, &task1_init_param);
    if(LOS_OK == uwRet){
        ret = 0;
    }
    return ret;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
   UINT32 uwRet = LOS_OK;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init(); //这里配置了中断分组以及系统时钟

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  //MX_LPUART1_UART_Init();
  MX_RTC_Init();
  //MX_USART1_UART_Init();
  //MX_USART3_UART_Init();
  HardWare_Init();
  /* Initialize interrupts */
  //MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  //任务创建
    HardWare_Init();
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    extern void shell_uart_init(int baud);
    shell_uart_init(115200);
    //printf("Hello World! This is LiteOS!\r\n");
    
    Creat_task1();

    (void)LOS_Start();
    return 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability 
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_LPUART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* LPUART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(LPUART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(LPUART1_IRQn);
}

/* USER CODE BEGIN 4 */
VOID HardWare_Init(VOID)
{
  //初始化延时函数
	dwt_delay_init(SystemCoreClock);
  //初始化OLED
  OLED_Init();
	OLED_Clear();
	OLED_ShowCHinese(9, 0, 0);//朱
	OLED_ShowCHinese(27, 0, 1);//有
	OLED_ShowCHinese(45, 0, 2);//鹏
	OLED_ShowCHinese(63, 0, 3);//物
	OLED_ShowCHinese(81, 0, 4);//联
	OLED_ShowCHinese(99, 0, 5);//网
	OLED_ShowCHinese(36, 2, 6);//大
	OLED_ShowCHinese(54, 2, 7);//讲
	OLED_ShowCHinese(72, 2, 8);//堂
	OLED_ShowString(18, 4, "L476_LiTeOS", 16);
	OLED_ShowString(48, 6, "NBIOT", 16);
	//delayus(1000);
  //初始化DHT11
	DHT11_Init();
}
/* USER CODE END 4 */



#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

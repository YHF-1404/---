/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "freertosTask.h"
#include "uartApp.h"
#include "app_touchgfx.h"
#include "lcd_rgb.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t uartTaskHandle;
const osThreadAttr_t uartTask_attributes = {
  .name = "uartTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t uart3TaskHandle;
const osThreadAttr_t uart3Task_attributes = {
  .name = "uart3Task",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t dataTaskHandle;
const osThreadAttr_t dataTask_attributes = {
  .name = "dataTask",
  .stack_size = 128 * 10,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t dataCollectionTaskHandle;
const osThreadAttr_t dataCollectionTask_attributes = {
  .name = "dataCollectionTask",
  .stack_size = 128 * 8,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for lockTask */
osThreadId_t lockTaskHandle;
const osThreadAttr_t lockTask_attributes = {
  .name = "lockTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t temperTaskHandle;
const osThreadAttr_t temperTask_attributes = {
  .name = "temperTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GXFTask */
osThreadId_t GXFTaskHandle;
const osThreadAttr_t GXFTask_attributes = {
  .name = "GXFTask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for sys_ledTask */
osThreadId_t sys_ledTaskHandle;
const osThreadAttr_t sys_ledTask_attributes = {
  .name = "sys_ledTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};


/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/* Definitions for usart1_rx */
osMessageQueueId_t usart1_rxHandle;
const osMessageQueueAttr_t usart1_rx_attributes = {
  .name = "usart1_rx"
};
/* Definitions for usart3_rx */
osMessageQueueId_t usart3_rxHandle;
const osMessageQueueAttr_t usart3_rx_attributes = {
  .name = "usart3_rx"
};
/* 定义数据传输队列 */
osMessageQueueId_t dataHandle;
const osMessageQueueAttr_t data_attributes = {
  .name = "data"
};

void StartUartTask(void *argument);
void StartUart3Task(void *argument);

void StartDataTask(void *argument);
void StartDataCollectionTask(void *argument);
void StartLockTask(void *argument);
void StartTemperTask(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartGXFTask(void *argument);
void StartSysLedTask(void *argument);


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationIdleHook(void);

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  usart1_rxHandle = osMessageQueueNew (10, sizeof(uint32_t), &usart1_rx_attributes);
  usart3_rxHandle = osMessageQueueNew (10, sizeof(uint32_t), &usart3_rx_attributes);
  dataHandle = osMessageQueueNew (20, sizeof(DATA_TypeDef), &data_attributes);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of GXFTask */
  GXFTaskHandle = osThreadNew(StartGXFTask, NULL, &GXFTask_attributes);

  /* creation of sys_ledTask */
  sys_ledTaskHandle = osThreadNew(StartSysLedTask, NULL, &sys_ledTask_attributes);



  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  uartTaskHandle = osThreadNew(StartUartTask, NULL, &uartTask_attributes);
  uart3TaskHandle = osThreadNew(StartUart3Task, NULL, &uartTask_attributes);
  dataTaskHandle = osThreadNew(StartDataTask, NULL, &dataTask_attributes);
  dataCollectionTaskHandle =  osThreadNew(StartDataCollectionTask, NULL, &dataCollectionTask_attributes);
  /* creation of lockTask */
  lockTaskHandle = osThreadNew(StartLockTask, NULL, &lockTask_attributes);
  temperTaskHandle = osThreadNew(StartTemperTask, NULL, &temperTask_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartGXFTask */
/**
* @brief Function implementing the GXFTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGXFTask */
void StartGXFTask(void *argument)
{
  /* USER CODE BEGIN StartGXFTask */
	LCD_Init();
  /* Infinite loop */
  for(;;)
  {
	 MX_TouchGFX_Process();
    //osDelay(1);
  }
  /* USER CODE END StartGXFTask */
}

/* USER CODE BEGIN Header_StartSysLedTask */
/**
* @brief Function implementing the sys_ledTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartSysLedTask */
void StartSysLedTask(void *argument)
{
  /* USER CODE BEGIN StartSysLedTask */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(sys_led_GPIO_Port,sys_led_Pin);
    osDelay(100);
  }
  /* USER CODE END StartSysLedTask */
}

/* USER CODE BEGIN Header_StartLockTask */


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

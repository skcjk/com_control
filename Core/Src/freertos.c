/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "iwdg.h"
#include "SDTask.h"
#include "usart.h"
#include "rtc.h"
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
osPoolId  sdCmdQueuePoolHandle;
osPoolId  rx1QueuePoolHandle;
osPoolId  rx2QueuePoolHandle;
extern RTC_DateTypeDef RTC_DateStruct; 
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId cmdTaskHandle;
osThreadId sdTaskHandle;
osThreadId saveTaskHandle;
osMessageQId sdCmdQueueHandle;
osMessageQId rx1QueueHandle;
osMessageQId rx2QueueHandle;
osMutexId printMutexHandle;
osMutexId rtcMutexHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
extern void CMDTask(void const * argument);
extern void SDTask(void const * argument);
extern void SaveTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* definition and creation of printMutex */
  osMutexDef(printMutex);
  printMutexHandle = osMutexCreate(osMutex(printMutex));

  /* definition and creation of rtcMutex */
  osMutexDef(rtcMutex);
  rtcMutexHandle = osMutexCreate(osMutex(rtcMutex));

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of sdCmdQueue */
  osMessageQDef(sdCmdQueue, 5, uint32_t);
  sdCmdQueueHandle = osMessageCreate(osMessageQ(sdCmdQueue), NULL);

  /* definition and creation of rx1Queue */
  osMessageQDef(rx1Queue, 5, uint32_t);
  rx1QueueHandle = osMessageCreate(osMessageQ(rx1Queue), NULL);

  /* definition and creation of rx2Queue */
  osMessageQDef(rx2Queue, 5, uint32_t);
  rx2QueueHandle = osMessageCreate(osMessageQ(rx2Queue), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  osPoolDef(sdCmdQueuePool, 5, sdStruct); 
  sdCmdQueuePoolHandle = osPoolCreate(osPool(sdCmdQueuePool));

  osPoolDef(rx1QueuePool, 5, rxStruct); 
  rx1QueuePoolHandle = osPoolCreate(osPool(rx1QueuePool));

  osPoolDef(rx2QueuePool, 5, rxStruct); 
  rx2QueuePoolHandle = osPoolCreate(osPool(rx2QueuePool));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of cmdTask */
  osThreadDef(cmdTask, CMDTask, osPriorityHigh, 0, 256);
  cmdTaskHandle = osThreadCreate(osThread(cmdTask), NULL);

  /* definition and creation of sdTask */
  osThreadDef(sdTask, SDTask, osPriorityNormal, 0, 512);
  sdTaskHandle = osThreadCreate(osThread(sdTask), NULL);

  /* definition and creation of saveTask */
  osThreadDef(saveTask, SaveTask, osPriorityHigh, 0, 256);
  saveTaskHandle = osThreadCreate(osThread(saveTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(500);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_2);
    HAL_IWDG_Refresh(&hiwdg);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */


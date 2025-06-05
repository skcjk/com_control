/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <string.h>
#include "stdio.h"
#include "cmsis_os.h"

extern osMessageQId rx1QueueHandle;
extern osMessageQId rx2QueueHandle;
extern osPoolId rx1QueuePoolHandle;
extern osPoolId rx2QueuePoolHandle;
uint8_t aRx1Buffer;		
uint8_t aRx2Buffer;		
rxStruct rx1S = {
        .rx_buf = {0}, 
        .data_length = 0 
    };
rxStruct rx2S = {
        .rx_buf = {0}, 
        .data_length = 0 
    };
rxStruct *rx1SForQueue;
rxStruct *rx2SForQueue;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *f)
 
{
 
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
 
  return ch;
 
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */
  if (huart->Instance == USART1) 
  {
    if (rx1S.data_length >= RX_BUF_LEN) //
    {
      memset(rx1S.rx_buf, 0x00, RX_BUF_LEN);
      rx1S.data_length = 0;
    }
    else
    {
      rx1S.rx_buf[rx1S.data_length++] = aRx1Buffer; //

      if (((rx1S.rx_buf[rx1S.data_length - 1] == 0x0A) && (rx1S.rx_buf[rx1S.data_length - 2] == 0x0D)) || rx1S.data_length == RX_BUF_LEN) //
      {
        rx1SForQueue = osPoolAlloc(rx1QueuePoolHandle); // 从池中分配内存
        memcpy(rx1SForQueue->rx_buf, rx1S.rx_buf, RX_BUF_LEN); // 将数据复制到分配的内存中
        rx1SForQueue->data_length = rx1S.data_length; // 设置数据长度
        if (osOK != osMessagePut(rx1QueueHandle, (uint32_t)rx1SForQueue, 0)) osPoolFree(rx1QueuePoolHandle, rx1SForQueue);
        memset(rx1S.rx_buf, 0x00, rx1S.data_length);
        rx1S.data_length = 0;
      }
    }
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRx1Buffer, 1); //
  }
  if (huart->Instance == USART2) 
  {
    if (rx2S.data_length >= RX_BUF_LEN) //
    {
      memset(rx2S.rx_buf, 0x00, RX_BUF_LEN);
      rx2S.data_length = 0;
    }
    else
    {
      rx2S.rx_buf[rx2S.data_length++] = aRx2Buffer; //
      HAL_UART_Transmit(&huart1, (uint8_t *)&aRx2Buffer, 1, 0x00);

      if (((rx2S.rx_buf[rx2S.data_length - 1] == 0x0A) && (rx2S.rx_buf[rx2S.data_length - 2] == 0x0D)) || rx2S.data_length == RX_BUF_LEN) //
      {
        rx2SForQueue = osPoolAlloc(rx2QueuePoolHandle); // 从池中分配内存
        memcpy(rx2SForQueue->rx_buf, rx2S.rx_buf, RX_BUF_LEN); // 将数据复制到分配的内存中
        rx2SForQueue->data_length = rx2S.data_length; // 设置数据长度
        if (osOK != osMessagePut(rx2QueueHandle, (uint32_t)rx2SForQueue, 0)) osPoolFree(rx2QueuePoolHandle, rx2SForQueue);
        memset(rx2S.rx_buf, 0x00, rx2S.data_length);
        rx2S.data_length = 0;
      }
    }
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRx2Buffer, 1); //
  }
}
/* USER CODE END 1 */

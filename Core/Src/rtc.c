/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
#include "rtc.h"

/* USER CODE BEGIN 0 */
RTC_TimeTypeDef RTC_TimeStruct;  
RTC_DateTypeDef RTC_DateStruct; 

HAL_StatusTypeDef UISet_Time(uint8_t hour,uint8_t min,uint8_t sec)
{
	RTC_TimeStruct.Hours=hour;
	RTC_TimeStruct.Minutes=min;
	RTC_TimeStruct.Seconds=sec;
	return HAL_RTC_SetTime(&hrtc,&RTC_TimeStruct,RTC_FORMAT_BIN);	
}

HAL_StatusTypeDef UISet_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	RTC_DateStruct.Date=date;
	RTC_DateStruct.Month=month;
	RTC_DateStruct.WeekDay=week;
	RTC_DateStruct.Year=year;
	Date_write_BKP(&hrtc,&RTC_DateStruct);
	return HAL_RTC_SetDate(&hrtc,&RTC_DateStruct,RTC_FORMAT_BIN);
}

int16_t CharToDec(char *str, uint8_t cnt)
{
  int16_t data = 0;
  uint8_t i;

  if (str[0] == '-')
  {
    for (i = 1; i < cnt; i++)
    {
      data *= 10;
      if (str[i] < '0' || str[i] > '9')
        return 0;
      data += str[i] - '0';
    }
    data = -data;
  }
  else
  {
    for (i = 0; i < cnt; i++)
    {
      data *= 10;
      if (str[i] < '0' || str[i] > '9')
        return 0;
      data += str[i] - '0';
    }
  }
  return data;
}
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)!= 0x5051)
	  {
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_MONDAY;
  DateToUpdate.Month = RTC_MONTH_JANUARY;
  DateToUpdate.Date = 0x1;
  DateToUpdate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
  RTC_DateStruct = DateToUpdate;                              
  HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051); 
  Date_write_BKP(&hrtc,&DateToUpdate);
  }
  else
  {
    Date_read_BKP(&hrtc);
  }
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR5,RTC_DateStruct.WeekDay);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR2,RTC_DateStruct.Year);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR3,RTC_DateStruct.Month);
  HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR4,RTC_DateStruct.Date);
  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

void Date_write_BKP(RTC_HandleTypeDef *  hrtc,RTC_DateTypeDef * Date)
{//将日期保存在备份域
HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR5,Date->WeekDay);
HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR2,Date->Year);
HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR3,Date->Month);
HAL_RTCEx_BKUPWrite(hrtc,RTC_BKP_DR4,Date->Date);
}
void Date_read_BKP(RTC_HandleTypeDef *  hrtc)
{//将日期从备份域还原到hrtc->DateToUpdate用于HAL_RTC_GetDate更新日期
RTC_DateStruct.WeekDay=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR5);
RTC_DateStruct.Year=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR2);
RTC_DateStruct.Month=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR3);
RTC_DateStruct.Date=HAL_RTCEx_BKUPRead(hrtc,RTC_BKP_DR4);
HAL_RTC_SetDate(hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
}

/* USER CODE END 1 */

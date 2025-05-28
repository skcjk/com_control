#include "saveTask.h"
#include "rtc.h"
#include "SDTask.h"

extern osMessageQId rx2QueueHandle;
extern osMessageQId sdCmdQueueHandle;
extern osPoolId rx2QueuePoolHandle;
extern osPoolId  sdCmdQueuePoolHandle;
extern uint8_t aRx2Buffer;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 
extern osMutexId rtcMutexHandle;

static sdStruct sdS = {
    .rx_buf = {0}, 
    .read_path = {0},
    .delete_path = {0},
    .sd_cmd = SD_WRITE
};
static sdStruct *sdSForQueue;

rxStruct *receiveRx2FromQueneForCmd;

void SaveTask(void const * argument){
    osEvent evt;

    HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRx2Buffer, 1);
    while (1)
    {
        evt = osMessageGet(rx2QueueHandle, osWaitForever);
        if (evt.status == osEventMessage){
            receiveRx2FromQueneForCmd = evt.value.p;
            osMutexWait(rtcMutexHandle, osWaitForever);
            HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
            osMutexRelease(rtcMutexHandle);
            // 去除接收到的数据末尾的\r\n
            char *rx_buf = receiveRx2FromQueneForCmd->rx_buf;
            size_t len = receiveRx2FromQueneForCmd->data_length;
            if (len >= 2 && rx_buf[len - 2] == '\r' && rx_buf[len - 1] == '\n') {
                rx_buf[len - 2] = '\0';
                rx_buf[len - 1] = '\0';
                receiveRx2FromQueneForCmd->data_length -= 2;
            }
            snprintf(sdS.rx_buf, SD_BUF_LEN,
                "{\"data\":\"%s\",\"year\":%02d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n\r\n",
                rx_buf,
                2000 + RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
                RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
            osPoolFree(rx2QueuePoolHandle, receiveRx2FromQueneForCmd);

            sdSForQueue = osPoolAlloc(sdCmdQueuePoolHandle);
            sdSForQueue->sd_cmd = sdS.sd_cmd;
            memcpy(sdSForQueue->rx_buf, sdS.rx_buf, SD_BUF_LEN);
            memcpy(sdSForQueue->read_path, sdS.read_path, READ_PATH_LEN);
            memcpy(sdSForQueue->delete_path, sdS.delete_path, DELETE_PATH_LEN);
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sdSForQueue, 50)) {
                osPoolFree(sdCmdQueuePoolHandle, sdSForQueue); // 如果消息队列满了，释放内存
            }
        }
    }
    
}

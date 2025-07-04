#include "saveTask.h"
#include "rtc.h"
#include "SDTask.h"

extern osMessageQId rx1QueueHandle;
extern osMessageQId sdCmdQueueHandle;
extern osPoolId rx1QueuePoolHandle;
extern osPoolId  sdCmdQueuePoolHandle;
extern uint8_t aRx1Buffer;
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

rxStruct *receiveRx1FromQueneForSD;

void SaveTask(void const * argument){
    osEvent evt;

    HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)&aRx1Buffer, 1);
    while (1)
    {
        evt = osMessageGet(rx1QueueHandle, osWaitForever);
        if (evt.status == osEventMessage){
            receiveRx1FromQueneForSD = evt.value.p;
            osMutexWait(rtcMutexHandle, osWaitForever);
            HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
            Date_write_BKP(&hrtc,&RTC_DateStruct);  // 更新备份寄存器中的日期信息,调用HAL_RTC_GetTime后会清空天数计数器，所以必须将日期保存至备份区
            osMutexRelease(rtcMutexHandle);
            // 去除接收到的数据末尾的\r\n
            unsigned char *rx_buf = receiveRx1FromQueneForSD->rx_buf;
            size_t len = receiveRx1FromQueneForSD->data_length;
            if (len >= 2 && rx_buf[len - 2] == '\r' && rx_buf[len - 1] == '\n') {
                rx_buf[len - 2] = '\0';
                rx_buf[len - 1] = '\0';
                receiveRx1FromQueneForSD->data_length -= 2;
            }
            snprintf(sdS.rx_buf, SD_BUF_LEN,
                "{\"data\":\"%s\",\"year\":%02d,\"month\":%02d,\"day\":%02d,\"hour\":%02d,\"minute\":%02d,\"second\":%02d}\r\n",
                rx_buf,
                2000 + RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
                RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
            osPoolFree(rx1QueuePoolHandle, receiveRx1FromQueneForSD);

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

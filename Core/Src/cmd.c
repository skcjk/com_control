#include "cmd.h"
#include "rtc.h"
#include "SDTask.h"
#include <math.h>

extern osMessageQId rx1QueueHandle;
extern osMessageQId sdCmdQueueHandle;
extern osPoolId  sdCmdQueuePoolHandle;
extern osPoolId rx1QueuePoolHandle;
extern osMutexId printMutexHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 
extern char read_path[16];
extern char delete_path[16];
extern char WriteBuffer[100];
extern uint8_t aRx1Buffer;	
extern uint8_t aRx2Buffer;	

rxStruct *receiveRxFromQueneForCmd;

void CMDTask(void *argument)
{

    callback_t callbacks[] = { // 回调函数反射表
        {"sum", sum},
        {"reboot", reboot},
        {"time", timeRTC},
        {"sd", sdCMD},
    };
    uint8_t res = NO_SUCH_CMD;

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&aRx1Buffer, 1);
    HAL_UART_Receive_IT(&huart2, (uint8_t *)&aRx2Buffer, 1);
    osEvent evt;

    while (1)
    {
        evt = osMessageGet(rx1QueueHandle, osWaitForever);
        if (evt.status == osEventMessage)
        { // 从串口接收缓冲区接收数据
            receiveRxFromQueneForCmd = evt.value.p;
            cJSON *root = cJSON_ParseWithLength((char *)receiveRxFromQueneForCmd->rx_buf, (size_t)receiveRxFromQueneForCmd->data_length); // 解析接收信息
            osPoolFree(rx1QueuePoolHandle, receiveRxFromQueneForCmd);

            if (root == NULL)
            { // 解析失败
                res = JSON_PARSE_ERROR;
            }
            else
            { // 解析成功
                cJSON *cmdItem = cJSON_GetObjectItem(root, "cmd");
                cJSON *argvItem = cJSON_GetObjectItem(root, "argv");

                if ((cmdItem != NULL) && (argvItem != NULL))
                {
                    for (uint8_t i = 0; i < sizeof(callbacks) / sizeof(callbacks[0]); i++)
                    {
                        if (!strcmp(callbacks[i].name, cJSON_GetStringValue(cmdItem)))
                        {
                            res = callbacks[i].fn(argvItem);
                        }
                    }
                }
                else res = CMD_ERROR;
            }

            osMutexWait(printMutexHandle, osWaitForever);
            if (res != JSON_CMD_OK) printf("cmd error: %d\r\n", res);
            else printf("cmd_ok\r\n");
            osMutexRelease(printMutexHandle);
            cJSON_Delete(root); // 释放内存
        }
    }
}

uint8_t sum(cJSON *root)
{
    double sum;

    cJSON *aItem = cJSON_GetObjectItem(root, "a");
    cJSON *bItem = cJSON_GetObjectItem(root, "b");
    if ((aItem != NULL && cJSON_IsNumber(aItem)) && (bItem != NULL && cJSON_IsNumber(bItem)))
    {
        sum = cJSON_GetNumberValue(aItem) + cJSON_GetNumberValue(bItem);

        cJSON *return_root = cJSON_CreateObject();
        cJSON_AddStringToObject(return_root, "return", "sum");
        cJSON_AddNumberToObject(return_root, "result", sum);
        char *return_str = cJSON_Print(return_root);

        osMutexWait(printMutexHandle, osWaitForever);
        printf(return_str);
        printf("\r\n");
        osMutexRelease(printMutexHandle);

        free(return_str); // 释放内存
        cJSON_Delete(return_root);

        return JSON_CMD_OK;
    }
    return ARGV_ERROR;
}

uint8_t reboot(cJSON *root){
    osMutexWait(printMutexHandle, osWaitForever);
    printf("reboot\r\n");
    osMutexRelease(printMutexHandle);
    osDelay(50);
    __set_FAULTMASK(1);
    HAL_NVIC_SystemReset();
    return JSON_CMD_OK;
}

uint8_t timeRTC(cJSON *root){
    cJSON *timeItem = cJSON_GetObjectItem(root, "setTime");
    if ((timeItem != NULL && cJSON_IsString(timeItem)))
    {
        char *pt = cJSON_GetStringValue(timeItem);
        BKTime temptimept;
        temptimept.Year = CharToDec(pt, 2);
        pt += 2;
        temptimept.Month = CharToDec(pt, 2);
        pt += 2;
        temptimept.Date = CharToDec(pt, 2);
        pt += 2;
        temptimept.WeekDay = CharToDec(pt, 1);
        pt += 1;
        temptimept.Hours = CharToDec(pt, 2);
        pt += 2;
        temptimept.Minutes = CharToDec(pt, 2);
        pt += 2;
        temptimept.Seconds = CharToDec(pt, 2);
        UISet_Time(temptimept.Hours, temptimept.Minutes, temptimept.Seconds);
        UISet_Date(temptimept.Year, temptimept.Month, temptimept.Date, temptimept.WeekDay);
    }
    HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    osMutexWait(printMutexHandle, osWaitForever);
    printf("%02d/%02d/%02d\r\n",2000 + RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    printf("%02d:%02d:%02d\r\n",RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    printf("\r\n");
    osMutexRelease(printMutexHandle);
    return JSON_CMD_OK;
}

uint8_t sdCMD(cJSON *root){
    cJSON *optItem = cJSON_GetObjectItem(root, "opt");
    uint8_t res = ARGV_ERROR;
    uint16_t *sd_cmd;
    sd_cmd = osPoolAlloc(sdCmdQueuePoolHandle);
    if (optItem != NULL && cJSON_IsString(optItem)){
        char *opt_str = cJSON_GetStringValue(optItem);
        if (!strcmp(opt_str, "ls")){
            *sd_cmd = SD_LS;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "saveTest")){
            strcpy(WriteBuffer, "test\r\n");
            *sd_cmd = SD_WRITE;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "delete_all")){
            *sd_cmd = SD_DELETE_ALL;
            res = JSON_CMD_OK;
        }
        if (!strcmp(opt_str, "read")){
            cJSON *pathItem = cJSON_GetObjectItem(root, "path");
            if (pathItem != NULL && cJSON_IsString(pathItem)){
                *sd_cmd = SD_READ;
                strcpy(read_path, cJSON_GetStringValue(pathItem));
                res = JSON_CMD_OK;
            }
            else res = ARGV_ERROR;
        }
        if (!strcmp(opt_str, "delete")){
            cJSON *pathItem = cJSON_GetObjectItem(root, "path");
            if (pathItem != NULL && cJSON_IsString(pathItem)){
                *sd_cmd = SD_DELETE;
                strcpy(delete_path, cJSON_GetStringValue(pathItem));
                res = JSON_CMD_OK;
            }
            else res = ARGV_ERROR;
        }
        
        free(opt_str);
        if (res == JSON_CMD_OK){
            if (osOK != osMessagePut(sdCmdQueueHandle, (uint32_t)sd_cmd, 0)) {
                osPoolFree(sdCmdQueuePoolHandle, sd_cmd); // 如果消息队列满了，释放内存
                return OS_ERROR;
            }
            return JSON_CMD_OK;
        } 
    }
    else res = ARGV_ERROR;
    return res;
}

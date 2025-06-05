#include "SDTask.h"
#include "rtc.h"
#include <string.h>
#include "stdio.h"

FATFS fs;                         /* FatFs文件系统对象 */                         /* 文件对象 */
// FRESULT res_sd;                /* 文件操作结果 */
// UINT fnum;                        /* 文件成功读写数量 */
// BYTE ReadBuffer[1024]= {0};       /* 读缓冲区 */
// BYTE bpData[512] =  {0};   

extern osMessageQId sdCmdQueueHandle;
extern osPoolId  sdCmdQueuePoolHandle;
extern osMutexId printMutexHandle;
extern RTC_TimeTypeDef RTC_TimeStruct;  
extern RTC_DateTypeDef RTC_DateStruct; 

void SDTask(void const * argument)
{
    osEvent evt;
    FRESULT sd_res;
    sdStruct *sdS;
    sd_res = f_mount(&fs, "0:", 1);
    while(1)
    {
        evt = osMessageGet(sdCmdQueueHandle, osWaitForever);
        if (evt.status == osEventMessage)
        {
            sdS = evt.value.p;
            switch (sdS->sd_cmd)
            {
            case SD_LS:
                sd_res = list_dir("0:");
                break;
            case SD_WRITE:
                sd_res = save_data(sdS);
                break;
            case SD_READ:
                sd_res = read_data(sdS);
                break;
            case SD_DELETE_ALL:
                sd_res = delete_all_files("0:");
                break;
            case SD_DELETE:
                sd_res = f_unlink(sdS->delete_path);
                break;
            }
            osPoolFree(sdCmdQueuePoolHandle, sdS);
            osMutexWait(printMutexHandle, osWaitForever);
            if (sd_res != FR_OK) printf("fatfs error: %d\r\n", sd_res);
            osMutexRelease(printMutexHandle);
        }
    }
}

/* List contents of a directory */

FRESULT list_dir(const char *path)
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* Directory */
                osMutexWait(printMutexHandle, osWaitForever);
                printf("   <DIR>   %s\r\n", fno.fname);
                osMutexRelease(printMutexHandle);
                ndir++;
            }
            else
            { /* File */
                osMutexWait(printMutexHandle, osWaitForever);
                printf("file_size:%10u file_name:%s\r\n", fno.fsize, fno.fname);
                osMutexRelease(printMutexHandle);
                nfile++;
            }
        }
        f_closedir(&dir);
        osMutexWait(printMutexHandle, osWaitForever);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
        osMutexRelease(printMutexHandle);
    }
    return res;
}

FRESULT save_data(sdStruct *sdS)
{
    FIL fp;
    char path[16];
    FRESULT res;
    UINT fnum;                        /* 文件成功读写数量 */

    HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf(path, "%02d_%02d_%02d.txt\r\n", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);

    res = f_open(&fp, path, FA_WRITE | FA_OPEN_ALWAYS);
    if (res) return res;

    // Move to end of file for append
    res = f_lseek(&fp, f_size(&fp));
    if (res) {
        f_close(&fp);
        return res;
    }

    res = f_write(&fp, sdS->rx_buf, strlen(sdS->rx_buf), &fnum);
    f_close(&fp);

    return res;
}

FRESULT read_data(sdStruct *sdS)
{
    FIL fp;
    FRESULT res;
    char line[512]={0}; /* Line buffer */

    /* Open a text file */
    res = f_open(&fp, sdS->read_path, FA_READ);
    if (res) return res;

    /* Read every line and display it */
    osMutexWait(printMutexHandle, osWaitForever);
    while (f_gets(line, sizeof(line), &fp) != NULL){
        printf("%s", line);
    }
    osMutexRelease(printMutexHandle);

    /* Close the file */
    f_close(&fp);

    return res;
}

FRESULT delete_all_files(const char *path) 
{
    FRESULT res;
    DIR dir;
    FILINFO fno;
    int nfile, ndir;

    res = f_opendir(&dir, path); /* Open the directory */
    if (res == FR_OK)
    {
        nfile = ndir = 0;
        for (;;)
        {
            res = f_readdir(&dir, &fno); /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0)
                break; /* Error or end of dir */
            if (fno.fattrib & AM_DIR)
            { /* Directory */
            }
            else
            { /* File */
                char path[16];
                snprintf(path, sizeof(path), "0:%s", fno.fname);
                res = f_unlink(path);
                if (res != FR_OK) break;
                osMutexWait(printMutexHandle, osWaitForever);
                printf("delete: file_size:%10u file_name:%s\r\n", fno.fsize, fno.fname);
                osMutexRelease(printMutexHandle);
                nfile++;
            }
        }
        f_closedir(&dir);
        osMutexWait(printMutexHandle, osWaitForever);
        printf("%d dirs, %d files.\r\n", ndir, nfile);
        osMutexRelease(printMutexHandle);
    }
    return res;
}

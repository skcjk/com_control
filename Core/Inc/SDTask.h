#include <stdint.h>
#include "fatfs.h"
#include <stdio.h>

#define SD_LS 0     //sd指令
#define SD_WRITE 1
#define SD_READ 2
#define SD_DELETE_ALL 3
#define SD_DELETE 4

#define SD_BUF_LEN  512
#define READ_PATH_LEN 64
#define DELETE_PATH_LEN 64

typedef struct
{
    char rx_buf[SD_BUF_LEN];
    char read_path[READ_PATH_LEN];
    char delete_path[DELETE_PATH_LEN];
    uint16_t sd_cmd;
} sdStruct;

void SDTask(void const * argument);
FRESULT list_dir (const char *path);
FRESULT save_data(sdStruct *sdS);
FRESULT read_data(sdStruct *sdS);
FRESULT delete_all_files(const char *path);

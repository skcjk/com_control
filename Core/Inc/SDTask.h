#include <stdint.h>
#include "fatfs.h"
#include <stdio.h>

#define SD_LS 0     //sd指令
#define SD_WRITE 1
#define SD_READ 2
#define SD_DELETE_ALL 3
#define SD_DELETE 4

#define SD_BUF_LEN  256 

typedef struct
{
    unsigned char rx_buf[SD_BUF_LEN];
    uint16_t data_length;
} sdStruct;

void SDTask(void const * argument);
FRESULT list_dir (const char *path);
FRESULT save_data(void);
FRESULT read_data(void);
FRESULT delete_all_files(const char *path);

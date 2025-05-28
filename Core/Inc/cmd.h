#include <stdarg.h>
#include <stdio.h>
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include "cmsis_os.h"

#define JSON_CMD_OK 0
#define JSON_PARSE_ERROR 1
#define CMD_ERROR 2
#define ARGV_ERROR 3
#define NO_SUCH_CMD 4
#define OS_ERROR 4

typedef uint8_t (*callback)(cJSON *root);

typedef struct
{
    const char *name;
    callback fn;
} callback_t;

void CMDTask(void *argument);

uint8_t sum(cJSON *root);
uint8_t reboot(cJSON *root);
uint8_t timeRTC(cJSON *root);
uint8_t sdCMD(cJSON *root);
uint8_t trans(cJSON *root);

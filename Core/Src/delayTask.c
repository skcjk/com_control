#include "delayTask.h"
#include "cmsis_os.h"
#include "gpio.h"

extern uint32_t delayMin; // 延时的分钟数

void DelayTask(void const * argument){
    while(1){
        HAL_GPIO_WritePin(OUT_CTR_GPIO_Port, OUT_CTR_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(EN232_GPIO_Port, EN232_Pin, GPIO_PIN_RESET);
        osDelay(delayMin * 60 * 1000); // 延时指定的分钟数
        HAL_GPIO_WritePin(OUT_CTR_GPIO_Port, OUT_CTR_Pin, GPIO_PIN_SET); 
        HAL_GPIO_WritePin(EN232_GPIO_Port, EN232_Pin, GPIO_PIN_SET);
        osDelay(1000*60); // 延时指定的分钟数
        
        // 在这里可以添加需要循环执行的任务
    }
}

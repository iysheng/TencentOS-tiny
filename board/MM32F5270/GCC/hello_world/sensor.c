/******************************************************************************
* File:             sensor.c
*
* Author:           iysheng@163.com  
* Created:          08/27/22 
*                   环境传感器感知线程
*****************************************************************************/

#include <tos_k.h>
#include <tos_shell.h>
#include "sensor_base.h"

#define SENSOR_TASK_STK_SIZE       0x1000
k_task_t sensor_task;
uint8_t sensor_task_stk[SENSOR_TASK_STK_SIZE];

void sensor_entry(void *arg)
{
	tos_shell_printf("Hello Sensor app\n");
    while (1) {
        tos_task_delay(1000);
    }
}



int sensor_app_init(void *arg)
{
    return tos_task_create(&sensor_task, "sensor_task", sensor_entry, arg, 2, sensor_task_stk, SENSOR_TASK_STK_SIZE, 0);
}


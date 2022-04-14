#include "mcu_init.h"

#define APPLICATION_TASK_STK_SIZE       0x100
k_task_t application_task;
uint8_t application_task_stk[APPLICATION_TASK_STK_SIZE];

__attribute__ ((__weak__)) void application_entry(void *arg)
{
    while (1) {
        gpio_bit_toggle(LED_GPIO_Port, LED_Pin);
        printf("This is a demo task,please use your task entry!\r\n");
        tos_task_delay(1000);
    }
}

int main(void)
{
    board_init();
    printf("Welcome to TencentOS tiny(%s)\r\n", TOS_VERSION);
    tos_knl_init();
    tos_task_create(&application_task, "application_task", application_entry, NULL, 4, application_task_stk, APPLICATION_TASK_STK_SIZE, 0);
    tos_knl_start();
}


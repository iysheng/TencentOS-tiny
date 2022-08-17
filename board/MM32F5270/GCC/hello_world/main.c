/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "board_init.h"
#include "tos_shell.h"

#define APPLICATION_TASK_STK_SIZE       0x100
k_task_t application_task;
uint8_t application_task_stk[APPLICATION_TASK_STK_SIZE];
static char gs_cmd_buffer[32];

__attribute__ ((__weak__)) void application_entry(void *arg)
{
    while (1) {
        tos_task_delay(1000);
    }
}

static void board_shell_output_t(const char ch)
{
    if (ch == '\n')
    {
        while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
        {}
        UART_PutData(BOARD_DEBUG_UART_PORT, '\n');
    }
    while ( 0u == (UART_STATUS_TX_EMPTY & UART_GetStatus(BOARD_DEBUG_UART_PORT)) )
    {}
    UART_PutData(BOARD_DEBUG_UART_PORT, ch);
}

/*
 * Functions.
 */
int main(void)
{
    BOARD_Init();

    printf("hello, world\r\n");

    tos_knl_init();
    tos_task_create(&application_task, "application_task", application_entry, NULL, 4, application_task_stk, APPLICATION_TASK_STK_SIZE, 0);
    tos_shell_init(gs_cmd_buffer, sizeof(gs_cmd_buffer), board_shell_output_t, NULL);
    tos_knl_start();

    return 0;
}

/* EOF. */


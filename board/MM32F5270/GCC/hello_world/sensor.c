/******************************************************************************
* File:             sensor.c
*
* Author:           iysheng@163.com  
* Created:          08/27/22 
*                   环境传感器感知线程
*****************************************************************************/

#include <tos_k.h>
#include <tos_shell.h>
#include <stdint.h>
#include "sensor_base.h"
#include "hal_gpio.h"
#include "hal_adc.h"
#include "hal_dma.h"
#include "hal_dma_request.h"

#define APP_ADC_SEQ_LEN 2u
/* DMA for ADC  */
#define BOARD_DMA_ADC_PORT        DMA1
#define BOARD_DMA_ADC_CHN         DMA_REQ_DMA1_ADC1
#define BOARD_DMA_ADC_IRQn        DMA1_CH1_IRQn
#define BOARD_DMA_ADC_IRQHandler  DMA1_CH1_IRQHandler

/*
 * PF6 -> 温度传感器
 * PF7 -> 光照度传感器
 * */

#define SENSOR_THERMAL_CHANNEL            18u
#define SENSOR_BRIGHTNESS_CHANNEL         17u

#define SENSOR_TASK_STK_SIZE       0x1000
k_task_t sensor_task;
uint8_t sensor_task_stk[SENSOR_TASK_STK_SIZE];
volatile uint32_t __attribute__((section (".dma"))) app_dma_buff_to[APP_ADC_SEQ_LEN];
static volatile bool app_dma_xfer_done = false;

void dma4adc_init(void)
{
    DMA_Channel_Init_Type dma_channel_init;
    dma_channel_init.MemAddr = (uint32_t)(&app_dma_buff_to[0]);
    dma_channel_init.MemAddrIncMode = DMA_AddrIncMode_IncAfterXfer;
    dma_channel_init.PeriphAddr = (uint32_t)&(ADC1->ADDATA); /* ADC data register here */
    dma_channel_init.PeriphAddrIncMode = DMA_AddrIncMode_StayAfterXfer;
    dma_channel_init.Priority = DMA_Priority_High;
    dma_channel_init.ReloadMode = DMA_ReloadMode_AutoReloadContinuous;
    dma_channel_init.XferCount = APP_ADC_SEQ_LEN;
    dma_channel_init.XferMode = DMA_XferMode_PeriphToMemory;
    dma_channel_init.XferWidth = DMA_XferWidth_32b;
    DMA_InitChannel(BOARD_DMA_ADC_PORT, BOARD_DMA_ADC_CHN, &dma_channel_init);
    DMA_EnableChannel(BOARD_DMA_ADC_PORT, BOARD_DMA_ADC_CHN, true);

    /* enable DMA transfer done interrupt. */
    NVIC_EnableIRQ(BOARD_DMA_ADC_IRQn);
    DMA_EnableChannelInterrupts(BOARD_DMA_ADC_PORT, BOARD_DMA_ADC_CHN, DMA_CHN_INT_XFER_DONE, true);
}


void sensor_hw_init(void)
{
    GPIO_Init_Type gpio_init;

    /* PF6 - ADC1 channel_18 */
    gpio_init.Pins  = GPIO_PIN_6;
    gpio_init.PinMode  = GPIO_PinMode_In_Analog;
    gpio_init.Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &gpio_init);

    /* PF7 - ADC1 channel_17 */
    gpio_init.Pins  = GPIO_PIN_7;
    gpio_init.PinMode  = GPIO_PinMode_In_Analog;
    gpio_init.Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOF, &gpio_init);

    dma4adc_init();
    /* setup the converter. */
    ADC_Init_Type adc_init;
    adc_init.Resolution = ADC_Resolution_Alt0;
    adc_init.ConvMode = ADC_ConvMode_SeqOneTime;
    adc_init.Align = ADC_Align_Right;
    adc_init.SingleDiffMode = ADC_SingleDiffConvMode_SingleEnd; /* single-ended channel conversion mode. */
    adc_init.SingleVolt = ADC_SingleConvVref_Internal;  /* internal reference voltage as the reference voltage for single-ended conversion. */
    ADC_Init(ADC1, &adc_init);

    ADC_Enable(ADC1, true); /* power on the converter. */

    ADC_ResetAllSlot(ADC1);
    ADC_EnableSeqSlot(ADC1, 0, SENSOR_BRIGHTNESS_CHANNEL);
    ADC_SetChnSampleTime(ADC1, SENSOR_BRIGHTNESS_CHANNEL, ADC_SampleTime_Alt7);
    ADC_EnableSeqSlot(ADC1, 1, SENSOR_THERMAL_CHANNEL);
    ADC_SetChnSampleTime(ADC1, SENSOR_THERMAL_CHANNEL, ADC_SampleTime_Alt7);

    /* set channel sample time. */
	ADC_DoAutoCalib(ADC1);
    /* enable the DMA for transporting data. */
    ADC_EnableDMA(ADC1, true);
}

/* software tirgger the adc converter and start the sequence conversion. */
uint32_t app_adc_run_conv(uint32_t channel)
{
    uint32_t data;
    uint32_t flags;
    uint32_t adc_channel; /* keep the actual hardware conversion channel number. */

    /* software tirgger the conversion. */
    ADC_DoSwTrigger(ADC1, true);

    /* wait while the conversion is ongoing. */
    while( 0u == (ADC_GetStatus(ADC1) & ADC_STATUS_CONV_SEQ_DONE) )
    {}

    ADC_ClearStatus(ADC1, ADC_STATUS_CONV_SEQ_DONE);

    data = ADC_GetChnConvResult(ADC1, &adc_channel, &flags);
    if (0u == (flags & ADC_CONV_RESULT_FLAG_VALID) )
    {
        data = 0u; /* the data is unavailable when the VALID flag is not on. */
    }
	tos_shell_printf("channel1=%u\n", adc_channel);

    data = ADC_GetChnConvResult(ADC1, &adc_channel, &flags);
    if (0u == (flags & ADC_CONV_RESULT_FLAG_VALID) )
    {
        data = 0u; /* the data is unavailable when the VALID flag is not on. */
    }
	tos_shell_printf("channel2=%u\n", adc_channel);

    return data;
}
void sensor_entry(void *arg)
{
	uint32_t thermal_value, convert_tmp;
    sensor_hw_init();
    tos_shell_printf("Hello Sensor app\n");
    while (1) {
        ADC_DoAutoCalib(ADC1);
        ADC_DoSwTrigger(ADC1, true);
        //adc_value = app_adc_run_conv(SENSOR_THERMAL_CHANNEL);
        while (false == app_dma_xfer_done)
            tos_task_delay(1000);
app_dma_buff_to[0] &= 0xfff;
app_dma_buff_to[1] &= 0xfff;
        tos_shell_printf("Sensor thermal&brightness value raw=%u %u\n", app_dma_buff_to[0], app_dma_buff_to[1]);
        convert_tmp = 4095 * 1000 / (app_dma_buff_to[0]) - 1000;
        convert_tmp /= 1000;
        thermal_value = convert_tmp * convert_tmp * 9 - 481 * convert_tmp + 6561;
        //thermal_value /= 100;
        tos_shell_printf("Sensor thermal=%u.%u convert_tmp=%u\n", thermal_value / 100, thermal_value % 100, convert_tmp );
		/* lux = (16 - r) * 10
		 * r = 4095 / value - 1
		 * */
		if (app_dma_buff_to[1])
		{
        convert_tmp = 4095 * 1000 / (app_dma_buff_to[1]) - 1000;
        convert_tmp /= 1000;
        thermal_value = 10 * (16 - convert_tmp);
		}
		else
		{
        thermal_value = 0;
		}
        tos_shell_printf("Brightness value=%ulux \n", thermal_value);
        app_dma_xfer_done = false;
    }
}

/* entry to dma done interrupt. */
void DMA1_CH1_IRQHandler(void)
{
    ADC_DoSwTrigger(ADC1, false);
    if (0u != (DMA_CHN_INT_XFER_DONE & DMA_GetChannelInterruptStatus(BOARD_DMA_ADC_PORT, BOARD_DMA_ADC_CHN)) )
    {
        app_dma_xfer_done = true;
        DMA_ClearChannelInterruptStatus(BOARD_DMA_ADC_PORT, BOARD_DMA_ADC_CHN, DMA_CHN_INT_XFER_DONE);

        /* When enabling the DMA circular mode, the DMA counter and address pointers would be reloaded
         * automatically when the previous transfer sequence is done. The next transfer would be
         * enabled automatically as well. */
    }
}


int sensor_app_init(void *arg)
{
    return tos_task_create(&sensor_task, "sensor_task", sensor_entry, arg, 2, sensor_task_stk, SENSOR_TASK_STK_SIZE, 100);
}


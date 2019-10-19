/*
 * @Author: spy 
 * @Date: 2019-08-07 14:48:58 
 * @Last Modified by: spy
 * @Last Modified time: 2019-08-07 15:06:37
 */

#include "TF_min.h"
#include "sys.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\MCU\UART\usart5.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#include "BSP\Mecanum\Mecanum.h"

void TF_minx_UART_IRQHandler(void)
{
	static uint8_t rebuf[13]={0},i=0;
	if(USART_GetITStatus(TF_minx_UART_PORT, USART_IT_RXNE) != RESET)
	{
		rebuf[i++]=USART_ReceiveData(TF_minx_UART_PORT);
		if(rebuf[0]!=0x59)//判断帧头
			i=0;
	    if((i==2)&&(rebuf[1]!=0x59))//判断帧头
			i=0;
        if(i == 8) //数据接收完成
        {
            u16 re_sum = 0;
            for(int count = 0;count<8; count++)
            {
                re_sum += rebuf[i];
            }
            if((re_sum>>8) == rebuf[8])  //求和校验
            {
                Mecanum.X_Length = rebuf[2] + (rebuf[3]<<8);
                //u1_printf("X:%f\r\n",(float)(rebuf[2] + (rebuf[3]<<8)));
            }
            i = 0;
        }
		USART_ClearFlag(TF_minx_UART_PORT,USART_FLAG_RXNE);//清中断标志
	}	
}

void TF_miny_UART_IRQHandler(void)
{
	static uint8_t rebuf[13]={0},i=0;
	if(USART_GetITStatus(TF_miny_UART_PORT, USART_IT_RXNE) != RESET)
	{
		rebuf[i++]=USART_ReceiveData(TF_miny_UART_PORT);
		if(rebuf[0]!=0x59)//判断帧头
			i=0;
	    if((i==2)&&(rebuf[1]!=0x59))//判断帧头
			i=0;
        if(i == 8) //数据接收完成
        {
            u16 re_sum = 0;
            for(int count = 0;count<8; count++)
            {
                re_sum += rebuf[i];
            }
            if((re_sum>>8) == rebuf[8])  //求和校验
            {
                Mecanum.Y_Length = rebuf[2] + (rebuf[3]<<8);
                //u1_printf("Y:%f\r\n",(float)(rebuf[20] + (rebuf[3]<<8)));
            }
            i = 0;
        }
		USART_ClearFlag(TF_miny_UART_PORT,USART_FLAG_RXNE);//清中断标志
	}	
}

/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

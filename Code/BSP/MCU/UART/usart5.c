#include "sys.h"
#include "usart5.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "APP\Control\param.h"
#include "BSP\LED\LED.h"
#include "APP\Control\Control.h"
#include "APP\Math\My_Math.h"
#include "APP\Task_Loop\Task_Loop.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\HMI\HMI.h"

 u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//接收缓冲,最大USART5_MAX_RECV_LEN字节
 u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//发送缓冲,最大USART5_MAX_SEND_LEN字节
 u8 USART5_RX_BUF_tset[USART5_MAX_RECV_LEN];

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//串口3初始化代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/5/14
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
//////////////////////////////////////////////////////////////////////////////////


//串口发送缓存区
__align(8) u8 USART5_TX_BUF[USART5_MAX_SEND_LEN]; 	//发送缓冲,最大USART5_MAX_SEND_LEN字节
                                                    //串口接收缓存区
u8 USART5_RX_BUF[USART5_MAX_RECV_LEN]; 				//接收缓冲,最大USART5_MAX_RECV_LEN个字节.

// 发送一个字节
static void u5_putchar(u8 ch)
{
    while ((UART5->SR & 0X40) == 0);//循环发送,直到发送完毕   
    UART5->DR = (u8)ch;
    //return ch;
}


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
//void UART5_IRQHandler(void)
//{
//	u8 Res;
//    if (USART_GetFlagStatus(UART5, USART_FLAG_RXNE) != RESET)  //接收到数据
//    {
//		Res =USART_ReceiveData(UART5);//(USARTx->DR);	//读取接收到的数据
//			
//    }
//}

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void usart5_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_DeInit(UART5);  //复位串口3

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //使能GPIOC时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //使能GPIOD时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//使能USART5时钟


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOC12和GPIOD2初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure); //初始化GPIOC12，和GPIOD2
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); //GPIOB11复用为USART3
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); //GPIOB10复用为USART3

    USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART5, &USART_InitStructure); //初始化串口5

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//开启中断

    USART_Cmd(UART5, ENABLE);                    //使能串口

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART5_RX_STA=0;				//清零
}

//串口5,printf 函数
//确保一次发送数据不超过USART5_MAX_SEND_LEN字节
void u5_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART5_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART5_TX_BUF);   //此次发送数据的长度
    for (j = 0; j<i; j++)//循环发送数据
    {
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);//循环发送,直到发送完毕
        USART_SendData(UART5, (uint8_t)USART5_TX_BUF[j]);
    }
}




/*!
*  @brief      发送指定len个字节长度数组 （包括 NULL 也会发送）
*  @param      buff        数组地址
*  @param      len         发送数组的长度
*  @since      v5.0
*  Sample usage:       u5_putbuff ("1234567", 3); //实际发送了3个字节'1','2','3'
*/
void u5_putbuff(u8 *buff, u32 len)
{
    while (len--)
    {
        u5_putchar(*buff);
        buff++;
    }
}



#include "sys.h"
#include "uart4.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#include "includes.h"
#include "My_Flag.h"
#include "BSP\HMI\HMI.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
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
__align(8) u8 UART4_TX_BUF[UART4_MAX_SEND_LEN]; 	//发送缓冲,最大UART4_MAX_SEND_LEN字节
//串口接收缓存区
u8 UART4_RX_BUF[UART4_MAX_RECV_LEN]; 				//接收缓冲,最大UART4_MAX_RECV_LEN个字节.



vu16 UART4_RX_STA=0;
Flag Flag_UART4_RX = Not_Ready;
// void UART4_IRQHandler(void)
// {
//#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//     OSIntEnter();
//#endif
//    if (USART_GetFlagStatus(UART4, USART_FLAG_RXNE) != RESET)  //接收到数据
//    {
//        u8 res = 0;
//        res = USART_ReceiveData(UART4);//(USARTx->DR);	//读取接收到的数据

//        if ((UART4_RX_STA & 0x8000) == 0)//接收未完成
//        {
//            if (UART4_RX_STA & 0x4000)//接收到了0x0d
//            {
//                if (res != 0x0a)UART4_RX_STA = 0;//接收错误,重新开始
//                else
//                {
//                    uint16_t Uart_Len = 0;  //数据长度
//                    UART4_RX_STA |= 0x8000;	//接收完成了 
//                    Uart_Len = UART4_RX_STA & 0X3FFF;
//                    //请在这里添加接收字符处理函数


//                    //完成字符串处理
//                    memset((void *)UART4_RX_BUF, 0, UART4_MAX_RECV_LEN);
//                    UART4_RX_STA = 0;
//                    Flag_UART4_RX = Ready;   //接收标志位
//                }
//            }
//            else //还没收到0X0D
//            {
//                if (res == 0x0d)UART4_RX_STA |= 0x4000;
//                else
//                {
//                    UART4_RX_BUF[UART4_RX_STA & 0X3FFF] = res;
//                    UART4_RX_STA++;
//                    if (UART4_RX_STA > (UART4_MAX_RECV_LEN - 1))UART4_RX_STA = 0;//接收数据错误,重新开始接收	  
//                }
//            }
//        }
//    }

//#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
//    OSIntExit();
//#endif
// }

//初始化IO 串口4
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void uart4_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_DeInit(UART4);  //复位串口4

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);//使能UART4时钟


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1; //GPIOA1和GPIOA0初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化GPIOA1，和GPIOA0


    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_UART4); //GPIOA1复用为UART4
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); //GPIOA0复用为UART4

    USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(UART4, &USART_InitStructure); //初始化串口3

    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启中断

    USART_Cmd(UART4, ENABLE);                    //使能串口

    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //UART4_RX_STA=0;				//清零
}

//串口4,printf 函数
//确保一次发送数据不超过UART4_MAX_SEND_LEN字节
void u4_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)UART4_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)UART4_TX_BUF);   //此次发送数据的长度
    for (j = 0; j<i; j++)//循环发送数据
    {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);//循环发送,直到发送完毕
        USART_SendData(UART4, (uint8_t)UART4_TX_BUF[j]);
    }
}


// 发送一个字节
static void u4_putchar(u8 ch)
{
    while ((UART4->SR & 0X40) == 0);//循环发送,直到发送完毕   
    UART4->DR = (u8)ch;
    //return ch;
}

/*!
*  @brief      发送指定len个字节长度数组 （包括 NULL 也会发送）
*  @param      buff        数组地址
*  @param      len         发送数组的长度
*  @since      v5.0
*  Sample usage:       u4_putbuff ("1234567", 3); //实际发送了3个字节'1','2','3'
*/
void u4_putbuff(u8 *buff, u32 len)
{
    while (len--)
    {
        u4_putchar(*buff);
        buff++;
    }
}



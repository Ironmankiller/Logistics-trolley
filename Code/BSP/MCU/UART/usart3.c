#include "sys.h"
#include "usart3.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx.h"
#include "Printf_Uart.h"
#include "BSP\HMI\HMI.h"
#include "APP\OpenMV\OpenMV.h"

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
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
                                                    //串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于10ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过10ms,则认为不是1次连续数据.也就是超过10ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART3_RX_STA=0;
//void USART3_IRQHandler(void)
// {
//    if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET)  //接收到数据
//    {
//        u8 res = 0;
//        res = USART_ReceiveData(USART3);//(USARTx->DR);	//读取接收到的数据

//        if ((USART3_RX_STA & 0x8000) == 0)//接收未完成
//        {
//            if (USART3_RX_STA & 0x4000)//接收到了0x0d
//            {
//                if (res != 0x0a)USART3_RX_STA = 0;//接收错误,重新开始
//                else
//                {
//                    uint16_t Uart_Len = 0;  //数据长度
//                    USART3_RX_STA |= 0x8000;	//接收完成了 
//                    Uart_Len = USART3_RX_STA & 0X3FFF;
//                    //请在这里添加接收字符处理函数
//                    OpenMV_Data_Handle(USART3_RX_BUF,Uart_Len);
//                    //u1_printf("%d\r\n",USART3_RX_BUF);
//                    //完成字符串处理
//                    memset((void *)USART3_RX_BUF, 0, USART3_MAX_RECV_LEN);
//                    USART3_RX_STA = 0;
//                }
//            }
//            else //还没收到0X0D
//            {
//                if (res == 0x0d)USART3_RX_STA |= 0x4000;
//                else
//                {
//                    USART3_RX_BUF[USART3_RX_STA & 0X3FFF] = res;
//                    USART3_RX_STA++;
//                    if (USART3_RX_STA > (USART3_MAX_RECV_LEN - 1))USART3_RX_STA = 0;//接收数据错误,重新开始接收	  
//                }
//            }
//        }
//    }
//}

//初始化IO 串口3
//pclk1:PCLK1时钟频率(Mhz)
//bound:波特率
void usart3_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_DeInit(USART3);  //复位串口3

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//使能USART3时钟


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIOB11，和GPIOB10


    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //GPIOB11复用为USART3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //GPIOB10复用为USART3

    USART_InitStructure.USART_BaudRate = bound;//波特率一般设置为9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure); //初始化串口3

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断

    USART_Cmd(USART3, ENABLE);                    //使能串口

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

    //USART3_RX_STA=0;				//清零
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART3_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART3_TX_BUF);   //此次发送数据的长度
    for (j = 0; j<i; j++)//循环发送数据
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);//循环发送,直到发送完毕
        USART_SendData(USART3, (uint8_t)USART3_TX_BUF[j]);
    }
}


// 发送一个字节
static void u3_putchar(u8 ch)
{
    while ((USART3->SR & 0X40) == 0);//循环发送,直到发送完毕   
    USART3->DR = (u8)ch;
    //return ch;
}

/*!
*  @brief      发送指定len个字节长度数组 （包括 NULL 也会发送）
*  @param      buff        数组地址
*  @param      len         发送数组的长度
*  @since      v5.0
*  Sample usage:       u3_putbuff ("1234567", 3); //实际发送了3个字节'1','2','3'
*/
void u3_putbuff(u8 *buff, u32 len)
{
    while (len--)
    {
        u3_putchar(*buff);
        buff++;
    }
}



#include "Printf_Uart.h"	 
#include "includes.h"
#include "APP\Control\param.h"
#include "BSP\Mecanum\Mecanum.h"
#include "APP\Control\Control.h"
#include "BSP\HMI\HMI.h"
#include "APP\OpenMV\OpenMV.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "APP\Math\My_Math.h"
#include "stdlib.h"
#include "BSP\BEEP\BEEP.h"
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//串口2初始化 
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

//串口接收缓存区
vu8 USART1_RX_BUF[USART1_MAX_RECV_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//串口发送缓存区
__align(8) u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 	//发送缓冲,最大USART1_MAX_SEND_LEN字节
//接收状态标记	
vu16 USART1_RX_STA = 0;
//初始化IO 串口1
//bound:波特率 
void usart1_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);//使能USART1时钟

    //串口1引脚复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1); //GPIOA9复用为USART1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); //GPIOA10复用为USART1

    //USART1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9与GPIOA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9，PA10

     //USART1 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART1, &USART_InitStructure); //初始化串口1

    USART_Cmd(USART1, ENABLE);  //使能串口 1

    USART_ClearFlag(USART1, USART_FLAG_TC);
	
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启接受中断

    //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

}

void u1_putchar(u8 ch)
{
	while ((USART1->SR & 0X40) == 0);//循环发送,直到发送完毕   
	USART1->DR = (u8)ch;
	//return ch;
}

void u1_putbuff(u8 *buff, u32 len)
{
	while (len--)
	{
		u1_putchar(*buff);
		buff++;
	}
}


//串口1,printf 函数
//确保一次发送数据不超过USART1_MAX_SEND_LEN字节
void u1_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART1_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART1_TX_BUF);   //此次发送数据的长度
    for (j = 0; j<i; j++)//循环发送数据
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);//循环发送,直到发送完毕
        USART_SendData(USART1, (uint8_t)USART1_TX_BUF[j]);
    }
}

int i=1260;

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
    {
        u8 Res;
        Res = USART_ReceiveData(USART1);//(USARTx->DR);	//读取接收到的数据

        if ((USART1_RX_STA & 0x8000) == 0)//接收未完成
        {
            if (USART1_RX_STA & 0x4000)//接收到了0x0d
            {
                if (Res != 0x0a)USART1_RX_STA = 0;//接收错误,重新开始
                else
                {
                    USART1_RX_STA |= 0x8000;	//接收完成了 
                    //请在这里添加接收字符处理函数
                    //OpenMV_Data_Handle((char*)USART1_RX_BUF,Uart_Len);
                    if(USART1_RX_BUF[0]=='0')
                    {
                        Mecanum.state = ready;
                    } else if(USART1_RX_BUF[0]=='1')
                    {
                        Mecanum.state = back;
                    }
                    //完成字符串处理
                    memset((void *)USART1_RX_BUF, 0, USART1_MAX_RECV_LEN);
                    USART1_RX_STA = 0;
                }
            }
            else //还没收到0X0D
            {
                if (Res == 0x0d)USART1_RX_STA |= 0x4000;
                else
                {
                    USART1_RX_BUF[USART1_RX_STA & 0X3FFF] = Res;
                    USART1_RX_STA++;
                    if (USART1_RX_STA>(USART1_MAX_RECV_LEN - 1))USART1_RX_STA = 0;//接收数据错误,重新开始接收	  
                }
            }
        }
    }
}

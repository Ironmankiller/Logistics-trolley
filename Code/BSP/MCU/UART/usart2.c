#include "usart2.h"	 
#include "includes.h"
#include "APP\Control\param.h"
#include "BSP\Mecanum\Mecanum.h"
#include "APP\Control\Control.h"
#include "BSP\HMI\HMI.h"
#include "APP\OpenMV\OpenMV.h"
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


#define USART2_REC_LEN  			200  	//定义最大接收字节数 200

//注意,读取USARTx->SR能避免莫名其妙的错误
vu8 USART2_RX_BUF[USART2_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//串口发送缓存区
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
                                                    //串口接收缓存区

//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
vu16 USART2_RX_STA = 0;       //接收状态标记	
// 发送一个字节
static void u2_putchar(u8 ch)
{
	while ((USART2->SR & 0X40) == 0);//循环发送,直到发送完毕   
	USART2->DR = (u8)ch;
	//return ch;
}

void USART2_IRQHandler(void)
 {
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)  //接收到数据
    {
        u8 res = 0;
        res = USART_ReceiveData(USART2);//(USARTx->DR);	//读取接收到的数据

        if ((USART2_RX_STA & 0x8000) == 0)//接收未完成
        {
            if (USART2_RX_STA & 0x4000)//接收到了0x0d
            {
                if (res != 0x0a)USART2_RX_STA = 0;//接收错误,重新开始
                else
                {
                    //uint16_t Uart_Len = 0;  //数据长度
                    USART2_RX_STA |= 0x8000;	//接收完成了 
                    //Uart_Len = USART2_RX_STA & 0X3FFF;
                    //请在这里添加接收字符处理函数
                    //OpenMV_Data_Handle(USART2_RX_BUF,Uart_Len);
                    //完成字符串处理
                    memset((void *)USART2_RX_BUF, 0, USART2_MAX_RECV_LEN);
                    USART2_RX_STA = 0;
                }
            }
            else //还没收到0X0D
            {
                if (res == 0x0d)USART2_RX_STA |= 0x4000;
                else
                {
                    USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = res;
                    USART2_RX_STA++;
                    if (USART2_RX_STA > (USART2_MAX_RECV_LEN - 1))USART2_RX_STA = 0;//接收数据错误,重新开始接收	  
                }
            }
        }
    }
}


//初始化IO 串口2
//bound:波特率 
void usart2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//使能USART2时钟

    //串口2引脚复用映射
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA2复用为USART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA3复用为USART2

    //USART2    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2与GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2，PA3

     //USART2 初始化设置
    USART_InitStructure.USART_BaudRate = bound;//波特率设置
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure); //初始化串口2

    USART_Cmd(USART2, ENABLE);  //使能串口 2

    USART_ClearFlag(USART2, USART_FLAG_TC);
	
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、

}

/*****************  发送一个字符 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  发送多个字符 **********************/
void Usart_SendBytes( USART_TypeDef * pUSART, uint8_t * buf, uint8_t length)
{
    uint8_t i=0;
	while(i<length)
	{
		Usart_SendByte( pUSART, buf[i++]);
	}
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}



void u2_putbuff(u8 *buff, u32 len)
{
	while (len--)
	{
		u2_putchar(*buff);
		buff++;
	}
}


//串口2,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u2_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART2_TX_BUF);   //此次发送数据的长度
    for (j = 0; j<i; j++)//循环发送数据
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//循环发送,直到发送完毕
        USART_SendData(USART2, (uint8_t)USART2_TX_BUF[j]);
    }
}

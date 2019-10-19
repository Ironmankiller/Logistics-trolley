
#include <stdio.h>
#include "HMI.h"
#include "BSP\LED\LED.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "BSP\TIME\TIME.h"
#include "APP\OpenMV\OpenMV.h"
#include "BSP\MCU\UART\usart5.h"


u8 HMI_Send_Buf[USART_MAX_SEND_LEN];
u8  USART_RX_BUF[USART_MAX_RECV_LEN]; 		//接收缓冲,最大USART_MAX_RECV_LEN字节
u8  USART_TX_BUF[USART_MAX_SEND_LEN]; 		//发送缓冲,最大USART5_MAX_SEND_LEN字节
u8 USART_RX_BUF_tset[USART_MAX_RECV_LEN];
Flag Flag_UART_RX = Not_Ready;
vu16 USART_RX_STA;   						//接收数据状态
//static void HMI_Send_End(void);
static void HMISends(char *buf1);		  //字符串发送函数
static void HMISendb(u8 k);		         //字节发送函数

/*****************************************************************************\
* Function Name   : HMI_Send_Command
* Input           : None
* Output          : None
* Return          : None
* Description     : 发送字符串
\*****************************************************************************/
void HMI_Send_Command(char *buffer)
{
    HMISends(buffer);
    HMISendb(0xFF);
}

//===============================================================================


//-----------------------------------------------------------------------------
// Static Function
// 内部函数
//-----------------------------------------------------------------------------

static void HMISends(char *buf1)		  //字符串发送函数
{
    u8 i = 0;
    while (1)
    {
        if (buf1[i] != 0)
        {
            USART_SendData(HMI_UART_PORT, buf1[i]);  //发送一个字节
            while (USART_GetFlagStatus(HMI_UART_PORT, USART_FLAG_TXE) == RESET) {};//等待发送结束
            i++;
        }
        else
            return;

    }
}
static void HMISendb(u8 k)		         //字节发送函数
{
    u8 i;
    for (i = 0; i<3; i++)
    {
        if (k != 0)
        {
            USART_SendData(HMI_UART_PORT, k);  //发送一个字节
            while (USART_GetFlagStatus(HMI_UART_PORT, USART_FLAG_TXE) == RESET) {};//等待发送结束
        }
        else
            return;

    }
}

#include "APP\Task_Loop\Task_Loop.h"
#include "APP\Control\Control.h"
#include "BSP\Motor_Drive\Moto_Drive.h"
#include "APP\IMU\ahrs.h"
#include "APP\Control\param.h"
#include "My_Flag.h"

/**********************************************
* HMI数据解析执行
**********************************************/
void HMI_Data_Handle(uint8_t* data_buf)
{
	while (Flag_UART_RX)
	{
		if (!(*(data_buf) == 0xAA && *(data_buf + 1) == 0xAF))		return;		//判断帧头
		switch (*(data_buf + 2))
		{
		case 0x11:

			break;
		case 0x12:

			break;
		case 0x13:	

			break;
		case 0x21:	

			break;
		case 0x22:	

			break;
		case 0x23:	

			break;
		case 0x03:

			break;
		case 0x41:								//

			break;
        case 0xB0:

		default:
			break;
		}
        Flag_UART_RX = MY_FALSE; 
	}
}   

void HMI_Send_Pid()
{
	HMI_Send_Buf[0] = 0xAA;
	HMI_Send_Buf[1] = 0xAA;
#if defined(HMI_PID1)
	HMI_Send_Buf[2] = 0x05;
	for (int i = 1; i < 4; i++)
	{
		switch (i)
		{
		case 1:
			HMI_Send_Buf[3] = 'P';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID1[0] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			break;
		case 2:
			while (!Flag_UART_RX)
			{
				LED_Blink(LED_4);
			}
			HMI_Send_Buf[3] = 'I';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID1[1] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			break;
		case 3:
			while (!Flag_UART_RX)
			{
				LED_Blink(LED_4);
			}
			HMI_Send_Buf[3] = 'D';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID1[2] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			while (!Flag_UART_RX)
			{
				LED_Blink(LED_2);
			}
			break;
		default:
			break;
		}
	}
#endif
#if defined(HMI_PID2)
	HMI_Send_Buf[2] = 0x06;
	for (int i = 1; i < 4; i++)
	{
		switch (i)
		{
		case 1:
			HMI_Send_Buf[3] = 'P';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID2[0] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			break;
		case 2:
			while (!Flag_UART_RX)
			{
				LED_Blink(LED_4);
			}
			HMI_Send_Buf[3] = 'I';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID2[1] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			break;
		case 3:
			while (!Flag_UART_RX)
			{
				LED_Blink(LED_4);
			}
			HMI_Send_Buf[3] = 'D';
			u5_putbuff(HMI_Send_Buf, 4);
			u5_printf("%.0f", HMI_PID2[2] * HMI_K);
			u5_putbuff("\r\n", 2);
			Flag_UART_RX = Not_Ready;
			break;
		default:
			break;
		}
	}
#endif
#if defined(HMI_PID3)
	HMI_Send_Buf[2] = 0x05;
#endif
#if defined(HMI_PID4)
	HMI_Send_Buf[2] = 0x06;
#endif
}

void PS2_Control(uint8_t* buf)
{
}

#if (HMI_UART_PORT_SELECT == 1)
void USART1_IRQHandler(void)
#elif (HMI_UART_PORT_SELECT == 2)
void USART2_IRQHandler(void)
#elif (HMI_UART_PORT_SELECT == 3)
void USART3_IRQHandler(void)
#elif (HMI_UART_PORT_SELECT == 4)
void UART4_IRQHandler(void)
#elif (HMI_UART_PORT_SELECT == 5)
void UART5_IRQHandler(void)
#endif
{
	u8 Res;
    if (USART_GetFlagStatus(HMI_UART_PORT, USART_FLAG_RXNE) != RESET)  //接收到数据
    {
		Res =USART_ReceiveData(HMI_UART_PORT);//(USARTx->DR);	//读取接收到的数据
			
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
				if(Res!=0x0a)USART_RX_STA=0;//接收错误,重新开始
				else
				{
					USART_RX_STA |= 0x8000;	//接收完成了 
					USART_RX_STA = 0;
					sprintf(USART_RX_BUF, "%s", USART_RX_BUF_tset);
					memset(USART_RX_BUF_tset,0, USART_MAX_RECV_LEN);
					Flag_UART_RX = Ready;
				}
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF_tset[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_MAX_RECV_LEN-1))USART_RX_STA = 0;//接收数据错误,重新开始接收	  
				}		 
			}
		}   
	}			
}

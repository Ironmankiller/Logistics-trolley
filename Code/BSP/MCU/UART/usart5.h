#ifndef __USART5_H
#define __USART5_H
#include "./sys/sys.h"
#include "stdio.h"
#include "APP\Control\param.h"
#include "BSP\SYSTEM\My_Flag.h"
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

#define USART5_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART5_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART5_RX_EN 			1								//0,不接收;1,接收.
extern u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//接收缓冲,最大USART5_MAX_RECV_LEN字节
extern u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//发送缓冲,最大USART5_MAX_SEND_LEN字节
extern vu16 USART5_RX_STA;   						//接收数据状态
extern Flag Flag_UART5_RX;
void usart5_init(u32 bound);
void u5_printf(char* fmt,...);
void u5_putbuff(u8 *buff, u32 len);
#endif

















#ifndef __UART4_H
#define __UART4_H
#include "sys.h"
#include "stdio.h"
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

#include "My_Flag.h"

#define UART4_MAX_RECV_LEN		400					//最大接收缓存字节数
#define UART4_MAX_SEND_LEN		400					//最大发送缓存字节数
#define UART4_RX_EN 			1					//0,不接收;1,接收.

extern Flag Flag_UART4_RX;

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//接收缓冲,最大UART4_MAX_RECV_LEN字节
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//发送缓冲,最大UART4_MAX_SEND_LEN字节
extern vu16 UART4_RX_STA;   						//接收数据状态

void uart4_init(u32 bound);
void u4_printf(char* fmt, ...);
void u4_putbuff(u8 *buff, u32 len);
#endif






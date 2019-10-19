#ifndef __HMI_H
#define __HMI_H

#include <stdint.h>
#include "BSP\SYSTEM\My_Flag.h"

#define HMI_UART_PORT_SELECT 4

// 通信UART口定义
#if (HMI_UART_PORT_SELECT == 1)
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#define HMI_UART_PORT USART1
#define UART_Send_Char(ch) u1_putchar(ch)
#define UART_Send_String(string) printf(string)
#elif (HMI_UART_PORT_SELECT == 2)
#include "BSP/MCU/UART/usart2.h"
#define HMI_UART_PORT USART2
#define UART_Send_Char(ch) u2_putchar(ch)
#define UART_Send_String(string) u2_printf(string)
#elif (HMI_UART_PORT_SELECT == 3)
#include "BSP/MCU/UART/usart3.h"
#define HMI_UART_PORT USART3
#define UART_Send_Char(ch) u3_putchar(ch)
#define UART_Send_String(string) u3_printf(string)
#elif (HMI_UART_PORT_SELECT == 4)
#include "BSP/MCU/UART/uart4.h"
#define HMI_UART_PORT UART4
#define UART_Send_Char(ch) u4_putchar(ch)
#define UART_Send_String(string) u4_printf(string)
#elif (HMI_UART_PORT_SELECT == 5)
#include "BSP/MCU/UART/usart5.h"
#define HMI_UART_PORT UART5
#define UART_Send_Char(ch) u5_putchar(ch)
#define UART_Send_String(string) u5_printf(string)
#define UART_Send_Buff(buff) u5_putbuff(buff)
#else
#error "Please define ANO_DT_UART_PORT!!!"
#endif

/**********************************************
* HMI用户级定义区
**********************************************/
#define USART_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART_MAX_SEND_LEN		400					//最大发送缓存字节数
#define HMI_K  1000.0l   //发给串口屏要乘的倍数，对应小数点位数
#define HMI_K_BACK 1.0/HMI_K

/**********************************************
* 定义HMI控制的PID参数数组
**********************************************/
#define HMI_PID1	pidmotorAspeedparm
#define HMI_PID2	pidmotorBspeedparm

/**********************************************
* 发送HMI指令
**********************************************/
void HMI_Send_Command(char *buffer);

/**********************************************
* HMI数据解析执行
**********************************************/
void HMI_Data_Handle(uint8_t* data_buf);

/**********************************************
* HMI PID参数发送
**********************************************/
void HMI_Send_Pid(void);

/**********************************************
* HMI 遥控数据控制
**********************************************/
void PS2_Control(uint8_t* buf);

extern Flag Flag_UART_RX;
extern u8 USART_RX_BUF[USART_MAX_RECV_LEN]; 		//接收缓冲,最大USART_MAX_RECV_LEN字节

#endif

#ifndef __CAN_H
#define __CAN_H

/************************使用F4 CAN通信***************************/
#if defined STM32F40_41xxx 

#include "stm32f4xx.h" 

#define CAN1_RX_STD_ID Address_UnderPan         //定义接收标准CAN1 地址
#define CAN2_RX_STD_ID Address_UnderPan          //定义接收标准CAN2 地址

//CAN1,2的接收中断使能
#define CAN1_RX0_INT_ENABLE	1		//0,不使能;1,使能.
#define CAN2_RX0_INT_ENABLE	1		//0,不使能;1,使能.

#define CAN2_USE_ENABLE     1       //0,不使用CAN2;1 使用CAN2

void CAN_Config(void);                //F4的CAN初始化   500kb

u8 CAN1_SendMsg(u8* msg, u32 TX_STD_ID);
u8 CAN1_ReceiveMsg(u8* buf);

u8 CAN2_SendMsg(u8* msg, u32 TX_STD_ID);
u8 CAN2_ReceiveMsg(u8* buf);

#endif
/******************************************************************/

/************************使用F1 CAN通信****************************/
#if defined (STM32F10X_HD) || defined (STM32F10X_MD) 

#include "stm32f10x.h" 

#define CAN1_RX0_INT_ENABLE	   1		         //0,不使能;1,使能.
#define CAN1_RX_STD_ID         0X12          //定义F1接收标准CAN1 地址

void CAN_Config(void);                       //F1的CAN初始化   500kb
u8 CAN1_ReceiveMsg(u8 *buf);
u8 CAN1_SendMsg(u8* msg, u32 TX_STD_ID);

#endif
/*******************************************************************/

#endif

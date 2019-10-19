#ifndef _GY953_H
#define _GY953_H
#include "sys.h"

// -----用到的UART口-----
// USART1 -- 1
// USART2 -- 2
// USART3 -- 3
// USART4 -- 4
#define GY953_UART_PORT_SELECT 2

// 通信UART口定义
#if (GY953_UART_PORT_SELECT == 1)
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#define GY953_UART_PORT USART1
#define GY953_UART_IRQHandler USART1_IRQHandler
#elif (GY953_UART_PORT_SELECT == 2)
#include "BSP/MCU/UART/usart2.h"
#define GY953_UART_PORT USART2
#define GY953_UART_IRQHandler USART2_IRQHandler
#elif (GY953_UART_PORT_SELECT == 3)
#include "BSP/MCU/UART/usart3.h"
#define GY953_UART_PORT USART3
#define GY953_UART_IRQHandler USART3_IRQHandler
#elif (GY953_UART_PORT_SELECT == 4)
#include "BSP/MCU/UART/usart4.h"
#define GY953_UART_PORT USART4
#define GY953_UART_IRQHandler USART4_IRQHandler
#else
#define spy "nb"
#endif

#define ACC 0X01//加计控制位
#define GYR 0X02//陀螺控制位
#define MAG 0X04//磁场控制位
#define RPY 0X08//欧拉角控制位
#define Q4  0X10//四元数控制位

extern uint8_t stata_reg;
extern uint8_t data1_buf[7],data2_buf[7],data3_buf[7],
data4_buf[7],data5_buf[9];

void GY953_Init(void);
void GY953_Data_Prepare(void);
uint8_t GY953_Data_Check(uint8_t *data);
void GY953_Output_Speed(uint8_t f);
void GY953_Calibrate(void);



#endif /*_GY953_H*/

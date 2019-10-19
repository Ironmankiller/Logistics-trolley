/*
 * Uart_Command_Dispose.h
 *
 *  Created on: 2016年11月27日
 *      Author: DGVY
 */

/************************************************************************/
// 模块内部头文件，外部不要引用
/************************************************************************/

/************************************************************************/
//  指令格式：
//          Command_Head+Command_Body=([xxxx[,xxxx[,...]]])
/************************************************************************/

#ifndef CODE_BSP_UART_COMMAND_UART_COMMAND_DISPOSE_H_
#define CODE_BSP_UART_COMMAND_UART_COMMAND_DISPOSE_H_

#include <stdint.h>

// 命令头长度
#define COMMOND_HEAD_LEN 10

// 命令体长度
#define COMMOND_BODY_LEN 20


// SYSTEM
void SYSTEM_TEST(uint8_t *parameter);
void SYSTEM_RESET(uint8_t *parameter);

// RUNTO
void RUNTO_LOCATION(uint8_t *parameter);
void RUNTO_GO(uint8_t *parameter);
void RUNTO_STOP(uint8_t *parameter);
void RUNTO_ORIGIN(uint8_t *parameter);

// SEND
void SEND_COORDS(uint8_t *parameter);
void SEND_ANGLE(uint8_t *parameter);
void SEND_ALL(uint8_t *parameter);
void SEND_CONTINUE_COORDS(uint8_t *parameter);
void SEND_CONTINUE_ANGLE(uint8_t *parameter);
void SEND_CONTINUE_ALL(uint8_t *parameter);
void SEND_CLOSE(uint8_t *parameter);

// SET
void SET_MODE(uint8_t *parameter);
void SET_GROUND(uint8_t *parameter);


// 函数指针数组
extern void(*Command_Function[][10])(uint8_t *parameter);

extern const uint8_t Command_Head[][COMMOND_HEAD_LEN];
extern const uint8_t Command_Body[][10][COMMOND_BODY_LEN];

#endif /* CODE_BSP_UART_COMMAND_UART_COMMAND_DISPOSE_H_ */

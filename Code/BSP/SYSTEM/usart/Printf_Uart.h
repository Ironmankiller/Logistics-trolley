#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "My_Flag.h"

void usart1_init(u32 bound);
void u1_putchar(u8 ch);
void u1_putbuff(u8 *buff, u32 len);
void u1_printf(char* fmt, ...);

#define USART1_MAX_RECV_LEN		400					//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		400					//最大发送缓存字节数
#define USART1_RX_EN 			1					//0,不接收;1,接收.


#endif



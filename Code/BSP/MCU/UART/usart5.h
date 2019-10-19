#ifndef __USART5_H
#define __USART5_H
#include "./sys/sys.h"
#include "stdio.h"
#include "APP\Control\param.h"
#include "BSP\SYSTEM\My_Flag.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����3��ʼ������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/14
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
//////////////////////////////////////////////////////////////////////////////////

#define USART5_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART5_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART5_RX_EN 			1								//0,������;1,����.
extern u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//���ջ���,���USART5_MAX_RECV_LEN�ֽ�
extern u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//���ͻ���,���USART5_MAX_SEND_LEN�ֽ�
extern vu16 USART5_RX_STA;   						//��������״̬
extern Flag Flag_UART5_RX;
void usart5_init(u32 bound);
void u5_printf(char* fmt,...);
void u5_putbuff(u8 *buff, u32 len);
#endif

















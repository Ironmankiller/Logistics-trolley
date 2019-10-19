#ifndef __UART4_H
#define __UART4_H
#include "sys.h"
#include "stdio.h"
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

#include "My_Flag.h"

#define UART4_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define UART4_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define UART4_RX_EN 			1					//0,������;1,����.

extern Flag Flag_UART4_RX;

extern u8  UART4_RX_BUF[UART4_MAX_RECV_LEN]; 		//���ջ���,���UART4_MAX_RECV_LEN�ֽ�
extern u8  UART4_TX_BUF[UART4_MAX_SEND_LEN]; 		//���ͻ���,���UART4_MAX_SEND_LEN�ֽ�
extern vu16 UART4_RX_STA;   						//��������״̬

void uart4_init(u32 bound);
void u4_printf(char* fmt, ...);
void u4_putbuff(u8 *buff, u32 len);
#endif






#include "sys.h"
#include "usart5.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "APP\Control\param.h"
#include "BSP\LED\LED.h"
#include "APP\Control\Control.h"
#include "APP\Math\My_Math.h"
#include "APP\Task_Loop\Task_Loop.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\HMI\HMI.h"

 u8  USART5_RX_BUF[USART5_MAX_RECV_LEN]; 		//���ջ���,���USART5_MAX_RECV_LEN�ֽ�
 u8  USART5_TX_BUF[USART5_MAX_SEND_LEN]; 		//���ͻ���,���USART5_MAX_SEND_LEN�ֽ�
 u8 USART5_RX_BUF_tset[USART5_MAX_RECV_LEN];

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


//���ڷ��ͻ�����
__align(8) u8 USART5_TX_BUF[USART5_MAX_SEND_LEN]; 	//���ͻ���,���USART5_MAX_SEND_LEN�ֽ�
                                                    //���ڽ��ջ�����
u8 USART5_RX_BUF[USART5_MAX_RECV_LEN]; 				//���ջ���,���USART5_MAX_RECV_LEN���ֽ�.

// ����һ���ֽ�
static void u5_putchar(u8 ch)
{
    while ((UART5->SR & 0X40) == 0);//ѭ������,ֱ���������   
    UART5->DR = (u8)ch;
    //return ch;
}


//ͨ���жϽ�������2���ַ�֮���ʱ������10ms�������ǲ���һ������������.
//���2���ַ����ռ������10ms,����Ϊ����1����������.Ҳ���ǳ���10msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
//void UART5_IRQHandler(void)
//{
//	u8 Res;
//    if (USART_GetFlagStatus(UART5, USART_FLAG_RXNE) != RESET)  //���յ�����
//    {
//		Res =USART_ReceiveData(UART5);//(USARTx->DR);	//��ȡ���յ�������
//			
//    }
//}

//��ʼ��IO ����3
//pclk1:PCLK1ʱ��Ƶ��(Mhz)
//bound:������
void usart5_init(u32 bound)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    USART_DeInit(UART5);  //��λ����3

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); //ʹ��GPIOCʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); //ʹ��GPIODʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE);//ʹ��USART5ʱ��


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOC12��GPIOD2��ʼ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOC, &GPIO_InitStructure); //��ʼ��GPIOC12����GPIOD2
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_UART5); //GPIOB11����ΪUSART3
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_UART5); //GPIOB10����ΪUSART3

    USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(UART5, &USART_InitStructure); //��ʼ������5

    USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�

    USART_Cmd(UART5, ENABLE);                    //ʹ�ܴ���

    NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

    //USART5_RX_STA=0;				//����
}

//����5,printf ����
//ȷ��һ�η������ݲ�����USART5_MAX_SEND_LEN�ֽ�
void u5_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART5_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART5_TX_BUF);   //�˴η������ݵĳ���
    for (j = 0; j<i; j++)//ѭ����������
    {
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);//ѭ������,ֱ���������
        USART_SendData(UART5, (uint8_t)USART5_TX_BUF[j]);
    }
}




/*!
*  @brief      ����ָ��len���ֽڳ������� ������ NULL Ҳ�ᷢ�ͣ�
*  @param      buff        �����ַ
*  @param      len         ��������ĳ���
*  @since      v5.0
*  Sample usage:       u5_putbuff ("1234567", 3); //ʵ�ʷ�����3���ֽ�'1','2','3'
*/
void u5_putbuff(u8 *buff, u32 len)
{
    while (len--)
    {
        u5_putchar(*buff);
        buff++;
    }
}



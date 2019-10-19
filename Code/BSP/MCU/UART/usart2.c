#include "usart2.h"	 
#include "includes.h"
#include "APP\Control\param.h"
#include "BSP\Mecanum\Mecanum.h"
#include "APP\Control\Control.h"
#include "BSP\HMI\HMI.h"
#include "APP\OpenMV\OpenMV.h"
//////////////////////////////////////////////////////////////////////////////////	   
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����2��ʼ�� 
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


#define USART2_REC_LEN  			200  	//�����������ֽ��� 200

//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���
vu8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//���ڷ��ͻ�����
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
                                                    //���ڽ��ջ�����

//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
vu16 USART2_RX_STA = 0;       //����״̬���	
// ����һ���ֽ�
static void u2_putchar(u8 ch)
{
	while ((USART2->SR & 0X40) == 0);//ѭ������,ֱ���������   
	USART2->DR = (u8)ch;
	//return ch;
}

void USART2_IRQHandler(void)
 {
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)  //���յ�����
    {
        u8 res = 0;
        res = USART_ReceiveData(USART2);//(USARTx->DR);	//��ȡ���յ�������

        if ((USART2_RX_STA & 0x8000) == 0)//����δ���
        {
            if (USART2_RX_STA & 0x4000)//���յ���0x0d
            {
                if (res != 0x0a)USART2_RX_STA = 0;//���մ���,���¿�ʼ
                else
                {
                    //uint16_t Uart_Len = 0;  //���ݳ���
                    USART2_RX_STA |= 0x8000;	//��������� 
                    //Uart_Len = USART2_RX_STA & 0X3FFF;
                    //����������ӽ����ַ�������
                    //OpenMV_Data_Handle(USART2_RX_BUF,Uart_Len);
                    //����ַ�������
                    memset((void *)USART2_RX_BUF, 0, USART2_MAX_RECV_LEN);
                    USART2_RX_STA = 0;
                }
            }
            else //��û�յ�0X0D
            {
                if (res == 0x0d)USART2_RX_STA |= 0x4000;
                else
                {
                    USART2_RX_BUF[USART2_RX_STA & 0X3FFF] = res;
                    USART2_RX_STA++;
                    if (USART2_RX_STA > (USART2_MAX_RECV_LEN - 1))USART2_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
                }
            }
        }
    }
}


//��ʼ��IO ����2
//bound:������ 
void usart2_init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); //ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);//ʹ��USART2ʱ��

    //����2���Ÿ���ӳ��
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA2����ΪUSART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA3����ΪUSART2

    //USART2    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA2��PA3

     //USART2 ��ʼ������
    USART_InitStructure.USART_BaudRate = bound;//����������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure); //��ʼ������2

    USART_Cmd(USART2, ENABLE);  //ʹ�ܴ��� 2

    USART_ClearFlag(USART2, USART_FLAG_TC);
	
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

}

/*****************  ����һ���ַ� **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx,ch);
		
	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/*****************  ���Ͷ���ַ� **********************/
void Usart_SendBytes( USART_TypeDef * pUSART, uint8_t * buf, uint8_t length)
{
    uint8_t i=0;
	while(i<length)
	{
		Usart_SendByte( pUSART, buf[i++]);
	}
}

/*****************  �����ַ��� **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* �ȴ�������� */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* ȡ���߰�λ */
	temp_h = (ch&0XFF00)>>8;
	/* ȡ���Ͱ�λ */
	temp_l = ch&0XFF;
	
	/* ���͸߰�λ */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}



void u2_putbuff(u8 *buff, u32 len)
{
	while (len--)
	{
		u2_putchar(*buff);
		buff++;
	}
}


//����2,printf ����
//ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char*)USART2_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char*)USART2_TX_BUF);   //�˴η������ݵĳ���
    for (j = 0; j<i; j++)//ѭ����������
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);//ѭ������,ֱ���������
        USART_SendData(USART2, (uint8_t)USART2_TX_BUF[j]);
    }
}

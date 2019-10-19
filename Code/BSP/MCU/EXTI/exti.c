/*
 * @Author: spy 
 * @Date: 2019-08-06 10:56:32 
 * @Last Modified by:   spy 
 * @Last Modified time: 2019-08-06 10:56:32 
 */

#include "exti.h"
#include "BSP\LED\LED.h"
 /**
  * @brief  ����Ƕ�������жϿ�����NVIC
  * @param  ��
  * @retval ��
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ����NVICΪ���ȼ���1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* �����ж�Դ������1 */
  NVIC_InitStructure.NVIC_IRQChannel = NRF_INT_EXTI_IRQ;
  /* ������ռ���ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ���������ȼ���1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ʹ���ж�ͨ�� */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

 /**
  * @brief  ���� PA0 Ϊ���жϿڣ��������ж����ȼ�
  * @param  ��
  * @retval ��
  */
void EXTI_NRF_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*��������GPIO�ڵ�ʱ��*/
	RCC_AHB1PeriphClockCmd(NRF_INT_GPIO_CLK ,ENABLE);
  
    /* ʹ�� SYSCFG ʱ�� ��ʹ��GPIO�ⲿ�ж�ʱ����ʹ��SYSCFGʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
    /* ���� NVIC */
    NVIC_Configuration();
  
	/* ѡ�񰴼�1������ */ 
    GPIO_InitStructure.GPIO_Pin = NRF_INT_GPIO_PIN;
    /* ��������Ϊ����ģʽ */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    /* �������Ų�����Ҳ������ */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    /* ʹ������Ľṹ���ʼ������ */
    GPIO_Init(NRF_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* ���� EXTI �ж�Դ������ */
    SYSCFG_EXTILineConfig(NRF_INT_EXTI_PORTSOURCE,NRF_INT_EXTI_PINSOURCE);

    /* ѡ�� EXTI �ж�Դ */
    EXTI_InitStructure.EXTI_Line = NRF_INT_EXTI_LINE;
    /* �ж�ģʽ */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* �½��ش��� */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    /* ʹ���ж�/�¼��� */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

}


void NRF_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);//�����־
        LED_SWITCH(LED_2);
    }
}
/******************* (C) COPYRIGHT 2019 SPY **********END OF FILE***********/

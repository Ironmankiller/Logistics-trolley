#include "stm32f4xx.h"
#include "Timer.h"
#include "BSP\LED\LED.h"

/****************************************************************************************
*�� �� ��:bsp_Inittimer1
*��������:��ʼ����ʱ��1
*��    ��:��
*�� �� ֵ:��
*****************************************************************************************/
void TIM1_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);/*ʹ�ܶ�ʱ��1��ʱ��*/


    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;/*��ʱ��1���ж�ͨ��ʹ��*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*��ʱ��1���ж�ͨ��ʹ��*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/*��ռ���ȼ�*/
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/*��Ӧ���ȼ�*/
    NVIC_Init(&NVIC_InitStructure);/*�����жϷ��飬��ʹ���ж�*/

    TIM_TimeBaseInitStrecture.TIM_Period = 10000;/*��װ�ؼĴ���*/
    TIM_TimeBaseInitStrecture.TIM_Prescaler = 4200;/*Ԥ����*/
    TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*ʱ�ӷ�Ƶ*/
    TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*���ϼ���*/
    TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*�ظ������Ĵ���*/
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStrecture);/*��ʼ��*/

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);/*����±�־λ*/
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);/*ʹ���ж�*/
    TIM_Cmd(TIM1, ENABLE);                  /*ʹ�ܼ���*/
}



/****************************************************************************************
*�� �� ��:TIM1_UP_TIM10_IRQHandler
*��������:��ʱ��1���жϷ�����
*��    ��:��
*�� �� ֵ:��
*****************************************************************************************/
//void TIM1_UP_TIM10_IRQHandler(void)
//{
//    static unsigned char num;
//    num++;
//    if (num % 2 == 0)
//    {
//        LED0 = 1;
//        LED1 = 1;
//    }
//    else
//    {
//        LED0 = 0;
//        LED1 = 0;
//    }
//
//    TIM_ClearFlag(TIM1, TIM_FLAG_Update);/*���жϱ�־*/
//}
//

#include "stm32f4xx.h"
#include "Timer.h"
#include "BSP\LED\LED.h"

/****************************************************************************************
*函 数 名:bsp_Inittimer1
*函数功能:初始化定时器1
*形    参:无
*返 回 值:无
*****************************************************************************************/
void TIM1_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrecture;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);/*使能定时器1的时钟*/


    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;/*定时器1的中断通道使能*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;/*定时器1的中断通道使能*/
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;/*抢占优先级*/
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;/*响应优先级*/
    NVIC_Init(&NVIC_InitStructure);/*配置中断分组，并使能中断*/

    TIM_TimeBaseInitStrecture.TIM_Period = 10000;/*重装载寄存器*/
    TIM_TimeBaseInitStrecture.TIM_Prescaler = 4200;/*预分配*/
    TIM_TimeBaseInitStrecture.TIM_ClockDivision = TIM_CKD_DIV1;/*时钟分频*/
    TIM_TimeBaseInitStrecture.TIM_CounterMode = TIM_CounterMode_Up;/*向上计数*/
    TIM_TimeBaseInitStrecture.TIM_RepetitionCounter = 0;/*重复计数寄存器*/
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStrecture);/*初始化*/

    TIM_ClearFlag(TIM1, TIM_FLAG_Update);/*清更新标志位*/
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);/*使能中断*/
    TIM_Cmd(TIM1, ENABLE);                  /*使能计数*/
}



/****************************************************************************************
*函 数 名:TIM1_UP_TIM10_IRQHandler
*函数功能:定时器1的中断服务函数
*形    参:无
*返 回 值:无
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
//    TIM_ClearFlag(TIM1, TIM_FLAG_Update);/*清中断标志*/
//}
//

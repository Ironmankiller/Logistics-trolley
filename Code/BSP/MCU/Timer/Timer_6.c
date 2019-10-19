//-----------------------------------------------------------------------------
// Timer_6.c
//
//  Created on	: 2016-11-3
//      Author	: DGVY
//		version	: V1.0
//		brief	:
//-----------------------------------------------------------------------------
// Attention:
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Timer.h"
#include "My_Flag.h"

//-----------------------------------------------------------------------------
// Private Typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Macro
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

//===============================================================================


//-----------------------------------------------------------------------------
// Extern Function
// 外部函数
//-----------------------------------------------------------------------------

//通用定时器6中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
void Timer6_Interrupt_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);  //使能TIM6时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStructure);

    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE); //允许定时器6更新中断
    //TIM_Cmd(TIM6, ENABLE); //使能定时器6

    NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //抢占优先级0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03; //子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

// 开启Timer6定时器
void Timer6_ON(void)
{
    TIM_Cmd(TIM6, ENABLE);
}

// 关闭Timer6定时器
void Timer6_OFF(void)
{
    TIM_Cmd(TIM6, DISABLE);
}


//===============================================================================

#if defined (TIMER_6_INTERRUPT_FUNCTION_ENABLE)
//-----------------------------------------------------------------------------
// Interrupt Function
// 中断函数
//-----------------------------------------------------------------------------
//定时器6中断服务程序	 
void TIM6_DAC_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) == SET) //溢出中断
    {

    }
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);  //清除中断标志位
}
#endif

/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/

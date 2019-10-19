#include "stm32f4xx.h"

/**
* @brief  配置TIM10输出的PWM_CH1信号的模式
* @param  无
* @retval 无
*/
void TIM10_CH1_PWM_Init(uint16_t arr, u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10, ENABLE);  	//TIM10时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM10); //复用

    TIM_TimeBaseStructure.TIM_Prescaler = psc;  //定时器分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseStructure.TIM_Period = arr;   //自动重装载值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);//初始化定时器9

    //初始化TIM14 Channel1 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC1Init(TIM10, &TIM_OCInitStructure);           //根据T指定的参数初始化外设TIM10 OC1
    TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);  //使能TIM10在CCR1上的预装载寄存器

    TIM_ARRPreloadConfig(TIM10, ENABLE);    //ARPE使能 
    TIM_Cmd(TIM10, DISABLE);                 //使能TIM10 
    /* 配置输出比较通道1中断*/
    TIM_ITConfig(TIM10, TIM_IT_CC1, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;//TIM11中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//占先式优先级设置为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //副优先级设置为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//中断使能
    NVIC_Init(&NVIC_InitStructure);//中断初始化
}

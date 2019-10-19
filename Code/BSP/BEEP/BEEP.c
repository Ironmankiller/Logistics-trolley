#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "BEEP.h"
#define BEEP   TIM9->CCR1  //PB15
/**********************************************
* 初始化蜂鸣器
**********************************************/
void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //初始化PE5

    //初始化TIM9
    TIM_TimeBaseStructure.TIM_Period = 720-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 64; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM9 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC4
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //使能TIM1在CCR4上的预装载寄存器
    
    TIM_CtrlPWMOutputs(TIM9,ENABLE); 
    TIM_Cmd(TIM9, ENABLE);  //使能TIM1
}

/**********************************************
* 蜂鸣器开
**********************************************/
void Beep_On(void)
{
	//TIM_SetCompare4(TIM1, (TIM1->ARR)/2);
    BEEP = 500;
}

/**********************************************
* 蜂鸣器关
**********************************************/
void Beep_Off(void)
{
	BEEP = 0;
}

void Beep_Play(u16 t)
{
	BEEP = 500;
	delay_ms(t);
	BEEP = 0;
}

uint8_t beep_delay = 0;
void Beep_BB_Set(void)
{
    beep_delay = 3;
    Beep_On();
}

void Beep_BB(void)
{
    beep_delay--;
    if (!beep_delay)
    {
        Beep_Off();
    }
}





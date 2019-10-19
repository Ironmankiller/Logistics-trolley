#include "stm32f4xx.h"
#include "Wind_Moto.h"
#include "APP\Math\My_Math.h"
#include "sys.h"
#include "My_Flag.h"

#define PWMA2   TIM1->CCR1  //PE9 CH1
#define PWMA1   TIM1->CCR2  //PE11 CH2

#define PWMB1   TIM8->CCR3  //PC8 CH3
#define PWMB2    TIM8->CCR4  //PC9 CH4

#define OFF  MY_FALSE
#define ON   MY_TRUE
Flag Flag_Wind_Moto = ON;
#define death 0
/**********************************************
* 电机初始化
**********************************************/
void Wind_Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //电机A1A2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  //电机B1B2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8); //PC8 9复用为定时器8 CH3 CH4
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);//PE9 11复用为定时器1 CH1 CH2
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);              //初始化PC9 8
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
    GPIO_Init(GPIOE, &GPIO_InitStructure);


    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM1 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器

    TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC2
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR2上的预装载寄存器
    TIM_CtrlPWMOutputs(TIM1,ENABLE);

    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
    
    //初始化TIM8
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM8 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

    TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM8 OC4
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM8在CCR3上的预装载寄存器

    TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM8 OC4
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //使能TIM8在CCR4上的预装载寄存器
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    TIM_Cmd(TIM8, ENABLE);  //使能TIM8
}


/**********************************************
* 设置PWM
**********************************************/
void Wind_Moto_A_Set(int32_t pwm)
{
    if (Flag_Wind_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        PWMA1 = pwm + death;
        PWMA2 = 0;
    }
    else if (pwm < 0)
    {
        PWMA1 = 0;
        PWMA2 = -pwm - death;
    }
    else
    {
        Wind_Moto_A_Stop();
        return;
    }
}

void Wind_Moto_B_Set(int32_t pwm)
{
    if (Flag_Wind_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        PWMB1 = pwm + death;
        PWMB2 = 0;
    }
    else if (pwm < 0)
    {
        PWMB1 = 0;
        PWMB2 = -pwm - death;
    }
    else
    {
        Wind_Moto_B_Stop();
        return;
    }
}

/**********************************************
* 停止电机
**********************************************/
void Wind_Moto_A_Stop(void)
{
    TIM_SetCompare1(TIM1,0);
    TIM_SetCompare2(TIM1,0);
}
void Wind_Moto_B_Stop(void)
{
    TIM_SetCompare3(TIM8,0);
    TIM_SetCompare4(TIM8,0);
}

/**********************************************
* 关闭电机控制
**********************************************/
void Wind_Moto_OFF(void)
{
    Flag_Wind_Moto = OFF;
    Wind_Moto_A_Stop();
    Wind_Moto_B_Stop();
}

/**********************************************
* 开启电机控制
**********************************************/
void Wind_Moto_ON(void)
{
    Flag_Wind_Moto = ON;
}

/**********************************************
* 电机控制状态改变
**********************************************/
void Wind_Moto_Switch(void)
{
    if (Flag_Wind_Moto == ON)
    {
        Wind_Moto_OFF();
    }
    else
    {
        Wind_Moto_ON();
    }
}

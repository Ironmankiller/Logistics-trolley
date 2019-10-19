/*
 * @Author: spy
 * @Date: 2019-07-27 17:13:35 
 * @Last Modified by:   spy
 * @Last Modified time: 2019-07-27 17:33:25 
 */


#include "stm32f4xx.h"
#include "Moto_Drive.h"
#include "APP\Math\My_Math.h"
#include "sys.h"
#include "My_Flag.h"

#define AIN1   PCout(8)
#define AIN2   PCout(9)
#define PWMA   TIM1->CCR1  //PA8    对    应Encode(8)

#define BIN1   PBout(14)
#define BIN2   PBout(15)
#define PWMB   TIM1->CCR2  //PE11   对应Encode(5)

#define CIN1   PBout(0)
#define CIN2   PBout(1)
#define PWMC   TIM1->CCR3  //PE13   对应Encode(3)

#define DIN1   PBout(8)
#define DIN2   PBout(9)
#define PWMD   TIM1->CCR4  //PA11   对应Encode(4)

#define OFF  MY_FALSE
#define ON   MY_TRUE

#define dead    0;

Flag Flag_Moto = ON;

/**********************************************
* 电机初始化
**********************************************/
void MOTO_A_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* 电机方向控制IO初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); 
	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1); //复用为定时器1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化PA8


    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}

void MOTO_B_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* 电机方向控制IO初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 
	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1); //复用为定时器1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //初始化PE11

    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC2
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR2上的预装载寄存器
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}

void MOTO_C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* 电机方向控制IO初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 
	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1); //复用为定时器1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //初始化PE13


    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC3
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR3上的预装载寄存器
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}



void MOTO_D_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* 电机方向控制IO初始化 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM初始化 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); 
	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1); //复用为定时器1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);              //初始化PA11

    //初始化TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC4
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //使能TIM1在CCR4上的预装载寄存器
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //使能TIM1
}


/**********************************************
* 设置PWM
**********************************************/
 void MOTO_A_Set(int32_t pwm)
{
    if (Flag_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        pwm += dead;
        AIN2 = 1;
        AIN1 = 0;
    }
    else if (pwm < 0)
    {
        pwm -= dead;
        AIN2 = 0;
        AIN1 = 1;
    }
    else
    {
        MOTO_A_Stop();
        return;
    }


    PWMA = my_abs(pwm);
}

void MOTO_B_Set(int32_t pwm)
{
    if (Flag_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        pwm += dead;
        BIN2 = 1;
        BIN1 = 0;
    }
    else if (pwm < 0)
    {
        pwm -= dead;
        BIN2 = 0;
        BIN1 = 1;
    }
    else
    {
        MOTO_B_Stop();
        return;
    }
    
    PWMB = my_abs(pwm);
}

void MOTO_C_Set(int32_t pwm)
{
    if (Flag_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        pwm += dead;
        CIN2 = 1;
        CIN1 = 0;
    }
    else if (pwm < 0)
    {
        pwm -= dead;
        CIN2 = 0;
        CIN1 = 1;
    }
    else
    {
        MOTO_C_Stop();
        return;
    }
    
    PWMC = my_abs(pwm);
}

void MOTO_D_Set(int32_t pwm)
{
    if (Flag_Moto == OFF)
    {
        return;
    }

    if (pwm > 0)
    {
        pwm += dead;
        DIN2 = 1;
        DIN1 = 0;
    }
    else if (pwm < 0)
    {
        pwm -= dead;
        DIN2 = 0;
        DIN1 = 1;
    }
    else
    {
        MOTO_D_Stop();
        return;
    }
    
    PWMD = my_abs(pwm);
}
/**********************************************
* 停止电机
**********************************************/
void MOTO_A_Stop(void)
{
    AIN1 = 0;
    AIN2 = 0;
    PWMA = my_abs(0);
}
void MOTO_B_Stop(void)
{
    BIN1 = 0;
    BIN2 = 0;
    PWMB = my_abs(0);
}
void MOTO_C_Stop(void)
{
    CIN1 = 0;
    CIN2 = 0;
    PWMC = my_abs(0);
}
void MOTO_D_Stop(void)
{
    DIN1 = 0;
    DIN2 = 0;
    PWMD = my_abs(0);
}

/**********************************************
* 关闭电机控制
**********************************************/
void Moto_OFF(void)
{
    Flag_Moto = OFF;
    MOTO_A_Stop();
    MOTO_B_Stop();
	MOTO_C_Stop();
	MOTO_D_Stop();
}

/**********************************************
* 开启电机控制
**********************************************/
void Moto_ON(void)
{
    Flag_Moto = ON;
}

/**********************************************
* 电机控制状态改变
**********************************************/
void Moto_Switch(void)
{
    if (Flag_Moto == ON)
    {
        Moto_OFF();
    }
    else
    {
        Moto_ON();
    }
}

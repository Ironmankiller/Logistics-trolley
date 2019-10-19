/*
 * @Author: spy 
 * @Date: 2019-08-05 16:21:42 
 * @Last Modified by: spy
 * @Last Modified time: 2019-08-05 19:53:03
 */
#include "Servo_Moto.h"


void Servo_MOTO_A_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM初始化 */
    Servo_MOTO_A_TIM_CLK_INIT(Servo_MOTO_A_TIM_CLK, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(Servo_MOTO_A_GPIO_CLK , ENABLE); 
	
    GPIO_PinAFConfig(Servo_MOTO_A_GPIO_PORT, Servo_MOTO_A_GPIO_PINSOURCE, Servo_MOTO_A_TIM_AF); //复用为定时器8

    GPIO_InitStructure.GPIO_Pin = Servo_MOTO_A_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;     //开漏复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(Servo_MOTO_A_GPIO_PORT, &GPIO_InitStructure);              //初始化PC6


    //初始化TIM8
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 200-1; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(Servo_MOTO_A_TIM, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM8 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    Servo_MOTO_A_TIM_OCInit(Servo_MOTO_A_TIM, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM8 OC1
    Servo_MOTO_A_TIM_OCPreloadConfig(Servo_MOTO_A_TIM, TIM_OCPreload_Enable);  //使能TIM8在CCR1上的装载寄存器
    
    TIM_CtrlPWMOutputs(Servo_MOTO_A_TIM,ENABLE); 
    TIM_Cmd(Servo_MOTO_A_TIM, ENABLE);  //使能TIM8
}

void Servo_MOTO_B_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM初始化 */
    Servo_MOTO_B_TIM_CLK_INIT(Servo_MOTO_B_TIM_CLK, ENABLE);   //电机时钟
    RCC_AHB1PeriphClockCmd(Servo_MOTO_B_GPIO_CLK , ENABLE); 
	
    GPIO_PinAFConfig(Servo_MOTO_B_GPIO_PORT, Servo_MOTO_B_GPIO_PINSOURCE, Servo_MOTO_B_TIM_AF); //复用为定时器8

    GPIO_InitStructure.GPIO_Pin = Servo_MOTO_B_GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;      //开漏复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(Servo_MOTO_B_GPIO_PORT, &GPIO_InitStructure);              //初始化PC7

    //初始化TIM8
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler = 200-1; //设置用来作为TIMx时钟频率除数的预分频值 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
    TIM_TimeBaseInit(Servo_MOTO_B_TIM, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    //初始化TIM3 Channel2 PWM模式	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高

    Servo_MOTO_B_TIM_OCInit(Servo_MOTO_B_TIM, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 OC1
    Servo_MOTO_B_TIM_OCPreloadConfig(Servo_MOTO_B_TIM, TIM_OCPreload_Enable);  //使能TIM1在CCR1上的预装载寄存器
    
    TIM_CtrlPWMOutputs(Servo_MOTO_B_TIM,ENABLE); 
    TIM_Cmd(Servo_MOTO_B_TIM, ENABLE);  //使能TIM1
}

void Servo_MOTO_A_Set(int pwm)
{
    Servo_MOTO_A_PWM = pwm;
}

void Servo_MOTO_B_Set(int pwm)
{
    Servo_MOTO_B_PWM = pwm;
}

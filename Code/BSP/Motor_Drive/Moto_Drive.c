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
#define PWMA   TIM1->CCR1  //PA8    ��    ӦEncode(8)

#define BIN1   PBout(14)
#define BIN2   PBout(15)
#define PWMB   TIM1->CCR2  //PE11   ��ӦEncode(5)

#define CIN1   PBout(0)
#define CIN2   PBout(1)
#define PWMC   TIM1->CCR3  //PE13   ��ӦEncode(3)

#define DIN1   PBout(8)
#define DIN2   PBout(9)
#define PWMD   TIM1->CCR4  //PA11   ��ӦEncode(4)

#define OFF  MY_FALSE
#define ON   MY_TRUE

#define dead    0;

Flag Flag_Moto = ON;

/**********************************************
* �����ʼ��
**********************************************/
void MOTO_A_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* ����������IO��ʼ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //���ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); 
	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1); //����Ϊ��ʱ��1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);              //��ʼ��PA8


    //��ʼ��TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM3 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
}

void MOTO_B_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* ����������IO��ʼ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //���ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 
	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1); //����Ϊ��ʱ��1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //��ʼ��PE11

    //��ʼ��TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM3 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC2
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
}

void MOTO_C_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* ����������IO��ʼ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //���ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 
	
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1); //����Ϊ��ʱ��1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //��ʼ��PE13


    //��ʼ��TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM3 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC3Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC3
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR3�ϵ�Ԥװ�ؼĴ���
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
}



void MOTO_D_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    /* ����������IO��ʼ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //���ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); 
	
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1); //����Ϊ��ʱ��1

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);              //��ʼ��PA11

    //��ʼ��TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM3 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC4Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC4
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���
    
    TIM_CtrlPWMOutputs(TIM1,ENABLE); 
    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
}


/**********************************************
* ����PWM
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
* ֹͣ���
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
* �رյ������
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
* �����������
**********************************************/
void Moto_ON(void)
{
    Flag_Moto = ON;
}

/**********************************************
* �������״̬�ı�
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

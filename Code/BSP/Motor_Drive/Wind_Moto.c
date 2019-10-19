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
* �����ʼ��
**********************************************/
void Wind_Moto_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);   //���A1A2
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  //���B1B2
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE); 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE); 

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8); //PC8 9����Ϊ��ʱ��8 CH3 CH4
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM8);
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);//PE9 11����Ϊ��ʱ��1 CH1 CH2
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOC, &GPIO_InitStructure);              //��ʼ��PC9 8
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
    GPIO_Init(GPIOE, &GPIO_InitStructure);


    //��ʼ��TIM1
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM1 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_OC2Init(TIM1, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC2
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR2�ϵ�Ԥװ�ؼĴ���
    TIM_CtrlPWMOutputs(TIM1,ENABLE);

    TIM_Cmd(TIM1, ENABLE);  //ʹ��TIM1
    
    //��ʼ��TIM8
    TIM_TimeBaseStructure.TIM_Period = 16800-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM8 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;

    TIM_OC3Init(TIM8, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM8 OC4
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);  //ʹ��TIM8��CCR3�ϵ�Ԥװ�ؼĴ���

    TIM_OC4Init(TIM8, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM8 OC4
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);  //ʹ��TIM8��CCR4�ϵ�Ԥװ�ؼĴ���
    TIM_CtrlPWMOutputs(TIM8,ENABLE);
    TIM_Cmd(TIM8, ENABLE);  //ʹ��TIM8
}


/**********************************************
* ����PWM
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
* ֹͣ���
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
* �رյ������
**********************************************/
void Wind_Moto_OFF(void)
{
    Flag_Wind_Moto = OFF;
    Wind_Moto_A_Stop();
    Wind_Moto_B_Stop();
}

/**********************************************
* �����������
**********************************************/
void Wind_Moto_ON(void)
{
    Flag_Wind_Moto = ON;
}

/**********************************************
* �������״̬�ı�
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

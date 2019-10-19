#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "BEEP.h"
#define BEEP   TIM9->CCR1  //PB15
/**********************************************
* ��ʼ��������
**********************************************/
void Beep_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    /* PWM��ʼ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 

    GPIO_PinAFConfig(GPIOE, GPIO_PinSource5, GPIO_AF_TIM9);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOE, &GPIO_InitStructure);              //��ʼ��PE5

    //��ʼ��TIM9
    TIM_TimeBaseStructure.TIM_Period = 720-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler = 64; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM9, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    //��ʼ��TIM9 Channel2 PWMģʽ	 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�

    TIM_OC1Init(TIM9, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 OC4
    TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  //ʹ��TIM1��CCR4�ϵ�Ԥװ�ؼĴ���
    
    TIM_CtrlPWMOutputs(TIM9,ENABLE); 
    TIM_Cmd(TIM9, ENABLE);  //ʹ��TIM1
}

/**********************************************
* ��������
**********************************************/
void Beep_On(void)
{
	//TIM_SetCompare4(TIM1, (TIM1->ARR)/2);
    BEEP = 500;
}

/**********************************************
* ��������
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





#include "Step_Moto.h"
#include "stm32f4xx.h"
#include "My_Flag.h"
#include "BSP\Time\Time.h"
#include "BSP\MCU\UART\usart3.h"
/**************************************************************************
�������ܣ������������
������飺��ʼ��TIM2��ͨ��1Ϊ������ģʽ��TIM3Ϊ�ſ�ģʽ�������PWM()��GPIOC Pin_5Ϊ�����źţ�GPIOC Pin_5Ϊ�ѻ��ź�
�������룺TIM2per��װֵ,Compare1�Ƚϲ���1��Ԥװ��ֵ
**************************************************************************/

void StepMoto_Stop()
{
    
    StepMoto_PWM(0,0);
}

void StepMoto_Offline()
{
    GPIO_SetBits(GPIOA,GPIO_Pin_4);
}

void StepMoto_Online()
{
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);
}

void StepMoto_Init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_OCInitTypeDef TIM_OCInitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;
  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��ʱ��

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOA, &GPIO_InitStruct);  //��ʼ��GPIO
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);          //���������ʼ״̬Ϊ�ѻ�״̬
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//ʹ�ܶ�ʱ��2��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//ʹ�ܶ�ʱ��3��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//ʹ��GPIOAʱ��
  
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;//PA0
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
  
    //TIM2�����ڵ�������
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//û��ʱ�ӷָ�
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
    TIM_TimeBaseInitStruct.TIM_Prescaler = 8400;//Ԥ��Ƶֵ,ÿ100us����һ��
    TIM_TimeBaseInitStruct.TIM_Period = 0;//��װֵ
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
  
    TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);//����TIM2�ڵ�����ģʽ�����ǵ�һ�����壬����һ�������¼�ֹͣ
    TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);//ʹ�ܶ�ʱ��2��ͨ��1Ԥװ�ؼĴ���
    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref);
  
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//�����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1���ʹ��
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽΪ��
    TIM_OCInitStruct.TIM_Pulse = 1;//�Ƚϲ���1��Ԥװ��ֵ
    TIM_OC1Init(TIM2,&TIM_OCInitStruct);
  
    TIM_Cmd(TIM2,DISABLE);//�Ȳ�ʹ����TIM2
  
  
    //TIM3�����ڴ�ģʽ���ſ�ģʽ�µ�PWM���ģʽ
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//û��ʱ�ӷָ�
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
    TIM_TimeBaseInitStruct.TIM_Prescaler = 840;//Ԥ��Ƶֵ��10us����һ��
    TIM_TimeBaseInitStruct.TIM_Period = 0;//��װֵ
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
  
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3Ϊ�ſ�ģʽ
    TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);//ʹ��TIM3������ģʽ
    TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//�ڲ���������TIM2����
   
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//�����ϼ���ʱ��һ��TIMx_CNT<TIMx_CCR1ʱͨ��1Ϊ��Ч��ƽ������Ϊ��Ч��ƽ
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1���ʹ��
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//��Ч��ƽΪ��
    TIM_OCInitStruct.TIM_Pulse = 0;//�Ƚϲ���1��Ԥװ��ֵ
    TIM_OC1Init(TIM3,&TIM_OCInitStruct);
  
    TIM_Cmd(TIM3,ENABLE);//ʹ��TIM3
}

/**************************************************************************
�������ܣ�����PWM�����
������飺���Ƶ���������ɵ���PWM
�������룺Cycle:Ϊ����,��λ(us),Pulse_Num:Ϊ�������
**************************************************************************/
void StepMoto_PWM(u16 Cycle, int32_t Pulse_Num)
{    
    static int Start_Time;
    static int End_Time;
    static Flag Flag_Finish = MY_TRUE;
    static int In_Index = 0;
    static int Out_Index = 0;
    static int FIFO_Array_C[30];
    static int FIFO_Array_P[30];
    u16 TIM3per = 0;
    u32 Time = 0;
    if(systime_ms()-Start_Time > End_Time)
    {

        Flag_Finish = MY_TRUE;
    } 
    FIFO_Array_C[In_Index] = Cycle;
    FIFO_Array_P[In_Index++] = Pulse_Num;
    if(In_Index == 30)
        In_Index = 0;
    if(Flag_Finish == MY_TRUE)
    {
        Start_Time = systime_ms();
        End_Time = FIFO_Array_C[Out_Index]*FIFO_Array_P[Out_Index]/1000;

        if(Out_Index == 30)
            Out_Index = 0;
        Flag_Finish = MY_FALSE;
        //�ı�TIM3����װֵ�ı�����Ƶ��������������ռ�ձ�Ϊ50%
        //�ı�TIM2��Ԥ��Ƶ������װֵ�ı��������
        if(FIFO_Array_P[Out_Index]<0){
            FIFO_Array_P[Out_Index] = -FIFO_Array_P[Out_Index];
            GPIO_SetBits(GPIOA,GPIO_Pin_5);
        } else {
            GPIO_ResetBits(GPIOA,GPIO_Pin_5);
        }

        Time = FIFO_Array_C[Out_Index]*FIFO_Array_P[Out_Index];
        Time /= 100;              //Ԥ��ƵΪ7200,100us����һ��
        TIM3per = FIFO_Array_C[Out_Index]/10;       //Ԥ��ƵΪ720,10us����һ��

        TIM_SetAutoreload(TIM2, Time+1);//����TIM2����װֵ
        TIM_SetAutoreload(TIM3, TIM3per-1);//����TIM3����װֵ
        TIM_SetCompare1(TIM3,TIM3per/2);//����ռ�ձ�Ϊ50%
        TIM_Cmd(TIM2,ENABLE);//ʹ��TIM2
        Out_Index++;
    }
    
}



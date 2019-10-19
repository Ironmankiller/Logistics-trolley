#include "Step_Moto.h"
#include "stm32f4xx.h"
#include "My_Flag.h"
#include "BSP\Time\Time.h"
#include "BSP\MCU\UART\usart3.h"
/**************************************************************************
函数功能：步进电机驱动
函数简介：初始化TIM2的通道1为单脉冲模式，TIM3为门控模式用来输出PWM()，GPIOC Pin_5为方向信号，GPIOC Pin_5为脱机信号
函数输入：TIM2per重装值,Compare1比较捕获1的预装载值
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
  
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能时钟

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStruct);  //初始化GPIO
    GPIO_ResetBits(GPIOA,GPIO_Pin_4);          //步进电机初始状态为脱机状态
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能定时器2的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);//使能定时器3的时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);//使能GPIOA时钟
  
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM2);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;//PA0
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
  
    //TIM2工作在单脉冲下
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//没有时钟分割
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
    TIM_TimeBaseInitStruct.TIM_Prescaler = 8400;//预分频值,每100us计数一次
    TIM_TimeBaseInitStruct.TIM_Period = 0;//重装值
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);
  
    TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);//设置TIM2在单脉冲模式，且是单一的脉冲，在下一个更新事件停止
    TIM_OC1PreloadConfig(TIM2,TIM_OCPreload_Enable);//使能定时器2的通道1预装载寄存器
    TIM_SelectOutputTrigger(TIM2,TIM_TRGOSource_OC1Ref);
  
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
    TIM_OCInitStruct.TIM_Pulse = 1;//比较捕获1的预装载值
    TIM_OC1Init(TIM2,&TIM_OCInitStruct);
  
    TIM_Cmd(TIM2,DISABLE);//先不使能能TIM2
  
  
    //TIM3工作在从模式的门控模式下的PWM输出模式
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//没有时钟分割
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
    TIM_TimeBaseInitStruct.TIM_Prescaler = 840;//预分频值，10us计数一次
    TIM_TimeBaseInitStruct.TIM_Period = 0;//重装值
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
  
    TIM_SelectSlaveMode(TIM3, TIM_SlaveMode_Gated);//TIM3为门控模式
    TIM_SelectMasterSlaveMode(TIM3,TIM_MasterSlaveMode_Enable);//使能TIM3的主从模式
    TIM_SelectInputTrigger(TIM3,TIM_TS_ITR1);//内部触发，从TIM2触发
   
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM2;//在向上计数时，一旦TIMx_CNT<TIMx_CCR1时通道1为无效电平，否则为有效电平
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//OC1输出使能
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高
    TIM_OCInitStruct.TIM_Pulse = 0;//比较捕获1的预装载值
    TIM_OC1Init(TIM3,&TIM_OCInitStruct);
  
    TIM_Cmd(TIM3,ENABLE);//使能TIM3
}

/**************************************************************************
函数功能：设置PWM的输出
函数简介：输出频率脉冲数可调的PWM
函数输入：Cycle:为周期,单位(us),Pulse_Num:为脉冲个数
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
        //改变TIM3的重装值改变脉冲频率这里设置脉冲占空比为50%
        //改变TIM2的预分频器和重装值改变脉冲个数
        if(FIFO_Array_P[Out_Index]<0){
            FIFO_Array_P[Out_Index] = -FIFO_Array_P[Out_Index];
            GPIO_SetBits(GPIOA,GPIO_Pin_5);
        } else {
            GPIO_ResetBits(GPIOA,GPIO_Pin_5);
        }

        Time = FIFO_Array_C[Out_Index]*FIFO_Array_P[Out_Index];
        Time /= 100;              //预分频为7200,100us计数一次
        TIM3per = FIFO_Array_C[Out_Index]/10;       //预分频为720,10us计数一次

        TIM_SetAutoreload(TIM2, Time+1);//设置TIM2的重装值
        TIM_SetAutoreload(TIM3, TIM3per-1);//设置TIM3的重装值
        TIM_SetCompare1(TIM3,TIM3per/2);//设置占空比为50%
        TIM_Cmd(TIM2,ENABLE);//使能TIM2
        Out_Index++;
    }
    
}



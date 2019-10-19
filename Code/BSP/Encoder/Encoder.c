#include "Encoder.h"
#include "stm32f4xx.h"
#include "BSP\Time\Time.h"

/**************************************************************************
函数功能：把TIM3初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM3(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  	//TIM3时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3); //GPIOA6复用为定时器3
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3); //GPIOA7复用为定时器3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;           //GPIOA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz

    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}
/**************************************************************************
函数功能：把TIM4初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM4(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);  	//TIM4时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTB时钟	

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_TIM4); //GPIOB6复用为定时器4
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_TIM4); //GPIOB7复用为定时器4

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;           //GPIOB
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz

    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}
/**************************************************************************
函数功能：把TIM5初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM5(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);  	//TIM5时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTA时钟	

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_TIM5); //GPIOA0复用为定时器5
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5); //GPIOA1复用为定时器5

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;           //GPIOA
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz

    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM5, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM5, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
    //Reset counter
    TIM_SetCounter(TIM5, 0);
    TIM_Cmd(TIM5, ENABLE);
}
/**************************************************************************
函数功能：把TIM8初始化为编码器接口模式
入口参数：无
返回  值：无
**************************************************************************/
void Encoder_Init_TIM8(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);  	//TIM8时钟使能    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//使能PORTC时钟	

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8); //GPIOC6复用为定时器8
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8); //GPIOC7复用为定时器8

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;           //GPIOC7 GPIOC6
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;        //
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // 预分频器 
    TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;////TIM向上计数  
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
    TIM_EncoderInterfaceConfig(TIM8, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_ICFilter = 10;
    TIM_ICInit(TIM8, &TIM_ICInitStructure);
    TIM_ClearFlag(TIM8, TIM_FLAG_Update);//清除TIM的更新标志位
    TIM_ITConfig(TIM8, TIM_IT_Update, ENABLE);

    //Reset counter
    TIM_SetCounter(TIM8, 0);
    TIM_Cmd(TIM8, ENABLE);
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
int32_t Read_Encoder(uint8_t TIMX)
{
    int32_t Encoder_TIM;
    switch (TIMX)
    {
    case 3:
        Encoder_TIM = TIM_GetCounter(TIM3);
		TIM3->CNT = 0;
        break;
    case 4:
        Encoder_TIM = TIM_GetCounter(TIM4);
		TIM4->CNT = 0;
        break;
    case 5:
        Encoder_TIM = TIM_GetCounter(TIM5);
		TIM5->CNT = 0;
        break;
    case 8:
        Encoder_TIM = TIM_GetCounter(TIM8);
		TIM8->CNT = 0;
        break;
    default:
        Encoder_TIM = 0;
    }
    if(Encoder_TIM>ENCODER_TIM_PERIOD/2)
        Encoder_TIM-=ENCODER_TIM_PERIOD;
    return Encoder_TIM;
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：角度值
**************************************************************************/
float Read_Encoder_Angle(uint8_t TIMX, int32_t pulse)
{
    int32_t Encoder_TIM;
    float err=0.0f;
    switch (TIMX)
    {
    case 3:
        Encoder_TIM = TIM_GetCounter(TIM3);
		pulse = TIM3->ARR + 1;
        break;
    case 4:
        Encoder_TIM = TIM_GetCounter(TIM4);
		pulse = TIM4->ARR + 1;
        break;
    case 5:
        Encoder_TIM = TIM_GetCounter(TIM5);
		pulse = TIM5->ARR + 1;
        break;
    case 8:
        Encoder_TIM = TIM_GetCounter(TIM8);
		pulse = TIM8->ARR + 1;
        break;
    default:
        Encoder_TIM = 0;
    }
    
    if(Encoder_TIM>ENCODER_TIM_PERIOD/2)
        Encoder_TIM-=ENCODER_TIM_PERIOD;
    return Encoder_TIM*360.0f/(float)pulse + err;
}

/**************************************************************************
函数功能：单位时间读取编码器计数
入口参数：定时器
返回  值：速度值
**************************************************************************/
float Read_Encoder_Speed(uint8_t TIMX, int32_t pulse)
{
    float cycle_time = Get_Cycle_T(1);
    int32_t Encoder_TIM;
    switch (TIMX)
    {
    case 3:
        Encoder_TIM = TIM_GetCounter(TIM3);
		pulse = TIM3->ARR + 1;
		TIM3->CNT = 0;
        break;
    case 4:
        Encoder_TIM = TIM_GetCounter(TIM4);
		pulse = TIM4->ARR + 1;
		TIM4->CNT = 0;
        break;
    case 5:
        Encoder_TIM = TIM_GetCounter(TIM5);
		pulse = TIM5->ARR + 1;
		TIM5->CNT = 0;
        break;
    case 8:
        Encoder_TIM = TIM_GetCounter(TIM8);
		pulse = TIM8->ARR + 1;
		TIM8->CNT = 0;
        break;
    default:
        Encoder_TIM = 0;
    }
    if(Encoder_TIM>ENCODER_TIM_PERIOD/2)
        Encoder_TIM-=ENCODER_TIM_PERIOD;
    return ((float)(Encoder_TIM))*360.0f/ (float)pulse /cycle_time;
}
///**************************************************************************
//函数功能：TIM3中断服务函数
//入口参数：无
//返回  值：无
//**************************************************************************/
//void TIM3_IRQHandler(void)
//{
//	if (TIM3->SR & 0X0001)//溢出中断
//	{
//	}
//	TIM3->SR &= ~(1 << 0);//清除中断标志位 	    
//}
///**************************************************************************
//函数功能：TIM4中断服务函数
//入口参数：无
//返回  值：无
//**************************************************************************/
//void TIM4_IRQHandler(void)
//{
//	if (TIM4->SR & 0X0001)//溢出中断
//	{
//	}
//	TIM4->SR &= ~(1 << 0);//清除中断标志位 	    
//}
///**************************************************************************
//函数功能：TIM5中断服务函数
//入口参数：无
//返回  值：无
//**************************************************************************/
//void TIM5_IRQHandler(void)
//{
//	if (TIM5->SR & 0X0001)//溢出中断
//	{
//	}
//	TIM5->SR &= ~(1 << 0);//清除中断标志位 	    
//}
///**************************************************************************
//函数功能：TIM8中断服务函数
//入口参数：无
//返回  值：无
//**************************************************************************/
//void TIM8_IRQHandler(void)
//{
//	if (TIM8->SR & 0X0001)//溢出中断
//	{
//	}
//	TIM8->SR &= ~(1 << 0);//清除中断标志位 	    
//}


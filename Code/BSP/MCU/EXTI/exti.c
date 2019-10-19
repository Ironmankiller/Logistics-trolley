/*
 * @Author: spy 
 * @Date: 2019-08-06 10:56:32 
 * @Last Modified by:   spy 
 * @Last Modified time: 2019-08-06 10:56:32 
 */

#include "exti.h"
#include "BSP\LED\LED.h"
 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源：按键1 */
  NVIC_InitStructure.NVIC_IRQChannel = NRF_INT_EXTI_IRQ;
  /* 配置抢占优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级：1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

 /**
  * @brief  配置 PA0 为线中断口，并设置中断优先级
  * @param  无
  * @retval 无
  */
void EXTI_NRF_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
  
	/*开启按键GPIO口的时钟*/
	RCC_AHB1PeriphClockCmd(NRF_INT_GPIO_CLK ,ENABLE);
  
    /* 使能 SYSCFG 时钟 ，使用GPIO外部中断时必须使能SYSCFG时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
    /* 配置 NVIC */
    NVIC_Configuration();
  
	/* 选择按键1的引脚 */ 
    GPIO_InitStructure.GPIO_Pin = NRF_INT_GPIO_PIN;
    /* 设置引脚为输入模式 */ 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    /* 设置引脚不上拉也不下拉 */
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    /* 使用上面的结构体初始化按键 */
    GPIO_Init(NRF_INT_GPIO_PORT, &GPIO_InitStructure); 

	/* 连接 EXTI 中断源到引脚 */
    SYSCFG_EXTILineConfig(NRF_INT_EXTI_PORTSOURCE,NRF_INT_EXTI_PINSOURCE);

    /* 选择 EXTI 中断源 */
    EXTI_InitStructure.EXTI_Line = NRF_INT_EXTI_LINE;
    /* 中断模式 */
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    /* 下降沿触发 */
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    /* 使能中断/事件线 */
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

}


void NRF_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);//清除标志
        LED_SWITCH(LED_2);
    }
}
/******************* (C) COPYRIGHT 2019 SPY **********END OF FILE***********/

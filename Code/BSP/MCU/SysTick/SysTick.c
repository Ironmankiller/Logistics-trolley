
#include "stm32f4xx.h"
#include "BSP\Time\Time.h"
#include "APP\Task_Loop\Task_Loop.h"

// cycles per microsecond
volatile uint32_t usTicks = 0;
// 滴答定时器计数变量 ,49天后溢出
volatile uint32_t sysTickUptime = 0;

#define TICK_PER_SECOND 1000 
#define TICK_US	(1000000/TICK_PER_SECOND)


/**********************************************
* SysTick初始化
* 滴答定时器
**********************************************/
void SysTick_Init(void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    uint32_t         cnts;

    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (uint32_t)rcc_clocks.HCLK_Frequency / TICK_PER_SECOND;
    cnts = cnts / 8;

    SysTick_Config(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

/**********************************************
* 滴答定时器 中断函数
**********************************************/
void SysTick_Handler(void)
{
    sysTickUptime++;
    Loop_Time_Check();
    System_Time_Running();
}

/**********************************************
* 获取系统时间 单位us
* 两次获取若大于u32/1000(us),则两次差值溢出，不可取
**********************************************/
uint32_t GetSysTime_us(void)
{
    register uint32_t ms;
    u32 value;
    ms = sysTickUptime;
    value = ms * TICK_US + (SysTick->LOAD - SysTick->VAL) * TICK_US / SysTick->LOAD;
    return value;
}

/**********************************************
* 微秒延时 us
**********************************************/
void delay_us(volatile uint32_t nTime)
{
    u32 now;
    now = GetSysTime_us();
    while (GetSysTime_us() - now<nTime);
}

/**********************************************
* 毫秒延时 ms
**********************************************/
void delay_ms(volatile uint32_t nTime)
{
    u16 i;
    for (i = 0; i<nTime; i++)
    {
        delay_us(1000);
    }
}

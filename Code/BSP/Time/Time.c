#include <string.h>
#include <stdint.h>
#include "Time.h"
#include "stm32f4xx.h"


System_Time_TypeDef System_Time;    //系统时间

/**********************************************
* 清空系统时间
**********************************************/
void System_Tiem_Clear(void)
{
    memset(&System_Time, 0, sizeof(System_Time_TypeDef));
}

/**********************************************
* 记录系统时间
**********************************************/
void System_Time_Running(void)
{
    if (System_Time.Milliseconds < 999)
    {
        System_Time.Milliseconds++;
    }
    else
    {
        System_Time.Milliseconds = 0;
        if (System_Time.Seconds<59)
        {
            System_Time.Seconds++;
        }
        else
        {
            System_Time.Seconds = 0;
            if (System_Time.Minutes<59)
            {
                System_Time.Minutes++;
            }
            else
            {
                System_Time.Minutes = 0;
                if (System_Time.Hours<23)
                {
                    System_Time.Hours++;
                }
                else
                {
                    System_Time.Hours = 0;
                }
            }
        }
    }
}


#define GET_TIME_NUM 	(5)		//设置获取时间的数组数量
volatile float Cycle_T[GET_TIME_NUM][3];

enum
{
    NOW = 0,
    OLD,
    NEW,
};

float Get_Cycle_T(uint8_t item)
{
    Cycle_T[item][OLD] = Cycle_T[item][NOW];	//上一次的时间
    Cycle_T[item][NOW] = GetSysTime_us() / 1000000.0f; //本次的时间
    Cycle_T[item][NEW] = ((Cycle_T[item][NOW] - Cycle_T[item][OLD]));//间隔的时间（周期）
    return Cycle_T[item][NEW];
}

void Cycle_Time_Init(void)
{
    uint8_t i;
    for (i = 0; i<GET_TIME_NUM; i++)
    {
        Get_Cycle_T(i);
    }
}

/*获取系统时间(ms)*/
uint32_t systime_ms(void)
{
    uint32_t time;
    time = System_Time.Hours * 3600000 + System_Time.Minutes * 60000 + System_Time.Seconds * 1000 + System_Time.Milliseconds;
    return time;
}

/*获取系统时间(s)*/
uint32_t systime_s(void)
{
    uint32_t time;
    time = System_Time.Hours * 3600 + System_Time.Minutes * 60 + System_Time.Seconds;
    return time;
}











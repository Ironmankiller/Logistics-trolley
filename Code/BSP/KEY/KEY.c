#include "stm32f4xx.h"
#include "sys.h"
#include "KEY.h"
#include "delay.h"
#include "BSP\BEEP\BEEP.h"

#define KEY2 PAin(4)
#define KEY3 PAin(5)
#define KEY4 PEin(3)
#define KEY5 PEin(2)

/**********************************************
* 初始化按键输入
**********************************************/
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;  //KEY2、KEY3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2; //KEY4、KEY5
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，WKUP按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP!!
uint8_t KEY_Scan(void)
{
    //static uint8_t key_count = 0;
    static u8 key_up = 1;   //按键松开标志
    if (key_up && (KEY2 == 0 || KEY3 == 0 || KEY4 == 0 || KEY5 == 0)) //有按键按下
    {
        key_up = 0;
        //Beep_On();
        if (KEY2 == 0) 
            return 2;
        else if (KEY3 == 0)
            return 3;
        else if (KEY4 == 0) 
            return 4;
        else if (KEY5 == 0)
            return 5;
    }
    else if (KEY2 == 1 && KEY3 == 1 && KEY4 == 1 && KEY5 == 1)
    {
        key_up = 1;
        //Beep_Off();
    }

    return 0; //无按键按下
}

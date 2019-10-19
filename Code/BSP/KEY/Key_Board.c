#include "stm32f4xx.h"
#include "Key_Board.h"
#include "My_Flag.h"
#include <stdint.h>

BUTTON_TypeDef Matrix_Keyboard;

#define MATRIX_KEY_GPIO_CLK    RCC_AHB1Periph_GPIOE
#define MATRIX_KEY_PORT        GPIOE

#define KEY1                GPIO_Pin_8    //GPIOB
#define KEY2                GPIO_Pin_9    //GPIOB
#define KEY3                GPIO_Pin_10   //GPIOB
#define KEY4                GPIO_Pin_11   //GPIOB
#define KEY5                GPIO_Pin_12   //GPIOB
#define KEY6                GPIO_Pin_13   //GPIOB
#define KEY7                GPIO_Pin_14   //GPIOB
#define KEY8                GPIO_Pin_15   //GPIOB

#define KEYPAD2_STATUS1    0x11
#define KEYPAD2_STATUS2    0x21
#define KEYPAD2_STATUS3    0x41
#define KEYPAD2_STATUS4    0x81
#define KEYPAD2_STATUS5    0x12
#define KEYPAD2_STATUS6    0x22
#define KEYPAD2_STATUS7    0x42
#define KEYPAD2_STATUS8    0x82
#define KEYPAD2_STATUS9    0x14
#define KEYPAD2_STATUS10   0x24
#define KEYPAD2_STATUS11   0x44
#define KEYPAD2_STATUS12   0x84
#define KEYPAD2_STATUS13   0x18
#define KEYPAD2_STATUS14   0x28
#define KEYPAD2_STATUS15   0x48
#define KEYPAD2_STATUS16   0x88



#define MASK_KEY         0x40 
#define LOCK_KEY        (MASK_KEY-1) 
#define CONV_KEY        (MASK_KEY-2) 
#define NONE_KEY         0 


/**********************************************
* 矩阵键盘初始化
**********************************************/
void Key_Board_Iint(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_8 |
        GPIO_Pin_9 |
        GPIO_Pin_10 |
        GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin =
        GPIO_Pin_12 |
        GPIO_Pin_13 |
        GPIO_Pin_14 |
        GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    //GPIO_SetBits(MATRIX_KEY_PORT, KEY1);
    //GPIO_SetBits(MATRIX_KEY_PORT, KEY2);
    //GPIO_SetBits(MATRIX_KEY_PORT, KEY3);
    //GPIO_SetBits(MATRIX_KEY_PORT, KEY4);
    GPIO_SetBits(MATRIX_KEY_PORT, KEY5);
    GPIO_SetBits(MATRIX_KEY_PORT, KEY6);
    GPIO_SetBits(MATRIX_KEY_PORT, KEY7);
    GPIO_SetBits(MATRIX_KEY_PORT, KEY8);
}


/*
scan matrix  keypad
scan_keyboard
*/
uint8_t scan_keypad2(BUTTON_TypeDef *p)
{
    static u8 keyValue, tmp_keyValue, keyTrigger;
    static u8 keyDelayNum, keyDelayNumValue;
    static u8 keyFlag;
    u8 x = 0, y = 0;
    //矩阵按键考虑低功耗问题，没用行列翻转扫描
    //第一列==================================================================
    GPIO_ResetBits(MATRIX_KEY_PORT, KEY5);//pull down the first col
    y = ((MATRIX_KEY_PORT->IDR >> 8) & 0x0f) ^ 0x0f;
    if (y) x |= y | 0x10;
    GPIO_SetBits(MATRIX_KEY_PORT, KEY5);//release col
                                        //第二列==================================================================
    GPIO_ResetBits(MATRIX_KEY_PORT, KEY6);//pull down the second col
    y = ((MATRIX_KEY_PORT->IDR >> 8) & 0x0f) ^ 0x0f;
    if (y) x |= y | 0x20;
    GPIO_SetBits(MATRIX_KEY_PORT, KEY6);//release col
                                        //第三列==================================================================
    GPIO_ResetBits(MATRIX_KEY_PORT, KEY7);//pull down the third col
    y = ((MATRIX_KEY_PORT->IDR >> 8) & 0x0f) ^ 0x0f;
    if (y) x |= y | 0x40;
    GPIO_SetBits(MATRIX_KEY_PORT, KEY7);//release col
                                        //第四列==================================================================
    GPIO_ResetBits(MATRIX_KEY_PORT, KEY8);//pull down the fourthly col
    y = ((MATRIX_KEY_PORT->IDR >> 8) & 0x0f) ^ 0x0f;
    if (y) x |= y | 0x80;
    GPIO_SetBits(MATRIX_KEY_PORT, KEY8);//release col
    keyValue = x;

    static uint8_t keyboard_delay_switch = 0;
    if (keyValue && !keyboard_delay_switch)     //有按键输入
    {
        keyboard_delay_switch = 1;              //开启延时
        keyDelayNum = 0;                        //延时清零
        keyTrigger = MY_FALSE;                  //不触发按键
        keyFlag = MY_FALSE;
        return NONE_KEY;
    }
    else if (keyboard_delay_switch && keyTrigger == MY_FALSE)
    {
        keyDelayNum++;
        if (keyDelayNum <= 1)                   //延时未完成
        {
            return NONE_KEY;
        }
        keyTrigger = MY_TRUE;                   //延时完成
    }

    if (!keyValue)                              //按键触发完成
    {
        keyboard_delay_switch = 0;
        return NONE_KEY;
    }
    else if (keyValue && keyFlag == MY_TRUE)    //触发完成后长按
    {
        return NONE_KEY;
    }

    //有按键按下
    if (keyValue)
    {
        keyDelayNumValue = 0;
        switch (keyValue)
        {
        case KEYPAD2_STATUS1:
            p->getKeyVal = S1;
            break;
        case KEYPAD2_STATUS2:
            p->getKeyVal = S2;
            break;
        case KEYPAD2_STATUS3:
            p->getKeyVal = S3;
            break;
        case KEYPAD2_STATUS4:
            p->getKeyVal = S4;
            break;
        case KEYPAD2_STATUS5:
            p->getKeyVal = S5;
            break;
        case KEYPAD2_STATUS6:
            p->getKeyVal = S6;
            break;
        case KEYPAD2_STATUS7:
            p->getKeyVal = S7;
            break;
        case KEYPAD2_STATUS8:
            p->getKeyVal = S8;
            break;
        case KEYPAD2_STATUS9:
            p->getKeyVal = S9;
            break;
        case KEYPAD2_STATUS10:
            p->getKeyVal = S10;
            break;
        case KEYPAD2_STATUS11:
            p->getKeyVal = S11;
            break;
        case KEYPAD2_STATUS12:
            p->getKeyVal = S12;
            break;
        case KEYPAD2_STATUS13:
            p->getKeyVal = S13;
            break;
        case KEYPAD2_STATUS14:
            p->getKeyVal = S14;
            break;
        case KEYPAD2_STATUS15:
            p->getKeyVal = S15;
            break;
        case KEYPAD2_STATUS16:
            p->getKeyVal = S16;
            break;
        default:
            p->getKeyVal = NONE_KEY; //没有输入
            break;
        }
        keyFlag = MY_TRUE;
        keyTrigger = MY_FALSE;
        return   p->getKeyVal;
    }

    return   NONE_KEY;

}


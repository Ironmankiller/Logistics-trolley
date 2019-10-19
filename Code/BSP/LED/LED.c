#include "stm32f4xx.h"
#include "sys.h"
#include "LED.h"

#define LED4_ON  PCout(13) = 0;
#define LED4_OFF PCout(13) = 1;

#define LED3_ON  PEout(6) = 0;
#define LED3_OFF PEout(6) = 1;

#define LED2_ON  PEout(1) = 0;
#define LED2_OFF PEout(1) = 1;

#define LED1_ON  PEout(4) = 0;
#define LED1_OFF PEout(4) = 1;

/* ָʾ�Ƶ�״̬ */
static uint8_t LED_Statue[4] = { 0 };  

/**********************************************
* ��ʼ��LED
**********************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ��ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_1 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);  //��ʼ��GPIO
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  

}

/**********************************************
* ����
**********************************************/
void LED_ON(enum LED_Num led)
{
	switch (led)
	{
	case LED_1:LED1_ON; break;
	case LED_2:LED2_ON; break;
	case LED_3:LED3_ON; break;
	case LED_4:LED4_ON; break;
	default:
		break;
	}
    LED_Statue[led] = 1;
}

/**********************************************
* �ص�
**********************************************/
void LED_OFF(enum LED_Num led)
{
	switch (led)
	{
	case LED_1:LED1_OFF; break;
	case LED_2:LED2_OFF; break;
	case LED_3:LED3_OFF; break;
	case LED_4:LED4_OFF; break;
	default:
		break;
	}
    LED_Statue[led] = 0;
}

/**********************************************
* �л�
**********************************************/
void LED_SWITCH(enum LED_Num led)
{
        
	switch (led)
	{
	case LED_1:if(LED_Statue[led] == 1) {LED1_OFF;} else {LED1_ON;} break;
	case LED_2:if(LED_Statue[led] == 1) {LED2_OFF;} else {LED2_ON;} break;
	case LED_3:if(LED_Statue[led] == 1) {LED3_OFF;} else {LED3_ON;} break;
	case LED_4:if(LED_Statue[led] == 1) {LED4_OFF;} else {LED4_ON;} break;
	default:
		break;
	}
    LED_Statue[led] = !LED_Statue[led];
}


/**********************************************
* ����ȫ��LED
**********************************************/
void LED_ALL_ON(void)
{
	LED1_ON;
    LED_Statue[LED_1] = 1;
	LED2_ON;
    LED_Statue[LED_2] = 1;
	LED3_ON;
    LED_Statue[LED_3] = 1;
	LED4_ON;
    LED_Statue[LED_4] = 1;

}

/**********************************************
* �ر�ȫ��LED
**********************************************/
void LED_ALL_OFF(void)
{
	LED1_OFF;
    LED_Statue[LED_1] = 0;
	LED2_OFF;
    LED_Statue[LED_2] = 0;
	LED3_OFF;
    LED_Statue[LED_3] = 0;
	LED4_OFF;
    LED_Statue[LED_4] = 0;

}

/**********************************************
* ��˸ĳ��LED 
* ��50ms��ʱ�������
**********************************************/
void LED_Blink(enum LED_Num led)
{
    static uint8_t time_count = 0;
    if (time_count++ > 5)
    {
        if (LED_Statue[led])
        {
            LED_OFF(led);
        }
        else
        {
            LED_ON(led);
        }
        time_count = 0;
    }
}

#ifndef __74HC165_H_
#define __74HC165_H_
#include "sys.h"
#include "stm32f4xx.h"

//����  74HC165���ŵ� IO����
//SHIFT/LOAD (�ߵ�ƽ��λ,�͵�ƽװ��)
#define HC165_SHLD_GPIO_PORT              GPIOD
#define HC165_SHLD_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define HC165_SHLD_GPIO_PIN               GPIO_Pin_8
//CLK
#define HC165_CLK_GPIO_PORT               GPIOE
#define HC165_CLK_GPIO_CLK                RCC_AHB1Periph_GPIOE
#define HC165_CLK_GPIO_PIN                GPIO_Pin_15
//DATA
#define HC165_DATA_GPIO_PORT              GPIOD
#define HC165_DATA_GPIO_CLK               RCC_AHB1Periph_GPIOD
#define HC165_DATA_GPIO_PIN               GPIO_Pin_10

#define HC165_CLK_H                       HC165_CLK_GPIO_PORT->BSRRL = HC165_CLK_GPIO_PIN;
#define HC165_CLK_L                       HC165_CLK_GPIO_PORT->BSRRH = HC165_CLK_GPIO_PIN
#define SHLD_H                            HC165_SHLD_GPIO_PORT->BSRRL = HC165_SHLD_GPIO_PIN;
#define SHLD_L                            HC165_SHLD_GPIO_PORT->BSRRH = HC165_SHLD_GPIO_PIN;
#define DATA_read                         HC165_DATA_GPIO_PORT->IDR  & HC165_DATA_GPIO_PIN

void HC165_Init(void);
void HC165_delay(void);
u8 HC165_Read_Byte(void);
u16 HC165_Read_2Byte(void);
void HC165_Show(void);
#endif /* __74HC165_H_ */


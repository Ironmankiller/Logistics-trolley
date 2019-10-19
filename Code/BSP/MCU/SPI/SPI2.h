#ifndef __SPI2_H
#define __SPI2_H
#include "sys.h"	


/*SPI接口定义-开头****************************/
//#define SPI1                           SPI1
#define SPI2_CLK                       RCC_APB1Periph_SPI2
#define SPI2_CLK_INIT                  RCC_APB1PeriphClockCmd

#define SPI2_SCK_PIN                   GPIO_Pin_10                  
#define SPI2_SCK_GPIO_PORT             GPIOB                 
#define SPI2_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define SPI2_SCK_PINSOURCE             GPIO_PinSource10
#define SPI2_SCK_AF                    GPIO_AF_SPI2

#define SPI2_MISO_PIN                  GPIO_Pin_14              
#define SPI2_MISO_GPIO_PORT            GPIOB             
#define SPI2_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI2_MISO_PINSOURCE            GPIO_PinSource14
#define SPI2_MISO_AF                   GPIO_AF_SPI2

#define SPI2_MOSI_PIN                  GPIO_Pin_15              
#define SPI2_MOSI_GPIO_PORT            GPIOB               
#define SPI2_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define SPI2_MOSI_PINSOURCE            GPIO_PinSource15
#define SPI2_MOSI_AF                   GPIO_AF_SPI2

#define SPI2_CS_PIN                    GPIO_Pin_7             
#define SPI2_CS_GPIO_PORT              GPIOE                
#define SPI2_CS_GPIO_CLK               RCC_AHB1Periph_GPIOE

#define SPI2_CS_LOW()      {SPI2_CS_GPIO_PORT->BSRRH=SPI2_CS_PIN;}
#define SPI2_CS_HIGH()     {SPI2_CS_GPIO_PORT->BSRRL=SPI2_CS_PIN;}
/*SPI接口定义-结尾****************************/

/*等待超时时间*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

extern SPI_InitTypeDef  SPI2_InitStructure;
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif


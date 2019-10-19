#ifndef __SPI1_H
#define __SPI1_H
#include "sys.h"	


/*SPI�ӿڶ���-��ͷ****************************/
//#define SPI1                           SPI1
#define SPI1_CLK                       RCC_APB2Periph_SPI1
#define SPI1_CLK_INIT                  RCC_APB2PeriphClockCmd

#define SPI1_SCK_PIN                   GPIO_Pin_5                  
#define SPI1_SCK_GPIO_PORT             GPIOA                       
#define SPI1_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOA
#define SPI1_SCK_PINSOURCE             GPIO_PinSource5
#define SPI1_SCK_AF                    GPIO_AF_SPI1

#define SPI1_MISO_PIN                  GPIO_Pin_6                
#define SPI1_MISO_GPIO_PORT            GPIOA                   
#define SPI1_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPI1_MISO_PINSOURCE            GPIO_PinSource6
#define SPI1_MISO_AF                   GPIO_AF_SPI1

#define SPI1_MOSI_PIN                  GPIO_Pin_7                
#define SPI1_MOSI_GPIO_PORT            GPIOA                     
#define SPI1_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOA
#define SPI1_MOSI_PINSOURCE            GPIO_PinSource7
#define SPI1_MOSI_AF                   GPIO_AF_SPI1

#define SPI1_CS_PIN                    GPIO_Pin_4               
#define SPI1_CS_GPIO_PORT              GPIOA                     
#define SPI1_CS_GPIO_CLK               RCC_AHB1Periph_GPIOA

#define SPI1_CS_LOW()      {SPI1_CS_GPIO_PORT->BSRRH=SPI1_CS_PIN;}
#define SPI1_CS_HIGH()     {SPI1_CS_GPIO_PORT->BSRRL=SPI1_CS_PIN;}
/*SPI�ӿڶ���-��β****************************/

/*�ȴ���ʱʱ��*/
#define SPIT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define SPIT_LONG_TIMEOUT         ((uint32_t)(10 * SPIT_FLAG_TIMEOUT))

extern SPI_InitTypeDef  SPI1_InitStructure;
void SPI1_Init(void);			 //��ʼ��SPI��
void SPI1_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif


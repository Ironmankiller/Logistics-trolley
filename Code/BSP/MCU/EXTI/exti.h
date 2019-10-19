#ifndef __EXTI_H
#define	__EXTI_H

#include "stm32f4xx.h"

//Òý½Å¶¨Òå
/*******************************************************/
#define NRF_INT_GPIO_PORT                GPIOE
#define NRF_INT_GPIO_CLK                 RCC_AHB1Periph_GPIOE
#define NRF_INT_GPIO_PIN                 GPIO_Pin_15
#define NRF_INT_EXTI_PORTSOURCE          EXTI_PortSourceGPIOE
#define NRF_INT_EXTI_PINSOURCE           EXTI_PinSource15
#define NRF_INT_EXTI_LINE                EXTI_Line15
#define NRF_INT_EXTI_IRQ                 EXTI15_10_IRQn

#define NRF_IRQHandler                   EXTI15_10_IRQHandler

/*******************************************************/


void EXTI_NRF_Config(void);

#endif /* __EXTI_H */

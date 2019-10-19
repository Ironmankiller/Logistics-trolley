#ifndef __SERVO_MOTO_H_
#define __SERVO_MOTO_H_

#include "sys.h"

#define Servo_MOTO_A_GPIO_PORT            GPIOA
#define Servo_MOTO_A_GPIO_Pin             GPIO_Pin_8
#define Servo_MOTO_A_GPIO_PINSOURCE       GPIO_PinSource8
#define Servo_MOTO_A_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define Servo_MOTO_A_TIM                  TIM1
#define Servo_MOTO_A_TIM_CLK_INIT         RCC_APB2PeriphClockCmd
#define Servo_MOTO_A_TIM_CLK              RCC_APB2Periph_TIM1
#define Servo_MOTO_A_TIM_AF               GPIO_AF_TIM1
#define Servo_MOTO_A_TIM_OCInit           TIM_OC1Init
#define Servo_MOTO_A_TIM_OCPreloadConfig  TIM_OC1PreloadConfig   
#define Servo_MOTO_A_PWM                  TIM1->CCR1

#define Servo_MOTO_B_GPIO_PORT            GPIOA
#define Servo_MOTO_B_GPIO_Pin             GPIO_Pin_11
#define Servo_MOTO_B_GPIO_PINSOURCE       GPIO_PinSource11
#define Servo_MOTO_B_GPIO_CLK             RCC_AHB1Periph_GPIOA

#define Servo_MOTO_B_TIM                  TIM1
#define Servo_MOTO_B_TIM_CLK_INIT         RCC_APB2PeriphClockCmd
#define Servo_MOTO_B_TIM_CLK              RCC_APB2Periph_TIM1
#define Servo_MOTO_B_TIM_AF               GPIO_AF_TIM1
#define Servo_MOTO_B_TIM_OCInit           TIM_OC4Init
#define Servo_MOTO_B_TIM_OCPreloadConfig  TIM_OC4PreloadConfig
#define Servo_MOTO_B_PWM                  TIM1->CCR4   

void Servo_MOTO_A_Init(void);
void Servo_MOTO_B_Init(void);
void Servo_MOTO_A_Set(int pwm);
void Servo_MOTO_B_Set(int pwm);

#endif /* __SERVO_MOTO_H_ */


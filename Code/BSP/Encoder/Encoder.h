#ifndef __ENCODER_H
#define __ENCODER_H

#include <stdint.h>
#include "sys.h"

void Encoder_Init_TIM3(void);
void Encoder_Init_TIM4(void);
void Encoder_Init_TIM5(void);
void Encoder_Init_TIM8(void);

int32_t Read_Encoder(uint8_t TIMX);
float Read_Encoder_Speed(uint8_t TIMX, int32_t pulse);
float Read_Encoder_Angle(uint8_t TIMX, int32_t pulse);

#define ENCODER_TIM_PERIOD (u16)(0x7FFFFFFF)
#endif

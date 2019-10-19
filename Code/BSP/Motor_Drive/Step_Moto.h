#ifndef __MOTOR_H
#define __MOTOR_H
#include <stdint.h>
#include "stm32f4xx.h"

void StepMoto_Init(void);
void StepMoto_PWM(u16 Cycle, int32_t Pulse_Num);
void StepMoto_Stop(void);
void StepMoto_Offline(void);
void StepMoto_Online(void);
#endif

#ifndef __MOTO_DRIVE_H
#define __MOTO_DRIVE_H

#include <stdint.h>

/**********************************************
* 电机初始化
**********************************************/
void MOTO_A_Init(void);
void MOTO_B_Init(void);
void MOTO_C_Init(void);
void MOTO_D_Init(void);

/**********************************************
* 设置PWM
**********************************************/
void MOTO_A_Set(int32_t pwm);
void MOTO_B_Set(int32_t pwm);
void MOTO_C_Set(int32_t pwm);
void MOTO_D_Set(int32_t pwm);

/**********************************************
* 停止电机
**********************************************/
void MOTO_A_Stop(void);
void MOTO_B_Stop(void);
void MOTO_C_Stop(void);
void MOTO_D_Stop(void);

/**********************************************
* 关闭电机控制
**********************************************/
void Moto_OFF(void);

/**********************************************
* 关闭电机控制
**********************************************/
void Moto_ON(void);
/**********************************************
* 电机控制状态改变
**********************************************/
void Moto_Switch(void);


#endif

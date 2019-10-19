#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include "stm32f4xx.h"
#include <stdint.h>

/*--------------------------------------------------------
模糊控制
--------------------------------------------------------*/
void fuzzy_ctr_x(void);
void fuzzy_ctr_y(void);

/*--------------------------------------------------------
前馈控制
--------------------------------------------------------*/
void forward_setup_x(float Xtarget);
void forward_setup_y(float Ytarget);
void forward_ctr_x(void);
void forward_ctr_y(void);

/*--------------------------------------------------------
PID控制
--------------------------------------------------------*/
//设置
void pid_setup_speed(void);

void pid_setup_angle(float target);

void pid_setup_y(float Ytarget);

void pid_setup_x(float Xtarget);

void pid_setup_angle_y(float Ytarget);

void pid_setup_angle_x(float Xtarget);

void pid_setup_speed_x(float Xtarget);

void pid_setup_speed_y(float Ytarget);

void pid_setup_mecanum_speed(void);

void pid_setup_mecanum_position(void);

//控制
void pid_ctr_y(void);

void pid_ctr_x(void);

void pid_ctr_angle_y(void);

void pid_ctr_angle_x(void);

void pid_ctr_speed_x(void);

void pid_ctr_speed_y(void);

void pid_ctr_speed(void);

void pid_ctr_angle(void);

void pid_ctr_mecanum_speed(int Encode_A,int Encode_B,int Encode_C,int Encode_D);

void pid_ctr_mecanum_position(int Position_A,int Position_B,int Position_C,int Position_D);






#endif





//-----------------------------------------------------------------------------
//  Created on	: 2017-8-9
//      Author	: ZB
//		version	: V1.0
//		brief	: Pid Forward Fuzzy Control
//-----------------------------------------------------------------------------
#include "Algorithm.h"
#include "forward.h"
#include "APP\Math\My_Math.h"
#include "APP\Control\param.h"
#include "includes.h"
#include <stdint.h>
#include "BSP\Time\Time.h"
#include "pid.h"
#include "BSP\Motor_Drive\Moto_Drive.h"
#include "BSP\Motor_Drive\Wind_Moto.h"
#include "APP\IMU\ahrs.h"
#include "BSP\MCU\UART\usart3.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "APP\Math\My_Math.h"
#include "APP\Control\param.h"
#include "BSP\MPU6050\MPU6050.h"
#include "BSP\Motor_Drive\Steer_Moto.h"
#include "BSP\Mecanum\Mecanum.h"

void pid_setup_mecanum_speed()
{
	update_pid_param(&pid_motorA_speed,pidmotorAspeedparm,Mecanum.Target_A);
	update_pid_param(&pid_motorB_speed,pidmotorBspeedparm,Mecanum.Target_B);
	update_pid_param(&pid_motorC_speed,pidmotorCspeedparm,Mecanum.Target_C);
	update_pid_param(&pid_motorD_speed,pidmotorDspeedparm,Mecanum.Target_D);
}
void pid_setup_mecanum_position()
{

}

void pid_ctr_mecanum_speed(int Encode_A,int Encode_B,int Encode_C,int Encode_D)
{
	Mecanum.Moto_A = pid_inc_calc(&pid_motorA_speed, Encode_A);
	Mecanum.Moto_B = pid_inc_calc(&pid_motorB_speed, Encode_B);
	Mecanum.Moto_C = pid_inc_calc(&pid_motorC_speed, Encode_C);
	Mecanum.Moto_D = pid_inc_calc(&pid_motorD_speed, Encode_D);
	MOTO_A_Set(Mecanum.Moto_A);
	MOTO_B_Set(Mecanum.Moto_B);
	MOTO_C_Set(Mecanum.Moto_C);
	MOTO_D_Set(Mecanum.Moto_D);

}
void pid_ctr_mecanum_position(int Position_A,int Position_B,int Position_C,int Position_D)
{

}
void pid_setup_speed(void)
{

}

void pid_ctr_speed(void)
{

}

void pid_setup_angle(float target)
{
	
}

void pid_ctr_angle(void)
{

}


/*--------------------------------------------------------
                       Y_PID_Angle
   --------------------------------------------------------*/
void pid_setup_angle_y(float Ytarget)
{

}

void pid_ctr_angle_y(void)
{

}

/*--------------------------------------------------------
                       X_PID_Angle
   --------------------------------------------------------*/
void pid_setup_angle_x(float Xtarget)
{

}

void pid_ctr_angle_x(void)
{

}

/*--------------------------------------------------------
                         Y_PID
   --------------------------------------------------------*/
void pid_setup_y(float Ytarget)
{

}

void pid_ctr_y(void)
{

}

/*Fuzzy PID*/
void fuzzy_ctr_y(void)
{

}


/*--------------------------------------------------------
                          X_PID
   --------------------------------------------------------*/
void pid_setup_x(float Xtarget)
{

}



void pid_ctr_x(void)
{


}
/*Fuzzy PID*/
void fuzzy_ctr_x(void)
{

}


/*--------------------------------------------------------
                       Y_PID_Speed
   --------------------------------------------------------*/
void pid_setup_speed_y(float Ytarget)
{

}

void pid_ctr_speed_y(void)
{

}

/*--------------------------------------------------------
                         X_PID_Speed
   --------------------------------------------------------*/
void pid_setup_speed_x(float Xtarget)
{

}

void pid_ctr_speed_x(void)
{

}



/*--------------------------------------------------------
   Y_Forward
   --------------------------------------------------------*/
/*初始化Y直线前馈参数*/
void forward_setup_y(float Ytarget)
{

}

//Y直线前馈控制
void forward_ctr_y(void)
{


}


/*--------------------------------------------------------
   X_Forward
   --------------------------------------------------------*/
/*初始化X直线前馈参数*/
void forward_setup_x(float Xtarget)
{

}

//X直线前馈控制
void forward_ctr_x(void)
{

}
/******************* (C) COPYRIGHT 2017 ZB ***********END OF FILE***********/

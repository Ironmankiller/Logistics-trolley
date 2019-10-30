/*
 * @Author: spy 
 * @Date: 2019-08-01 17:03:29 
 * @Last Modified by: spy
 * @Last Modified time: 2019-08-02 23:04:14
 * 麦克纳姆轮驱动代码
 */
#include "Mecanum.h"
#include "BSP\MPU6050\MPU6050.h"
#include "APP\Control\control.h"

sMecanum_Typedef Mecanum;
u8 rxbuf[8],Usart_ON_Flag=0;
/**********************************************
函数功能：麦轮参数初始化
入口参数：无
返回  值：无
**********************************************/
void Mecanum_Param_Init(void)
{
    memset(&Mecanum, 0, sizeof(sMecanum_Typedef));
    Mecanum.state = ready;
    Mecanum.RC_Velocity = 15.0f;
	Mecanum.Gyro_K=0.0060f;
    Mecanum.Departure_Order = rlc;
    Mecanum.Processing_Place_Order = lrc;
    Mecanum.Processing_Grab_Order = rlc;
    Mecanum.Finish_Order = rlc;
    Init_flag = MY_TRUE;
}

/**********************************************
函数功能：麦轮参数更新
入口参数：编码器读值
返回  值：无
**********************************************/
void Mecanum_Update(int A,int B,int C,int D)
{
    Mecanum.Encoder_A = A * 0.25f + Mecanum.Encoder_A*0.75f;
    Mecanum.Encoder_B = B * 0.25f + Mecanum.Encoder_B*0.75f;
    Mecanum.Encoder_C = C * 0.25f + Mecanum.Encoder_C*0.75f;
    Mecanum.Encoder_D = D * 0.25f + Mecanum.Encoder_D*0.75f;
}

/**************************************************************************
函数功能：麦轮小车运动数学模型
入口参数：X Y Z 三轴速度或者位置
返回  值：无
**************************************************************************/
void Kinematic_Analysis(float Vx,float Vy,float Vz)
{
    Mecanum.Target_A = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER) + Mecanum.Gyro_K*mpu6050.Gyro.z;
    Mecanum.Target_B = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER) + Mecanum.Gyro_K*mpu6050.Gyro.z;
    Mecanum.Target_C = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER) - Mecanum.Gyro_K*mpu6050.Gyro.z;
	Mecanum.Target_D = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER) - Mecanum.Gyro_K*mpu6050.Gyro.z;
//  Mecanum.Target_A = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
//  Mecanum.Target_B = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
//  Mecanum.Target_C = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
//	Mecanum.Target_D = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
}


/**************************************************************************
函数功能：接收CAN或者串口控制指令进行处理
入口参数：模式
返回  值：无
**************************************************************************/
void PS2_Usart_Control(void)
{
	int Yuzhi=5;
	int LX,LY,RX;
	LX=Anolog_L_X-128;
	LY=Anolog_L_Y-128;
	RX=Anolog_R_X-128;
    Mecanum.Gyro_K=0.0045;
	if(LX>-Yuzhi&&LX<Yuzhi)LX=0, Mecanum.Gyro_K=0;
	if(LY>-Yuzhi&&LY<Yuzhi)LY=0, Mecanum.Gyro_K=0;
	if(RX>-Yuzhi&&RX<Yuzhi)RX=0, Mecanum.Gyro_K=0.0045;
	Mecanum.Move_X=LX*Mecanum.RC_Velocity/128.0f;
	Mecanum.Move_Y=-LY*Mecanum.RC_Velocity/128.0f;	
	Mecanum.Move_Z=-RX*Mecanum.RC_Velocity/128.0f;		 
	Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析 
}


/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

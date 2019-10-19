/*
 * @Author: spy 
 * @Date: 2019-08-06 10:55:02 
 * @Last Modified by: spy
 * @Last Modified time: 2019-08-06 17:58:21
 */

#include "Car.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "APP\Math\Vect\Vect.h"
#include "My_Flag.h"
#include "BSP\Time\Time.h"
#include "APP\Math\My_Math.h"
#include "BSP\Motor_Drive\Steer_Moto.h"
#include "BSP\PS2\PS2.h"
#include "BSP\MCU\UART\usart3.h"
/* ���������־λ */

sCar_Typedef Car;     //С����Ϣ

/**************************************************************************
�������ܣ�С���˶���ѧģ��
��ڲ������ٶȺ�ת��
����  ֵ����
**************************************************************************/
void Car_Kinematic_Analysis(float velocity,float angle)
{
    Car.Left_Speed_Target = velocity*(1 + Width*tan(angle*pi/180.0f)/2/Length);
	Car.Right_Speed_Target = velocity*(1 - Width*tan(angle*pi/180.0f)/2/Length);      //���ֲ���
	Car.Steer_Moto = Car.Steer_Moto_INIT + angle * K;                    //���ת��   
}
/**********************************************
* С��������ʼ��
**********************************************/
void Car_Param_Init(void)
{
    memset(&Car, 0, sizeof(sCar_Typedef));
    Car.Mode = PS2;
    Car.Steer_Moto_INIT = 1580;
}

/**********************************************
* ����С����Ϣ
**********************************************/
void Car_Update(float left, float right)
{
    //Car.Left_Speed = left;
    //Car.Right_Speed = right;
    Car.Left_Speed = left*0.85f + Car.Left_Speed*0.15f;
    Car.Right_Speed = right*0.85f + Car.Right_Speed*0.15f;
}

void Get_PS2(void){
    int Yuzhi=3;
    float LY,RX;
    LY=Anolog_L_Y-128;     //����ƫ��
	RX=Anolog_R_X-128;
	if(LY>-Yuzhi&&LY<Yuzhi)LY=0;   //С�Ƕ���Ϊ���� ��ֹ���������쳣
	if(RX>-Yuzhi&&RX<Yuzhi)RX=0;
	Car.velocity=-LY/2.0f;	  //�ٶȺ�ҡ�˵�������ء�
	Car.angle=RX/4.0f; 
}
/******************* (C) COPYRIGHT 2019 spy **********END OF FILE***********/

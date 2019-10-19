#include "Pan_Tilt.h"
#include <string.h>
#include <stdio.h>
#include "BSP\Motor_Drive\Servo_Moto.h"

sPan_Tilt_Typedef Pan_Tilt;

static void Pan_Tilt_XY_Control(float X, float Y);
void Fire_INT(void);
void Charge_INT(void);

void Pan_Tilt_Param_Init(void)
{
    memset(&Pan_Tilt, 0, sizeof(sPan_Tilt_Typedef));
    Fire_INT();                      //控制开火引脚初始化
    Charge_INT();                    //控制充放电引角初始化
    Pan_Tilt.Question = Question_11;
    Pan_Tilt.State = ready;
    Pan_Tilt.X_Angle = 0.0f;
    Pan_Tilt.Y_Angle = 0.0f;
    Pan_Tilt.X_Offset = -111;  //使得云台初始化位置为炮管仰角0度偏航角-30度
    Pan_Tilt.Y_Offset = -446;
    Pan_Tilt.Target_Length_Buchang = 0;
    Pan_Tilt_Reset();
}


//设置炮台目标点
void Pan_Tilt_Set_Target(float Target_Angle, float Target_Length)
{
    Pan_Tilt.Target_Angle = Target_Angle;
    Pan_Tilt.Target_Length = Target_Length;
}

//控制炮台到目标点
void Pan_Tilt_Control(void)
{
    float X,Y;
    Pan_Tilt.Target_Length+=Pan_Tilt.Target_Length_Buchang;
    //在这里做曲线拟合，关于靶心与炮台的间距和Y方向角度的函数关系
    Y = -0.0002817f*Pan_Tilt.Target_Length*Pan_Tilt.Target_Length + 0.2939f*Pan_Tilt.Target_Length - 37.21f;
    X=Pan_Tilt.Target_Angle;

    //输入XY角度控制偏转
    Pan_Tilt_XY_Control(X, Y);
}

//复位电磁炮台
void Pan_Tilt_Reset(void)
{
    Servo_MOTO_A_Set(1260 + Pan_Tilt.X_Offset);     //电磁炮炮口方向复位
    Servo_MOTO_B_Set(1260 + Pan_Tilt.Y_Offset);
    Pan_Tilt_Update(0,0);
}

//调制电磁炮体坐标系零点
void Pan_Tilt_Calibrate(void)
{
    Pan_Tilt.X_Offset += Pan_Tilt.X_Angle/270.0f*1680.0f;     //设置当前角度为电磁炮体坐标系零点
    Pan_Tilt.Y_Offset += Pan_Tilt.Y_Angle/270.0f*1680.0f;
    Pan_Tilt.X_Angle = 0;
    Pan_Tilt.Y_Angle = 0;
}

//更新当前角度
void Pan_Tilt_Update(float x, float y)
{
    Pan_Tilt.X_Angle = x;
    Pan_Tilt.Y_Angle = y;
}

//炮台XY方向控制
static void Pan_Tilt_XY_Control(float X, float Y)
{
    if(Y>40) Y=40;
    if(Y<0) Y=0;
    if(X != Pan_Tilt.X_Angle){                                 //判断是否到达目标角度
        Servo_MOTO_A_Set(X/270.0f*1680.0f + Pan_Tilt.X_Offset + 1260);                  //控制转向
        Pan_Tilt_Update(Pan_Tilt.X_Angle+X, Pan_Tilt.Y_Angle);      //更新当前角度
    }
    if(Y != Pan_Tilt.Y_Angle){
        Servo_MOTO_B_Set(Y/270.0f*1680.0f + Pan_Tilt.Y_Offset + 1260);
        Pan_Tilt_Update(Pan_Tilt.X_Angle, Pan_Tilt.Y_Angle+Y);
    }
}

void Pan_Tilt_Control_plus(void){
    float X,Y;
    //在这里做曲线拟合，关于靶心与炮台的间距和Y方向角度的函数关系
    Y = 0.2*Pan_Tilt.Target_Length -20;
    X=Pan_Tilt.Target_Angle;
    //输入XY角度控制偏转
    Pan_Tilt_XY_Control(X, Y);
}

//充电控制引脚初始化
//_CHARGE_HIGH
//_CHAGER_LOW
void Charge_INT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);  //初始化GPIO 

    PBout(1) = 0;
}

//开火控制引脚初始化 
//_FIRE_HIGH
//_FIRE_LOW
void Fire_INT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//使能时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);  //初始化GPIO 

    PEout(7) = 0;
}

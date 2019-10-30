#ifndef _MECANUM_H
#define _MECANUM_H
#include "sys.h"
#include <string.h>
#include <stdio.h>
#include "BSP\SYSTEM\My_Flag.h"
#include "APP\Control\param.h"
#include "BSP\PS2\PS2.h"




#define a_PARAMETER  (0.171f) //x方向
#define b_PARAMETER  (0.191f) //y方向

enum Run_State
{
    ready,
    goToDeparture,
    grabFromDeparture,
    goToProcessing,
    placeToProcessing,
    backToDeparture,
    grabFromProcessing,
    goToFinish,
    placeToFinish,
    backToProcessing,
    back
};

enum Order_Set
{
    lcr,
    lrc,
    rlc,
    rcl,
    clr,
    crl
};

// 小车麦轮编号
//     头
//   B—^—C
//   |   |
//   A—^—D
//    尾

typedef struct
{
    float side_AB;
    float side_BC;
    float side_CD;
    float side_DA;
    
    enum Order_Set Finish_Order;
    enum Order_Set Processing_Grab_Order;
    enum Order_Set Processing_Place_Order;
    enum Order_Set Departure_Order;
    
    u16 Y_Length;
    u16 X_Length;

    /* 编码器值 */
    long int Encoder_A;
    long int Encoder_B;
    long int Encoder_C;
    long int Encoder_D;

    /* 电机目标值 */
    long int Target_A;
    long int Target_B;
    long int Target_C;
    long int Target_D;

    /* 电机pwm */
    long int Moto_A;
    long int Moto_B;
    long int Moto_C;
    long int Moto_D;    

    /* XYZ轴目标速度 */
    float Move_X;
    float Move_Y;
    float Move_Z;

    /* 速度限幅 */
    float RC_Velocity;

    /* 运行状态标志位 */
     enum Run_State state;

    /* 陀螺仪比例系数 */
    float Gyro_K;     				  											//陀螺仪比例系数
}sMecanum_Typedef;

extern sMecanum_Typedef Mecanum;
extern u8 rxbuf[8],Usart_ON_Flag; //串口PS2控制相关变量

void Mecanum_Param_Init(void);
void Mecanum_Update(int A,int B,int C,int D);
void Kinematic_Analysis(float Vx,float Vy,float Vz);
void Kinematic_Analysis2(float Vx,float Vy,float Vz);
void Get_RC(Flag mode);
void Count_Velocity(void);
void Count_Velocity(void);
void PS2_Usart_Control(void);
u8 click_RC (void);

#endif /* _MECANUM_H */

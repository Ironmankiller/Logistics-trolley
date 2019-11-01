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
    ready,                   //就绪
    scan,                    //扫描
    goToDeparture,           //前往原料区
    grabFromDeparture,       //在原料区抓取物料
    goToProcessing,          //前往加工区
    placeToProcessing,       //在加工区放置物料
    backToDeparture,         //返回原料区
    grabFromProcessing,      //在加工区抓取物料
    goToFinish,              //前往成品区
    placeToFinish,           //在成品区放置物料
    backToProcessing,        //返回加工区
    back                     //返回起始位置
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

enum rgb_Order
{
    RGB,
    RBG,
    GRB,
    GBR,
    BRG,
    BGR
};

// 小车麦轮编号
//     头
//   B—^—C
//   |   |
//   A—^—D
//    尾

typedef struct
{
    /* 感光条参数(不可用) */
    float side_AB;
    float side_BC;
    float side_CD;
    float side_DA;
    
    /* 放置顺序(左右中) */
    enum Order_Set Place_Order;
    /* 抓取顺序(左右中) */
    enum Order_Set Departure_Order;
    
    /* 颜色顺序(rgb) */
    enum rgb_Order Color;
    /* 夹取顺序(rgb) */
    enum rgb_Order Grab;
    /* 颜色顺序检测标志位 */
    Flag color_order_ready;
    /* 夹取顺序检测标志位 */
    Flag grab_order_ready;
    
    /* XY轴位置坐标 */
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

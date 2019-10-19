#ifndef __CAR_H
#define __CAR_H

#include "APP\Math\Vect\Vect.h"
#include "My_Flag.h"

#define Width 0.163f
#define Length 0.150f
#define K 11.11f  //K=a/α  a为舵机给定值 α为目标角度

/* 题目标号 */
enum Question_num_ {
    Question_1_1,
    Question_1_2,
    Question_1_3,
    Question_1_4,
    Question_2_1,
    Question_2_2,
    Question_2_3,
    Point_calibrate,
    pause
};

/* 题目是否开始 */
enum Question_state_
{
    running,
    stoping
};
enum Car_state
{
    forward,
    back,
    left,
    right,
    forward_right,
    back_right,
    back_left,
    forward_left,
    stop
};

enum Control_Mode{
    AUTO,
    PS2,
    CCD,
};

/* 小车信息 */
typedef struct
{
    int32_t Left_Speed;
    int32_t Right_Speed;

    float Left_Speed_Target;
    float Right_Speed_Target;

    int32_t Steer_Moto;         //舵机控制值
    int32_t Steer_Moto_INIT;     //舵机中值

    float velocity;             //速度
    float angle;                //转向角度

    enum Car_state state;       //小车运行状态
    enum Control_Mode Mode;     //小车控制方式

    enum Question_num_ Question; //当前题目
    enum Question_state_ State;  //是否运行
}sCar_Typedef;

extern sCar_Typedef Car;     //小车信息
/**********************************************
* 小车参数初始化
**********************************************/
void Car_Param_Init(void);

/**********************************************
* 更新小车信息
**********************************************/
void Car_Update(float left, float right);

/**********************************************
* 小车运动学分析
**********************************************/
void Car_Kinematic_Analysis(float velocity,float angle);

/**********************************************
* 获得PS2遥控信息
**********************************************/
void Get_PS2(void);
#endif // !__CAR_H

#ifndef _Pan_Tilt_H
#define _Pan_Tilt_H

#include "APP\Math\Vect\Vect.h"
#include "My_Flag.h"
#include "sys.h"

/* 题目标号 */
enum Question_num {
    Question_11,       //将弹丸射出
    Question_12,       //200~300mm中心线处，偏差不少于50cm
    Question_13,       //输入角度和距离，按靶环数计分
    Question_21,       //任意放置目标，自动开炮
    Question_22,       //往返运动，过程中开炮
    Question_23,       //其他
    Gyro_calibrate    //矫正陀螺仪

};
/* 题目是否开始 */
enum Question_state
{
    ready,    //准备模式
    adjust,   //调整模式
    fire,     //开火
    charging,  //充电中
    finish    //发射完成
};


/* 电磁炮信息 */
typedef struct
{
    //炮台姿态
    float X_Angle;
    float Y_Angle;

    //炮台目标姿态
    float Target_X_Angle;
    float Target_Y_Angle;
    
    //靶子目标角度和长度
    float Target_Angle;
    float Target_Length;
    int Target_Length_Buchang;
    
    //炮台中值
    int X_Offset;
    int Y_Offset;
    
    float voltage;

    enum Question_num Question; //当前题目
    enum Question_state State;  //是否运行
}sPan_Tilt_Typedef;

#define _FIRE_HIGH PEout(7) = 1
#define _FIRE_LOW  PEout(7) = 0

#define _CHARGE_HIGH PBout(1) = 1
#define _CHARGE_LOW  PBout(1) = 0

extern sPan_Tilt_Typedef Pan_Tilt;     //小车信息
/**********************************************
* 电磁炮参数初始化
**********************************************/
void Pan_Tilt_Param_Init(void);

/**********************************************
* 更新电磁炮信息
**********************************************/
void Pan_Tilt_Update(float x, float y);

/**********************************************
* 设置目标点
**********************************************/
void Pan_Tilt_Set_Target(float Target_Angle, float Target_Length);

/**********************************************
* 控制炮台运动
**********************************************/
void Pan_Tilt_Control(void);
void Pan_Tilt_Control_plus(void);

/**********************************************
* 更新炮台当前角度
**********************************************/
void Pan_Tilt_Update(float x, float y);

/**********************************************
* 炮台复位
**********************************************/
void Pan_Tilt_Reset(void); 

/**********************************************
* 炮台校零
**********************************************/
void Pan_Tilt_Calibrate(void);

void Fire_INT(void);
void Charge_INT(void);
#endif /* _Pan_Tilt_H */


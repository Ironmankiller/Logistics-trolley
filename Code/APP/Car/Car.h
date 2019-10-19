#ifndef __CAR_H
#define __CAR_H

#include "APP\Math\Vect\Vect.h"
#include "My_Flag.h"

#define Width 0.163f
#define Length 0.150f
#define K 11.11f  //K=a/��  aΪ�������ֵ ��ΪĿ��Ƕ�

/* ��Ŀ��� */
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

/* ��Ŀ�Ƿ�ʼ */
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

/* С����Ϣ */
typedef struct
{
    int32_t Left_Speed;
    int32_t Right_Speed;

    float Left_Speed_Target;
    float Right_Speed_Target;

    int32_t Steer_Moto;         //�������ֵ
    int32_t Steer_Moto_INIT;     //�����ֵ

    float velocity;             //�ٶ�
    float angle;                //ת��Ƕ�

    enum Car_state state;       //С������״̬
    enum Control_Mode Mode;     //С�����Ʒ�ʽ

    enum Question_num_ Question; //��ǰ��Ŀ
    enum Question_state_ State;  //�Ƿ�����
}sCar_Typedef;

extern sCar_Typedef Car;     //С����Ϣ
/**********************************************
* С��������ʼ��
**********************************************/
void Car_Param_Init(void);

/**********************************************
* ����С����Ϣ
**********************************************/
void Car_Update(float left, float right);

/**********************************************
* С���˶�ѧ����
**********************************************/
void Car_Kinematic_Analysis(float velocity,float angle);

/**********************************************
* ���PS2ң����Ϣ
**********************************************/
void Get_PS2(void);
#endif // !__CAR_H

#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"
#include <stdint.h>

enum _now_last
{
    NOW = 1,
    LAST = 0,
};

/*PID����*/
typedef struct
{
    float set_value;              /*����ֵ*/
    float real_value[2];          /*���ֵ(ʵ��ֵ)*/
    float err[3];                 /*ƫ��*/
    float pid_out;                /*PID������*/
    float last_pid_out;           /*�ϴ�������*/
    double err_iteg;               /*������*/
    float lastdev;                /*�ϴλ������ֵ*/

    /*Param*/
    float Kp;                     /*Kp*/
    float Ki;                     /*Ki*/
    float Kd;                     /*Kd*/
    float out_max;                /*����޷�*/
    float iteg_max;               /*�����޷�*/
    float iteg_a;                 /*�������ٻ���A*/
    float iteg_b;                 /*�������ٻ���B*/
    float dead_zone;              /*����*/
    float out_filter_alpha;       /*�����ͨ�˲���ϵ��*/
    float derivative_filter_alpha;/*����ȫ΢��ϵ��*/
    float gama;                   /*΢������ϵ��*/
}pid;

extern pid pid_moto1;
extern pid pid_moto2;


void update_pid_param(pid * pid,  float * param, float setvalue);
float pid_pos_calc(pid * pid, float real_value);
float pid_inc_calc(pid * pid, float real_value);
float pid_fuzzy_calc(pid * pid, float real_value);


#endif


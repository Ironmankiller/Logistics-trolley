#ifndef __PID_H
#define __PID_H

#include "stm32f4xx.h"
#include <stdint.h>

enum _now_last
{
    NOW = 1,
    LAST = 0,
};

/*PID参数*/
typedef struct
{
    float set_value;              /*给定值*/
    float real_value[2];          /*输出值(实际值)*/
    float err[3];                 /*偏差*/
    float pid_out;                /*PID运算结果*/
    float last_pid_out;           /*上次运算结果*/
    double err_iteg;               /*误差积分*/
    float lastdev;                /*上次积分项的值*/

    /*Param*/
    float Kp;                     /*Kp*/
    float Ki;                     /*Ki*/
    float Kd;                     /*Kd*/
    float out_max;                /*输出限幅*/
    float iteg_max;               /*积分限幅*/
    float iteg_a;                 /*连续变速积分A*/
    float iteg_b;                 /*连续变速积分B*/
    float dead_zone;              /*死区*/
    float out_filter_alpha;       /*输出低通滤波器系数*/
    float derivative_filter_alpha;/*不完全微分系数*/
    float gama;                   /*微分先行系数*/
}pid;

extern pid pid_moto1;
extern pid pid_moto2;


void update_pid_param(pid * pid,  float * param, float setvalue);
float pid_pos_calc(pid * pid, float real_value);
float pid_inc_calc(pid * pid, float real_value);
float pid_fuzzy_calc(pid * pid, float real_value);


#endif


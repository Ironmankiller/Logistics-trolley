#include "pid.h"
#include "APP\Math\My_Math.h"
#include "fuzzy.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\MPU6050\MPU6050.h"
/**************************************************************************
函数功能：更新PID参数
**************************************************************************/
void update_pid_param(pid * pid,  float * param, float setvalue)
{
    memset(pid, 0.0f, sizeof(pid));

    pid->set_value = (float)setvalue; /*更新设定值*/
    pid->Kp = (float)param[0];
    pid->Ki = (float)param[1];
    pid->Kd = (float)param[2];
    pid->out_max = (float)param[3];
    pid->iteg_max = (float)param[4];
    pid->dead_zone = (float)param[5];
    pid->iteg_b = (float)param[6];
    pid->iteg_a = (float)param[7];
    pid->out_filter_alpha = (float)param[8];
    pid->derivative_filter_alpha = (float)param[9];
    pid->gama = (float)param[10];
};

/**************************************************************************
函数功能：位置式PID控制器
函数输入：注意输入可以是位置、速度、加速度。
根据位置式离散PID公式
out=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
out代表输出
**************************************************************************/
float pid_pos_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*比例运算输出*/
    float ui = 0.0f; /*积分运算输出*/
    float ud = 0.0f; /*微分运算输出*/
    //float c1,c2,c3,temp; /*微分先行变量*/
    float iteg_f = 0.0f; /*连续变速积分系数*/
    
    pid->real_value[NOW] = real_value;                       /*系统当前的实际值*/
    pid->err[NOW] = pid->set_value - pid->real_value[NOW];   /*获取误差*/
    /*死区控制*/
    if (my_abs(pid->err[NOW]) < pid->dead_zone)
        pid->err[NOW] = 0;

    /*连续变速积分 (deadzone,b) f=1; (b,a+b) f=(a+b-err)/a; (a+b,inf) f=0;*/
    else if (my_abs(pid->err[NOW]) < pid->iteg_b)
        iteg_f = 1;
    else if (my_abs(pid->err[NOW]) < (pid->iteg_a + pid->iteg_b))
        iteg_f = (pid->iteg_a - my_abs(pid->err[NOW]) + pid->iteg_b) / pid->iteg_a;
    else
        iteg_f = 0;
    pid->err_iteg += (pid->err[NOW] * iteg_f);        /*误差积分*/
    //pid->err_iteg += pid->err[NOW];  /*误差积分*/

    /*积分限幅*/
    if (pid->err_iteg > pid->iteg_max)       pid->err_iteg = pid->iteg_max;
    else if (pid->err_iteg < -pid->iteg_max) pid->err_iteg = -pid->iteg_max;

    /*PID运算*/
    up = pid->err[NOW] * pid->Kp;
    ui = pid->err_iteg * pid->Ki;
    /*普通微分运算*/
    ud = (pid->real_value[LAST] - pid->real_value[NOW]) * pid->Kd;
    //ud = mpu6050.Gyro.y * pid->Kd;
    /*不完全微分*/
    //ud = (pid->err[LAST] - pid->err[NOW]) * pid->Kd * pid->derivative_filter_alpha + pid->lastdev * (1-pid->derivative_filter_alpha);
    /*微分先行*/
    //temp= pid->gama * pid->Kd + pid->Kp;
    //c3= pid->Kd / temp;
    //c2= (pid->Kd + pid->Kp) / temp;
    //c1= pid->gama*c3;
    //ud= c1*pid->lastdev + c2*pid->real_value[NOW] + c3*pid->real_value[LAST];
    
    pid->pid_out = up + ui + ud;
    
    /*低通滤波器*/
    pid->pid_out = pid->pid_out*pid->out_filter_alpha + pid->last_pid_out*(1-pid->out_filter_alpha);

    /*对PID输出进行限幅*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];            /*系统实际迭代*/
    pid->err[LAST] = pid->err[NOW];                          /*系统偏差迭代*/
    pid->last_pid_out = pid->pid_out;                        /*系统输出迭代*/
    pid->lastdev = ud;                                       /*微分结果迭代*/

    return pid->pid_out;
}

/**************************************************************************
函数功能：增量式PID控制器
根据增量式离散PID公式
out+=A0*[e（k）-e(k-1)]+A1*e(k)+A2*[e(k)-2e(k-1)+e(k-2)]
A0 = Kp + Ki + Kd; A1 = -Kp - 2Kd; A2 = Kd
e(k)代表本次偏差
e(k-1)代表上一次的偏差  以此类推
out代表输出
**************************************************************************/
float pid_inc_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*比例运算输出*/
    float ui = 0.0f; /*积分运算输出*/
    float ud = 0.0f; /*微分运算输出*/
    pid->real_value[NOW] = real_value;                       /*系统当前的实际值*/
    pid->err[2] = pid->set_value - pid->real_value[NOW];   /*获取误差*/

    /*PID运算*/
    up = (pid->err[2] - pid->err[1]) * pid->Kp;
    ui = pid->err[2] * pid->Ki;
    /*普通微分运算*/
    //ud = (pid->err[2] - 2 * pid->err[1] + pid->err[0]) * pid->Kd; 
    /*不完全微分*/
    //ud = (pid->err[2] - 2 * pid->err[1] + pid->err[0]) * pid->Kd * pid->derivative_filter_alpha + pid->lastdev * (1 - pid->derivative_filter_alpha);
    /*微分先行*/
    
    pid->pid_out += (up + ui + ud);  /*增量式PID*/
    
    /*低通滤波器*/
    pid->pid_out = pid->pid_out*pid->out_filter_alpha + pid->last_pid_out*(1-pid->out_filter_alpha);
    
    /*对PID输出进行限幅*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];  /*系统实际迭代*/
    pid->err[1] = pid->err[2];             
    pid->err[0] = pid->err[1];  /*系统偏差迭代*/
    pid->last_pid_out = pid->pid_out;                        /*系统输出迭代*/
    pid->lastdev = ud;                                       /*微分结果迭代*/

    return pid->pid_out;
}


/**************************************************************************
函数功能：位置式模糊PID控制器
函数输入：注意输入可以是位置、速度、加速度。
根据位置式离散PID公式
out=Kp*e(k)+Ki*∑e(k)+Kd[e（k）-e(k-1)]
e(k)代表本次偏差
e(k-1)代表上一次的偏差
∑e(k)代表e(k)以及之前的偏差的累积和;其中k为1,2,,k;
Kp、Ki、Kd由模糊控制器根据误差和误差变化率输出
out代表输出
**************************************************************************/
float pid_fuzzy_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*比例运算输出*/
    float ui = 0.0f; /*积分运算输出*/
    float ud = 0.0f; /*微分运算输出*/
    float iteg_f = 0.0f; /*连续变速积分系数*/
    float Kp_new;

    pid->real_value[NOW] = real_value;  /*系统当前的实际值*/
    pid->err[NOW] = pid->set_value - pid->real_value[NOW];  /*获取误差*/

    /*死区控制*/
    if (my_abs(pid->err[NOW]) < pid->dead_zone)
        pid->err[NOW] = 0;

    /*连续变速积分 (deadzone,b) f=1; (b,a+b) f=(a+b-err)/a; (a+b,inf) f=0;*/
    else if (my_abs(pid->err[NOW]) < pid->iteg_b)
        iteg_f = 1;
    else if (my_abs(pid->err[NOW]) < (pid->iteg_a + pid->iteg_b))
        iteg_f = (pid->iteg_a - my_abs(pid->err[NOW]) + pid->iteg_b) / pid->iteg_a;
    else
        iteg_f = 0;
    pid->err_iteg += (pid->err[NOW] * iteg_f);  /*误差积分*/
    //pid->err_iteg += pid->err[NOW];  /*误差积分*/

    /*积分限幅*/
    if (pid->err_iteg > pid->iteg_max)       pid->err_iteg = pid->iteg_max;
    else if (pid->err_iteg < -pid->iteg_max) pid->err_iteg = -pid->iteg_max;

    /*模糊控制更新PID参数*/
    Kp_new =pid->Kp + Fuzzy_P(pid->err[NOW], pid->err[LAST] - pid->err[NOW]);
    //u3_printf("Fuzzy: %f\r\n", Fuzzy_P(pid->err[NOW], pid->err[LAST] - pid->err[NOW]));
    //pid->Kd += Fuzzy_D(pid->err[NOW], pid->err[LAST] - pid->err[NOW]);

    /*PID运算*/
    up = pid->err[NOW] * Kp_new;
    ui = pid->err_iteg * pid->Ki;
    ud = (pid->real_value[LAST] - pid->real_value[NOW]) * pid->Kd; /*微分先行PID*/
    //u3_printf("ERROR ENC:%f\r\n", (float)(pid->real_value[LAST] - pid->real_value[NOW]));
    //ud = (pid->err[LAST] - pid->err[NOW]) * pid->Kd;
    pid->pid_out = up + ui + ud;
    //u3_printf("pid out:%f\tup: %f\t errnow:%f\r\n", pid->pid_out, up, pid->err[NOW]);

    //if (pid->err[NOW]<40)
    //{
    //    pid->pid_out = pid->err[NOW] * 0.05f + (pid->real_value[LAST] - pid->real_value[NOW])*0.02f;
    //}

    /*对PID输出进行限幅*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];  /*系统实际迭代*/
    pid->err[LAST] = pid->err[NOW];  /*系统偏差迭代*/

    return pid->pid_out;
}

#include "pid.h"
#include "APP\Math\My_Math.h"
#include "fuzzy.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\MPU6050\MPU6050.h"
/**************************************************************************
�������ܣ�����PID����
**************************************************************************/
void update_pid_param(pid * pid,  float * param, float setvalue)
{
    memset(pid, 0.0f, sizeof(pid));

    pid->set_value = (float)setvalue; /*�����趨ֵ*/
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
�������ܣ�λ��ʽPID������
�������룺ע�����������λ�á��ٶȡ����ٶȡ�
����λ��ʽ��ɢPID��ʽ
out=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
out�������
**************************************************************************/
float pid_pos_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*�����������*/
    float ui = 0.0f; /*�����������*/
    float ud = 0.0f; /*΢���������*/
    //float c1,c2,c3,temp; /*΢�����б���*/
    float iteg_f = 0.0f; /*�������ٻ���ϵ��*/
    
    pid->real_value[NOW] = real_value;                       /*ϵͳ��ǰ��ʵ��ֵ*/
    pid->err[NOW] = pid->set_value - pid->real_value[NOW];   /*��ȡ���*/
    /*��������*/
    if (my_abs(pid->err[NOW]) < pid->dead_zone)
        pid->err[NOW] = 0;

    /*�������ٻ��� (deadzone,b) f=1; (b,a+b) f=(a+b-err)/a; (a+b,inf) f=0;*/
    else if (my_abs(pid->err[NOW]) < pid->iteg_b)
        iteg_f = 1;
    else if (my_abs(pid->err[NOW]) < (pid->iteg_a + pid->iteg_b))
        iteg_f = (pid->iteg_a - my_abs(pid->err[NOW]) + pid->iteg_b) / pid->iteg_a;
    else
        iteg_f = 0;
    pid->err_iteg += (pid->err[NOW] * iteg_f);        /*������*/
    //pid->err_iteg += pid->err[NOW];  /*������*/

    /*�����޷�*/
    if (pid->err_iteg > pid->iteg_max)       pid->err_iteg = pid->iteg_max;
    else if (pid->err_iteg < -pid->iteg_max) pid->err_iteg = -pid->iteg_max;

    /*PID����*/
    up = pid->err[NOW] * pid->Kp;
    ui = pid->err_iteg * pid->Ki;
    /*��ͨ΢������*/
    ud = (pid->real_value[LAST] - pid->real_value[NOW]) * pid->Kd;
    //ud = mpu6050.Gyro.y * pid->Kd;
    /*����ȫ΢��*/
    //ud = (pid->err[LAST] - pid->err[NOW]) * pid->Kd * pid->derivative_filter_alpha + pid->lastdev * (1-pid->derivative_filter_alpha);
    /*΢������*/
    //temp= pid->gama * pid->Kd + pid->Kp;
    //c3= pid->Kd / temp;
    //c2= (pid->Kd + pid->Kp) / temp;
    //c1= pid->gama*c3;
    //ud= c1*pid->lastdev + c2*pid->real_value[NOW] + c3*pid->real_value[LAST];
    
    pid->pid_out = up + ui + ud;
    
    /*��ͨ�˲���*/
    pid->pid_out = pid->pid_out*pid->out_filter_alpha + pid->last_pid_out*(1-pid->out_filter_alpha);

    /*��PID��������޷�*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];            /*ϵͳʵ�ʵ���*/
    pid->err[LAST] = pid->err[NOW];                          /*ϵͳƫ�����*/
    pid->last_pid_out = pid->pid_out;                        /*ϵͳ�������*/
    pid->lastdev = ud;                                       /*΢�ֽ������*/

    return pid->pid_out;
}

/**************************************************************************
�������ܣ�����ʽPID������
��������ʽ��ɢPID��ʽ
out+=A0*[e��k��-e(k-1)]+A1*e(k)+A2*[e(k)-2e(k-1)+e(k-2)]
A0 = Kp + Ki + Kd; A1 = -Kp - 2Kd; A2 = Kd
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��  �Դ�����
out�������
**************************************************************************/
float pid_inc_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*�����������*/
    float ui = 0.0f; /*�����������*/
    float ud = 0.0f; /*΢���������*/
    pid->real_value[NOW] = real_value;                       /*ϵͳ��ǰ��ʵ��ֵ*/
    pid->err[2] = pid->set_value - pid->real_value[NOW];   /*��ȡ���*/

    /*PID����*/
    up = (pid->err[2] - pid->err[1]) * pid->Kp;
    ui = pid->err[2] * pid->Ki;
    /*��ͨ΢������*/
    //ud = (pid->err[2] - 2 * pid->err[1] + pid->err[0]) * pid->Kd; 
    /*����ȫ΢��*/
    //ud = (pid->err[2] - 2 * pid->err[1] + pid->err[0]) * pid->Kd * pid->derivative_filter_alpha + pid->lastdev * (1 - pid->derivative_filter_alpha);
    /*΢������*/
    
    pid->pid_out += (up + ui + ud);  /*����ʽPID*/
    
    /*��ͨ�˲���*/
    pid->pid_out = pid->pid_out*pid->out_filter_alpha + pid->last_pid_out*(1-pid->out_filter_alpha);
    
    /*��PID��������޷�*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];  /*ϵͳʵ�ʵ���*/
    pid->err[1] = pid->err[2];             
    pid->err[0] = pid->err[1];  /*ϵͳƫ�����*/
    pid->last_pid_out = pid->pid_out;                        /*ϵͳ�������*/
    pid->lastdev = ud;                                       /*΢�ֽ������*/

    return pid->pid_out;
}


/**************************************************************************
�������ܣ�λ��ʽģ��PID������
�������룺ע�����������λ�á��ٶȡ����ٶȡ�
����λ��ʽ��ɢPID��ʽ
out=Kp*e(k)+Ki*��e(k)+Kd[e��k��-e(k-1)]
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��
��e(k)����e(k)�Լ�֮ǰ��ƫ����ۻ���;����kΪ1,2,,k;
Kp��Ki��Kd��ģ�������������������仯�����
out�������
**************************************************************************/
float pid_fuzzy_calc(pid * pid, float real_value)
{
    float up = 0.0f; /*�����������*/
    float ui = 0.0f; /*�����������*/
    float ud = 0.0f; /*΢���������*/
    float iteg_f = 0.0f; /*�������ٻ���ϵ��*/
    float Kp_new;

    pid->real_value[NOW] = real_value;  /*ϵͳ��ǰ��ʵ��ֵ*/
    pid->err[NOW] = pid->set_value - pid->real_value[NOW];  /*��ȡ���*/

    /*��������*/
    if (my_abs(pid->err[NOW]) < pid->dead_zone)
        pid->err[NOW] = 0;

    /*�������ٻ��� (deadzone,b) f=1; (b,a+b) f=(a+b-err)/a; (a+b,inf) f=0;*/
    else if (my_abs(pid->err[NOW]) < pid->iteg_b)
        iteg_f = 1;
    else if (my_abs(pid->err[NOW]) < (pid->iteg_a + pid->iteg_b))
        iteg_f = (pid->iteg_a - my_abs(pid->err[NOW]) + pid->iteg_b) / pid->iteg_a;
    else
        iteg_f = 0;
    pid->err_iteg += (pid->err[NOW] * iteg_f);  /*������*/
    //pid->err_iteg += pid->err[NOW];  /*������*/

    /*�����޷�*/
    if (pid->err_iteg > pid->iteg_max)       pid->err_iteg = pid->iteg_max;
    else if (pid->err_iteg < -pid->iteg_max) pid->err_iteg = -pid->iteg_max;

    /*ģ�����Ƹ���PID����*/
    Kp_new =pid->Kp + Fuzzy_P(pid->err[NOW], pid->err[LAST] - pid->err[NOW]);
    //u3_printf("Fuzzy: %f\r\n", Fuzzy_P(pid->err[NOW], pid->err[LAST] - pid->err[NOW]));
    //pid->Kd += Fuzzy_D(pid->err[NOW], pid->err[LAST] - pid->err[NOW]);

    /*PID����*/
    up = pid->err[NOW] * Kp_new;
    ui = pid->err_iteg * pid->Ki;
    ud = (pid->real_value[LAST] - pid->real_value[NOW]) * pid->Kd; /*΢������PID*/
    //u3_printf("ERROR ENC:%f\r\n", (float)(pid->real_value[LAST] - pid->real_value[NOW]));
    //ud = (pid->err[LAST] - pid->err[NOW]) * pid->Kd;
    pid->pid_out = up + ui + ud;
    //u3_printf("pid out:%f\tup: %f\t errnow:%f\r\n", pid->pid_out, up, pid->err[NOW]);

    //if (pid->err[NOW]<40)
    //{
    //    pid->pid_out = pid->err[NOW] * 0.05f + (pid->real_value[LAST] - pid->real_value[NOW])*0.02f;
    //}

    /*��PID��������޷�*/
    if (pid->pid_out > pid->out_max)      pid->pid_out = pid->out_max;
    else if (pid->pid_out < -pid->out_max) pid->pid_out = -pid->out_max;

    pid->real_value[LAST] = pid->real_value[NOW];  /*ϵͳʵ�ʵ���*/
    pid->err[LAST] = pid->err[NOW];  /*ϵͳƫ�����*/

    return pid->pid_out;
}

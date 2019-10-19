#ifndef _PID_FORWARD_H_
#define _PID_FORWARD_H_
#include "stm32f4xx.h"
#include "APP\Math\My_Math.h"

#ifdef __cplusplus  
extern "C" {
#endif 
 

#define FWD_CTL_PERIOD	10 /*ǰ����������10ms*/

typedef struct 
{
	int32_t kf;				/*ǰ��ϵ��*/
	int32_t kp_aclt;		/*����ϵ��(���ٽ׶�)*/
	int32_t kp_decr;		/*����ϵ��(���ٽ׶�)*/
	int32_t kd_aclt;		/*΢��ϵ��(���ٽ׶�)*/
	int32_t kd_decr;		/*΢��ϵ��(���ٽ׶�)*/
	int32_t max_out;		/*�����������*/
	int32_t vect_v;         /*����ٶȷ���*/

    float start_v;		/*���ٶ�(mm/s)*/
    float unif_v;		    /*�����ٶ�(mm/s)*/
    float end_v;	        /*ĩ�ٶ�(mm/s)*/
    float aclt;	     	/*���ٶ�(mm/s/s)*/
    float decr;	    	/*���ٶ�(mm/s/s)*/
	
    float total_len;		/*��·��(mm)*/
    float start_pos;      /*��ʼλ��(mm)*/
    float cmd_pos[3];	    /*ָ��λ��(mm)*/
    float real_pos;		/*��ʵλ��(mm)*/

	uint32_t start_time;    /*��·�εĿ�ʼʱ��(ms)*/
	uint32_t end_time;		/*��·�εĽ���ʱ��(ms)*/
	uint32_t aclt_time;		/*���ٽ���ʱ���(ms)*/
	uint32_t unif_time;		/*���ٽ���ʱ��㣬uniform_time(ms)*/
	uint32_t decr_time;		/*���ٹ��̵�ʱ�䳤��(ms)*/
}
forward_ctl_t;  

void update_forward_param(forward_ctl_t * p, int32_t * param);
void update_speed_param(forward_ctl_t * p, int32_t * param);
void update_forward_ctl(forward_ctl_t * p, uint32_t start_time);
float forward_ctl(forward_ctl_t *p, float current_pos, uint32_t current_time);

#ifdef __cplusplus  
}
#endif 

#endif /* __PID_FORWARD_H_ */
 





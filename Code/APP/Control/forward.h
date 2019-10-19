#ifndef _PID_FORWARD_H_
#define _PID_FORWARD_H_
#include "stm32f4xx.h"
#include "APP\Math\My_Math.h"

#ifdef __cplusplus  
extern "C" {
#endif 
 

#define FWD_CTL_PERIOD	10 /*前馈控制周期10ms*/

typedef struct 
{
	int32_t kf;				/*前馈系数*/
	int32_t kp_aclt;		/*反馈系数(加速阶段)*/
	int32_t kp_decr;		/*反馈系数(减速阶段)*/
	int32_t kd_aclt;		/*微分系数(加速阶段)*/
	int32_t kd_decr;		/*微分系数(减速阶段)*/
	int32_t max_out;		/*最大控制量输出*/
	int32_t vect_v;         /*输出速度方向*/

    float start_v;		/*初速度(mm/s)*/
    float unif_v;		    /*匀速速度(mm/s)*/
    float end_v;	        /*末速度(mm/s)*/
    float aclt;	     	/*加速度(mm/s/s)*/
    float decr;	    	/*减速度(mm/s/s)*/
	
    float total_len;		/*总路程(mm)*/
    float start_pos;      /*初始位置(mm)*/
    float cmd_pos[3];	    /*指令位置(mm)*/
    float real_pos;		/*真实位置(mm)*/

	uint32_t start_time;    /*本路段的开始时间(ms)*/
	uint32_t end_time;		/*本路段的结束时间(ms)*/
	uint32_t aclt_time;		/*加速结束时间点(ms)*/
	uint32_t unif_time;		/*匀速结束时间点，uniform_time(ms)*/
	uint32_t decr_time;		/*减速过程的时间长度(ms)*/
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
 





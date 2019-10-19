#include "forward.h"
#include <stdint.h>
#include <stdio.h>

/*设定前馈参数*/
void update_forward_param(forward_ctl_t * p, int32_t * param)
{
	p->kf      = (float)param[0];
	p->kp_aclt = (float)param[1];
	p->kp_decr = (float)param[2];
	p->kd_aclt = (float)param[3];
	p->kd_decr = (float)param[4];
	p->max_out = (float)param[5];
}
void update_speed_param(forward_ctl_t * p, int32_t * param)
{
	p->unif_v = (float)param[0];
	p->aclt   = (float)param[1];
	p->decr   = (float)param[2];
}

/*
* 更新前馈控制信息
* 程序仅运行一次，计算时间
*/
void update_forward_ctl(forward_ctl_t * p, uint32_t start_time)
{
	float aclt_len, decr_len, unif_len;

	/*加速度判断*/
	if (p->start_v == p->unif_v) p->aclt = 0;
	else                         p->aclt = (p->start_v < p->unif_v) ? fabs(p->aclt) : -fabs(p->aclt);
	if (p->end_v == p->unif_v)   p->decr = 0;
	else                         p->decr = (p->end_v < p->unif_v) ? -fabs(p->decr) : fabs(p->decr);

	p->start_time = start_time;

	if (p->aclt == 0) p->aclt_time = 0;
	else              p->aclt_time = 1000 * (p->unif_v - p->start_v) / p->aclt;/*加速时间长度*/
	if (p->decr == 0) p->decr_time = 0;
	else              p->decr_time = 1000 * (p->end_v - p->unif_v) / p->decr;/*减速时间长度*/

	aclt_len = ((p->start_v + p->unif_v) * p->aclt_time / 2) / 1000;/*加速距离*/
	decr_len = ((p->unif_v + p->end_v)  * p->decr_time / 2) / 1000;	/*减速距离*/
	unif_len = p->total_len - aclt_len - decr_len;					/*匀速距离*/

	/*如果该路程不能达到满速*/
	if (unif_len < 0)
	{
		if (p->aclt > 0 && p->decr < 0) /*先加速再减速*/
		{
			p->unif_v = (float)(sqrt((p->aclt * square(p->end_v) - p->decr * square(p->start_v)
				- 2.0f * p->aclt * p->decr * p->total_len) / (p->aclt - p->decr)));
		}
		else/*先匀速再加速或者是再减速，忽略中间速度，直接看初速度和末速度*///需要加速度保护
		{
			float temp_aclt = (square(p->end_v) - square(p->start_v)) / 2 / p->total_len;
			/*加速度保护,如果算出来的加速度过大，那么需要改变末速度*/
			if (my_abs((temp_aclt)) <= (my_max(my_abs(p->aclt), my_abs(p->decr))))
			{
				p->aclt = temp_aclt;
				p->decr = p->aclt;
				p->unif_v = (p->start_v + p->end_v) / 2;
			}
			else
			{
				p->aclt = my_max(my_abs(p->aclt), my_abs(p->decr));
				if (p->end_v < p->start_v) p->aclt = -p->aclt;
				p->decr = p->aclt;
				p->end_v = sqrt(2 * p->aclt * p->total_len + square(p->start_v));
				p->unif_v = (p->start_v + p->end_v) / 2;
			}
		}
		/*重新计算参数*/
		if (p->aclt == 0) p->aclt_time = 0;
		else              p->aclt_time = 1000 * (p->unif_v - p->start_v) / p->aclt;/*加速时间长度，ms*/
		if (p->decr == 0) p->decr_time = 0;
		else              p->decr_time = 1000 * (p->end_v - p->unif_v) / p->decr;/*减速时间长度，ms*/

		aclt_len = ((p->start_v + p->unif_v) * p->aclt_time / 2) / 1000;	/*加速距离*/
		decr_len = ((p->unif_v + p->end_v)  * p->decr_time / 2) / 1000;	/*减速距离*/
		unif_len = p->total_len - aclt_len - decr_len;					/*匀速距离*/
	}

	p->unif_time = 1000 * unif_len / p->unif_v;					/*匀速时间长度，ms*/
	p->aclt_time += p->start_time;								/*计算加速过程结束的时间*/
	p->unif_time += p->aclt_time;								/*计算匀速过程结束的时间*/
	p->end_time = p->unif_time + p->decr_time;				/*结束时间*/

	/*更新指令位置记录*/
	p->cmd_pos[0] = -(float)(2 * p->start_v - p->aclt * FWD_CTL_PERIOD / 1000) * FWD_CTL_PERIOD / 2 / 1000;
	p->cmd_pos[1] = 0;
	p->cmd_pos[2] = 0;

}

/*    程序不断更新
*	forward_ctl:	前馈控制
*	p:				前馈控制结构体
*	current_pos:	当前位置
*	current_time:	当前时间
*	返回值：		速度控制量
*/
float forward_ctl(forward_ctl_t *p, float current_pos, uint32_t current_time)
{
    float    v0;			/*本周期初速度*/
    float    vt;			/*本周期末速度*/
	uint32_t   time;			/*计时变量*/
    float    backward_out;	/*反馈项输出*/
    float    forward_out;   /*前馈项输出*/
    float    derive_out;    /*微分项输出*/
    float    ctl_v;			/*总的速度控制量输出*/

	/*更新当前实际位置*/
	p->real_pos = current_pos;

	/*加速过程*/
	if (current_time < p->aclt_time)
	{
		time = current_time - p->start_time;
		v0 = time * p->aclt / 1000 + p->start_v; 
		time += FWD_CTL_PERIOD;
		vt = time * p->aclt / 1000 + p->start_v;
	}
	/*匀速过程*/
	else if (current_time < p->unif_time)
	{
		v0 = p->unif_v;
		vt = p->unif_v;
	}
	/*减速过程*/
	else
	{
		if (current_time < p->end_time) //如果没有超时
		{
			time = current_time - p->unif_time;
			v0 = p->unif_v + time*p->decr / 1000;
			time += FWD_CTL_PERIOD;
			/*防止时间不是FWD_CTL_PERIOD整数倍出现负数的情况*/
			if (time > p->decr_time)
			{
				time = p->decr_time;
			}
			vt = p->unif_v + time * p->decr / 1000;
		}
		else //if (current_time >= p->end_time) 如果超时说明这条路径是最后一条路径了
		{
			v0 = 0;
			vt = 0;
		}
	}

	/*指令位置
	* real_pos:      当前实际位置
	* cmd_pos[0]:    上一指令位置
	* cmd_pos[1]:    当前指令位置
	* cmd_pos[2]:    下一指令位置
	*/
	p->cmd_pos[2] += ((v0 + vt) / 2) * FWD_CTL_PERIOD / 1000;
	if (p->cmd_pos[2] > p->total_len) p->cmd_pos[2] = p->total_len;                //防止运算超过最大路程

	forward_out = p->kf * (p->cmd_pos[2] - p->cmd_pos[1]);							/*前馈控制输出项*/
	if (current_time < p->unif_time)
	{
		backward_out = p->kp_aclt * (p->cmd_pos[1] - p->real_pos);					    /*反馈比例项*/
		derive_out = p->kd_aclt * (p->cmd_pos[2] - 2 * p->cmd_pos[1] + p->cmd_pos[0]);	/*指令位置微分项*/
	}
	else
	{
		backward_out = p->kp_decr * (p->cmd_pos[1] - p->real_pos);					    /*反馈比例项*/
		derive_out = p->kd_decr * (p->cmd_pos[2] - 2 * p->cmd_pos[1] + p->cmd_pos[0]);	/*指令位置微分项*/
	}

	ctl_v = backward_out + forward_out + derive_out;  							       	/*总控制量*/

	/*更新指令位置*/
	p->cmd_pos[0] = p->cmd_pos[1];
	p->cmd_pos[1] = p->cmd_pos[2];

	return ctl_v;
}

 








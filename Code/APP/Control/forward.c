#include "forward.h"
#include <stdint.h>
#include <stdio.h>

/*�趨ǰ������*/
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
* ����ǰ��������Ϣ
* ���������һ�Σ�����ʱ��
*/
void update_forward_ctl(forward_ctl_t * p, uint32_t start_time)
{
	float aclt_len, decr_len, unif_len;

	/*���ٶ��ж�*/
	if (p->start_v == p->unif_v) p->aclt = 0;
	else                         p->aclt = (p->start_v < p->unif_v) ? fabs(p->aclt) : -fabs(p->aclt);
	if (p->end_v == p->unif_v)   p->decr = 0;
	else                         p->decr = (p->end_v < p->unif_v) ? -fabs(p->decr) : fabs(p->decr);

	p->start_time = start_time;

	if (p->aclt == 0) p->aclt_time = 0;
	else              p->aclt_time = 1000 * (p->unif_v - p->start_v) / p->aclt;/*����ʱ�䳤��*/
	if (p->decr == 0) p->decr_time = 0;
	else              p->decr_time = 1000 * (p->end_v - p->unif_v) / p->decr;/*����ʱ�䳤��*/

	aclt_len = ((p->start_v + p->unif_v) * p->aclt_time / 2) / 1000;/*���پ���*/
	decr_len = ((p->unif_v + p->end_v)  * p->decr_time / 2) / 1000;	/*���پ���*/
	unif_len = p->total_len - aclt_len - decr_len;					/*���پ���*/

	/*�����·�̲��ܴﵽ����*/
	if (unif_len < 0)
	{
		if (p->aclt > 0 && p->decr < 0) /*�ȼ����ټ���*/
		{
			p->unif_v = (float)(sqrt((p->aclt * square(p->end_v) - p->decr * square(p->start_v)
				- 2.0f * p->aclt * p->decr * p->total_len) / (p->aclt - p->decr)));
		}
		else/*�������ټ��ٻ������ټ��٣������м��ٶȣ�ֱ�ӿ����ٶȺ�ĩ�ٶ�*///��Ҫ���ٶȱ���
		{
			float temp_aclt = (square(p->end_v) - square(p->start_v)) / 2 / p->total_len;
			/*���ٶȱ���,���������ļ��ٶȹ�����ô��Ҫ�ı�ĩ�ٶ�*/
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
		/*���¼������*/
		if (p->aclt == 0) p->aclt_time = 0;
		else              p->aclt_time = 1000 * (p->unif_v - p->start_v) / p->aclt;/*����ʱ�䳤�ȣ�ms*/
		if (p->decr == 0) p->decr_time = 0;
		else              p->decr_time = 1000 * (p->end_v - p->unif_v) / p->decr;/*����ʱ�䳤�ȣ�ms*/

		aclt_len = ((p->start_v + p->unif_v) * p->aclt_time / 2) / 1000;	/*���پ���*/
		decr_len = ((p->unif_v + p->end_v)  * p->decr_time / 2) / 1000;	/*���پ���*/
		unif_len = p->total_len - aclt_len - decr_len;					/*���پ���*/
	}

	p->unif_time = 1000 * unif_len / p->unif_v;					/*����ʱ�䳤�ȣ�ms*/
	p->aclt_time += p->start_time;								/*������ٹ��̽�����ʱ��*/
	p->unif_time += p->aclt_time;								/*�������ٹ��̽�����ʱ��*/
	p->end_time = p->unif_time + p->decr_time;				/*����ʱ��*/

	/*����ָ��λ�ü�¼*/
	p->cmd_pos[0] = -(float)(2 * p->start_v - p->aclt * FWD_CTL_PERIOD / 1000) * FWD_CTL_PERIOD / 2 / 1000;
	p->cmd_pos[1] = 0;
	p->cmd_pos[2] = 0;

}

/*    ���򲻶ϸ���
*	forward_ctl:	ǰ������
*	p:				ǰ�����ƽṹ��
*	current_pos:	��ǰλ��
*	current_time:	��ǰʱ��
*	����ֵ��		�ٶȿ�����
*/
float forward_ctl(forward_ctl_t *p, float current_pos, uint32_t current_time)
{
    float    v0;			/*�����ڳ��ٶ�*/
    float    vt;			/*������ĩ�ٶ�*/
	uint32_t   time;			/*��ʱ����*/
    float    backward_out;	/*���������*/
    float    forward_out;   /*ǰ�������*/
    float    derive_out;    /*΢�������*/
    float    ctl_v;			/*�ܵ��ٶȿ��������*/

	/*���µ�ǰʵ��λ��*/
	p->real_pos = current_pos;

	/*���ٹ���*/
	if (current_time < p->aclt_time)
	{
		time = current_time - p->start_time;
		v0 = time * p->aclt / 1000 + p->start_v; 
		time += FWD_CTL_PERIOD;
		vt = time * p->aclt / 1000 + p->start_v;
	}
	/*���ٹ���*/
	else if (current_time < p->unif_time)
	{
		v0 = p->unif_v;
		vt = p->unif_v;
	}
	/*���ٹ���*/
	else
	{
		if (current_time < p->end_time) //���û�г�ʱ
		{
			time = current_time - p->unif_time;
			v0 = p->unif_v + time*p->decr / 1000;
			time += FWD_CTL_PERIOD;
			/*��ֹʱ�䲻��FWD_CTL_PERIOD���������ָ��������*/
			if (time > p->decr_time)
			{
				time = p->decr_time;
			}
			vt = p->unif_v + time * p->decr / 1000;
		}
		else //if (current_time >= p->end_time) �����ʱ˵������·�������һ��·����
		{
			v0 = 0;
			vt = 0;
		}
	}

	/*ָ��λ��
	* real_pos:      ��ǰʵ��λ��
	* cmd_pos[0]:    ��һָ��λ��
	* cmd_pos[1]:    ��ǰָ��λ��
	* cmd_pos[2]:    ��һָ��λ��
	*/
	p->cmd_pos[2] += ((v0 + vt) / 2) * FWD_CTL_PERIOD / 1000;
	if (p->cmd_pos[2] > p->total_len) p->cmd_pos[2] = p->total_len;                //��ֹ���㳬�����·��

	forward_out = p->kf * (p->cmd_pos[2] - p->cmd_pos[1]);							/*ǰ�����������*/
	if (current_time < p->unif_time)
	{
		backward_out = p->kp_aclt * (p->cmd_pos[1] - p->real_pos);					    /*����������*/
		derive_out = p->kd_aclt * (p->cmd_pos[2] - 2 * p->cmd_pos[1] + p->cmd_pos[0]);	/*ָ��λ��΢����*/
	}
	else
	{
		backward_out = p->kp_decr * (p->cmd_pos[1] - p->real_pos);					    /*����������*/
		derive_out = p->kd_decr * (p->cmd_pos[2] - 2 * p->cmd_pos[1] + p->cmd_pos[0]);	/*ָ��λ��΢����*/
	}

	ctl_v = backward_out + forward_out + derive_out;  							       	/*�ܿ�����*/

	/*����ָ��λ��*/
	p->cmd_pos[0] = p->cmd_pos[1];
	p->cmd_pos[1] = p->cmd_pos[2];

	return ctl_v;
}

 








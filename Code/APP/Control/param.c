#include "param.h"
#include "pid.h"
#include "forward.h"



int16_t grab_departure_right[4] = {-30, 10, -10, 45};
int16_t grab_departure_left[4] = {28, 13, -10, 45};
int16_t grab_departure_center[4] = {0, -5, 0, 45};
int16_t grab_processing[4] = {-180, 25, 25, 45};

int16_t turn_processing[3] = {-180, -60, -35};
int16_t turn_departure[3] = {0, -60, -35};
int16_t turn_finish[3] = {90, -60, -35};

int16_t place_processing[4] = {-180, 25, 25, 0};
int16_t place_finish[4] = {90, 25, 25, 0};

pid pid_motorA_speed;
pid pid_motorB_speed;
pid pid_motorC_speed;
pid pid_motorD_speed;

pid pid_motorA_position;
pid pid_motorB_position;
pid pid_motorC_position;
pid pid_motorD_position;

pid pid_x;
pid pid_y;
pid pid_x_speed;
pid pid_y_speed;
pid pid_length;
pid pid_angle;
pid pid_angle_speed;
pid pid_speed;

//����Ϊ Kp Ki Kd ����Ͻ� �������� ���� ȫ�ٻ����� ���ٻ����� �����ͨ�˲�ϵ�� ����ȫ΢��ϵ�� ΢������ϵ��
float pidmotorAspeedparm[11] = { 200.0f, 28.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 100.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorBspeedparm[11] = { 200.0f, 28.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 100.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorCspeedparm[11] = { 200.0f, 28.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 100.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorDspeedparm[11] = { 200.0f, 28.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 100.0f, 100.0f, 0.8f, 0.0f, 0.0f };

float pidmotorApositionparm[11] = { 120.0f, 12.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 10000.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorBpositionparm[11] = { 120.0f, 12.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 10000.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorCpositionparm[11] = { 120.0f, 12.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 10000.0f, 100.0f, 0.8f, 0.0f, 0.0f };
float pidmotorDpositionparm[11] = { 120.0f, 12.0f, 0.0f, 16000.0f, 40000.0f, 0.0f, 10000.0f, 100.0f, 0.8f, 0.0f, 0.0f };


float pidxparam[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float pidyparam[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

float pidxspeedparam[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
float pidyspeedparam[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

float pidlengthparam[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

float pidangleparam[11] = { 1650.0f, 0.0f, 0.75f,16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.8f, 0.0f, 0.0f };
float pidspeedparam[11] = { 123.0f, 0.615f, 0.0f,16800.0f, 16800.0f, 0.0f, 100000.0f, 0.0f, 0.8f, 0.0f, 0.0f };

float pidanglespeedparam[11] = { 13.20f, 0.387, 0.0157f, 16800.0f, 168000.0f, 5.0f, 1000.0f, 300.0f, 0.8f, 0.0f, 0.0f };

float pidq1param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
float pidq2param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
float pidq3param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
float pidq4param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
float pidq5param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };
float pidq6param[11] = { 0.0f, 0.0f, 0.0f, 16800.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

#include "param.h"
#include "pid.h"
#include "forward.h"

pos posScanQR1 = {10, 50};           //扫描二维码1
pos posScanQR2 = {170, 50};          //扫描二维码2
pos posScanQR3 = {170, 28};          //扫描二维码3

pos posDeparture1 = {112, 40};       //物料区1
pos posDeparture2 = {112, 4};        //物料区2

pos posProcessR1 = {155, 170};       //加工区R1
pos posProcessR2 = {155, 170};       //加工区R2
pos posProcessL1 = {95, 157};        //加工区L1
pos posProcessL2 = {95, 157};        //加工区L2
pos posProcessC1 = {125, 157};       //加工区1C
pos posProcessC2 = {125, 157};       //加工区2C

pos posFinishL = {180, 93};          //成品区R32
pos posFinishR = {180, 153};         //成品区R
pos posFinishC = {187, 123};         //成品区R

int16_t grab_departure_right[4] = {-30, 10, -10, 55};
int16_t grab_departure_left[4] = {27, 13, -10, 55};
int16_t grab_departure_center[4] = {0, -5, 0, 55};
int16_t grab_processing[4] = {-162, 22, 30, 55};

int16_t turn_processing[3] = {-180, -60, -35};
int16_t turn_departure[3] = {0, -60, -35};
int16_t turn_finish[3] = {90, -60, -35};
int16_t turn_scan[3] = {0, -35, 20};

int16_t place_processing[4] = {-163, 25, 28, 0};
int16_t place_finish[4] = {60, 25, 25, 0};

int16_t move_scan_color[4] = {0, -35, 20, -100};
int16_t move_scan_grab[4] = {0, 0, -5, -100};
int16_t move_ready[4] = {0, -70, -20, -10};

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

//依次为 Kp Ki Kd 输出上界 积分上限 死区 全速积分区 变速积分区 输出低通滤波系数 不完全微分系数 微分先行系数
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

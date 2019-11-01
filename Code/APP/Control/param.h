#ifndef _PARAM_H_
#define _PARAM_H_

#include "stm32f4xx.h"
#include "APP\Control\pid.h"
#include "APP\Control\forward.h"
#include "sys.h"

typedef struct
{
    int16_t x;
    int16_t y;
}pos;  //����ṹ��

extern pos posScanQR1;
extern pos posScanQR2;
extern pos posScanQR3;
extern pos posDeparture1;
extern pos posDeparture2;

extern pos posProcessR1;
extern pos posProcessR2;
extern pos posProcessL1;
extern pos posProcessL2;
extern pos posProcessC1;
extern pos posProcessC2;

extern pos posFinishR;
extern pos posFinishL;
extern pos posFinishC;

extern int16_t grab_departure_right[4];
extern int16_t grab_departure_left[4];
extern int16_t grab_departure_center[4];
extern int16_t grab_processing[4];

extern int16_t turn_processing[3];
extern int16_t turn_departure[3];
extern int16_t turn_finish[3];
extern int16_t turn_scan[3];

extern int16_t place_processing[4];
extern int16_t place_finish[4];

extern int16_t move_scan_color[4];
extern int16_t move_scan_grab[4];
extern int16_t move_ready[4];


extern pid pid_x;
extern pid pid_y;
extern pid pid_x_speed;
extern pid pid_y_speed;
extern pid pid_length;
extern pid pid_angle;
extern pid pid_angle_speed;
extern pid pid_speed;

extern pid pid_motorA_speed;
extern pid pid_motorB_speed;
extern pid pid_motorC_speed;
extern pid pid_motorD_speed;

extern pid pid_motorA_position;
extern pid pid_motorB_position;
extern pid pid_motorC_position;
extern pid pid_motorD_position;

extern float pidxparam[11];
extern float pidxparam[11];
extern float pidyparam[11];
extern float pidxspeedparam[11];
extern float pidyspeedparam[11];
extern float pidlengthparam[11];
extern float pidangleparam[11];
extern float pidanglespeedparam[11];
extern float pidq1param[11];
extern float pidq2param[11];
extern float pidq3param[11];
extern float pidq4param[11];
extern float pidq5param[11];
extern float pidq6param[11];
extern float pidspeedparam[11];

extern float pidmotorAspeedparm[11];
extern float pidmotorBspeedparm[11];
extern float pidmotorCspeedparm[11];
extern float pidmotorDspeedparm[11];

extern float pidmotorApositionparm[11];
extern float pidmotorBpositionparm[11];
extern float pidmotorCpositionparm[11];
extern float pidmotorDpositionparm[11];
#endif


#ifndef _MPU60502_H
#define _MPU60502_H

#include "stm32f4xx.h"
#include "./Sensor_Param/sensor_param.h"
#include "./MPU6050/MPU6050.h"


#define GYRO_DEATH_ZOOM2 20   //  20 / 65536

#define OFFSET_AV_NUM2 50
#define FILTER_NUM2 10

#define TO_ANGLE2 				0.06103f 		//0.061036 //   4000/65536  +-2000   ???
#define ACC_ADJ_EN2 									//是否允许校准加速度计,(定义则允许)

#define GYRO_FULLSCALE2 (250) //陀螺仪最大量程
#define ACC_FULLSCALE2 (2) //加速度计最大量程

#if  ACC_FULLSCALE2  == 2
#define AccAxis_Sensitive2 (float)(16384)
#elif ACC_FULLSCALE2 == 4
#define AccAxis_Sensitive (float)(8192)
#elif ACC_FULLSCALE2 == 8
#define AccAxis_Sensitive2 (float)(4096)
#elif ACC_FULLSCALE2 == 16
#define AccAxis_Sensitive2 (float)(2048)
#endif 

#if   GYRO_FULLSCALE2 == 250
#define GyroAxis_Sensitive2 (float)(131.0)
#elif GYRO_FULLSCALE2 == 500
#define GyroAxis_Sensitive2 (float)(65.5)
#elif GYRO_FULLSCALE2 == 1000
#define GyroAxis_Sensitive2 (float)(32.8)
#elif GYRO_FULLSCALE2 == 2000
#define GyroAxis_Sensitive2 (float)(16.4)
#endif




typedef struct
{
    char Acc_CALIBRATE;
    char Gyro_CALIBRATE;
    xyz_s16_t Acc_I16;
    xyz_s16_t Gyro_I16;

    xyz_f_t Acc;
    xyz_f_t Gyro;

    //	XYZ_STRUCT Acc_deg;
    xyz_f_t Gyro_deg;
	xyz_f_t Acc_deg;

    xyz_f_t Acc_Offset;
    xyz_f_t Gyro_Offset;
    xyz_f_t Gyro_Auto_Offset;
    float Temprea_Offset;
    float Gyro_Temprea_Adjust;
    float ACC_Temprea_Adjust;

    s16 Tempreature;
    float TEM_LPF;
    float Ftempreature;
}MPU60502_STRUCT;

extern MPU60502_STRUCT mpu60502;
extern float mpu60502_tmp[ITEMS];



void MPU60502_Init(u16);
void MPU60502_Read(void);

void MPU60502_Data_Prepare(float T);


/**********************************************
* 陀螺仪矫正
**********************************************/
void MPU60502_GYRO_Calibrate(void);

/**********************************************
* 加速度计矫正
**********************************************/
void MPU60502_ACC_Calibrate(void);

#endif

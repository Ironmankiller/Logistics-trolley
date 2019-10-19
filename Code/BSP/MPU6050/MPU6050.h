#ifndef _MPU6050_H
#define _MPU6050_H

#include "stm32f4xx.h"
#include "APP\Sensor_Param\Sensor_Param.h"


#define GYRO_DEATH_ZOOM 20   //  20 / 65536

#define OFFSET_AV_NUM 50
#define FILTER_NUM 10

#define TO_ANGLE 				0.06103f 		//0.061036 //   4000/65536  +-2000   ???
#define ACC_ADJ_EN 									//是否允许校准加速度计,(定义则允许)

#define GYRO_FULLSCALE (250) //陀螺仪最大量程
#define ACC_FULLSCALE (2) //加速度计最大量程

#if  ACC_FULLSCALE  == 2
#define AccAxis_Sensitive (float)(16384)
#elif ACC_FULLSCALE == 4
#define AccAxis_Sensitive (float)(8192)
#elif ACC_FULLSCALE == 8
#define AccAxis_Sensitive (float)(4096)
#elif ACC_FULLSCALE == 16
#define AccAxis_Sensitive (float)(2048)
#endif 

#if   GYRO_FULLSCALE == 250
#define GyroAxis_Sensitive (float)(131.072)
#elif GYRO_FULLSCALE == 500
#define GyroAxis_Sensitive (float)(65.5)
#elif GYRO_FULLSCALE == 1000
#define GyroAxis_Sensitive (float)(32.8)
#elif GYRO_FULLSCALE == 2000
#define GyroAxis_Sensitive (float)(16.4)
#endif

enum
{
    A_X = 0,
    A_Y,
    A_Z,
    G_Y,
    G_X,
    G_Z,
    TEM,
    ITEMS,
};


typedef struct
{
    char Acc_CALIBRATE;
    char Gyro_CALIBRATE;
    xyz_s16_t Acc_I16;
    xyz_s16_t Gyro_I16;

    xyz_f_t Acc;
    xyz_f_t Gyro;

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
}MPU6050_STRUCT;

extern MPU6050_STRUCT mpu6050;
extern float mpu6050_tmp[ITEMS];



void MPU6050_Init(u16);
void MPU6050_Read(void);

void MPU6050_Data_Prepare(float T);


/**********************************************
* 陀螺仪矫正
**********************************************/
void MPU6050_GYRO_Calibrate(void);

/**********************************************
* 加速度计矫正
**********************************************/
void MPU6050_ACC_Calibrate(void);

#endif

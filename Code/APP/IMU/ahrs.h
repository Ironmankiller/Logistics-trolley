#ifndef __AHRS__H__
#define __AHRS__H__

#include "BSP\MPU6050\MPU6050.h"
#include "math.h"

#define invSqrtf( iSq ) (1.0f/sqrtf((float)iSq))
#define squa( Sq )      (((float)Sq)*((float)Sq))
#define toRad( Math_D )	((float)(Math_D)*0.0174532925f)
#define toDeg( Math_R )	((float)(Math_R)*57.2957795f)

/* 采样率200Hz */
#define MPU6050_SAMPLERATE_FEQ  	 ((uint16_t)200)    //200Hz
#define MPU6050_SAMPLERATE_PRI  	 ((float)0.005f)    //5ms
#define MPU6050_SAMPLERATE_HALFPRI   ((float)0.0025f)   //2.5ms

/*------------------------------------------
欧拉角
------------------------------------------*/
typedef volatile struct
{
	float Pitch;
	float Roll;
	float Yaw;
}AHRS_EulerAngleTypeDef;
/*------------------------------------------
四元数
------------------------------------------*/
typedef volatile struct
{
	float q0;
	float q1;
	float q2;
	float q3;
}AHRS_QuaternionTypeDef;
/*------------------------------------------
磁力计
------------------------------------------*/
typedef volatile struct
{
	float EllipseSita;
	float EllipseA;
	float EllipseB;
	float X;
	float Y;
	float Z;
}Ellipse_TypeDef;
/*------------------------------------------
函数定义
------------------------------------------*/
float Kalman_Filter1(float Accel, float Gyro);
float Kalman_Filter2(float Accel, float Gyro);
void ahrs(void);
//void RungeKutta(AHRS_QuaternionTypeDef *pQ, float GyrX, float GyrY, float GyrZ, float halfTimes);
//void IMU_update(float gx, float gy, float gz, float ax, float ay, float az);

//矫正
void arhs_calibrate(void);
extern AHRS_EulerAngleTypeDef  EulerAngle;
extern AHRS_QuaternionTypeDef  QuadNumber;
extern Ellipse_TypeDef Mag;

#endif



#include "ahrs.h"
#include "APP\Math\My_Math.h"
#include "arm_math.h"
#include "BSP\MPU60502\MPU60502.h"
/*------------------------------------------
				AHRS参数
------------------------------------------*/
#define Kp 				 (15.0f)
#define Ki 				 (0.02f)
/*------------------------------------------
				全局变量
------------------------------------------*/
///* 四元数结构体	*/
AHRS_QuaternionTypeDef  QuadNumber = { 1.0f, 0.0f, 0.0f, 0.0f };
/* 欧拉角结构体 */
AHRS_EulerAngleTypeDef  EulerAngle = { 0.0f,0.0f,0.0f };
/* 磁力计结构体 */
Ellipse_TypeDef Mag;
//
///*------------------------------------------
//函数功能: 欧拉角转四元数
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//          - Pointer of struct AHRS_EulerAngleTypeDef
//------------------------------------------*/
//void Euler_ToQuaternion(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
//{
//	float halfP = pE->Pitch / 2.0f;
//	float halfR = pE->Roll / 2.0f;
//	float halfY = pE->Yaw / 2.0f;
//
//#ifndef USE_ARM_MATH_LIB
//	float sinP = sinf(halfP);
//	float cosP = cosf(halfP);
//	float sinR = sinf(halfR);
//	float cosR = cosf(halfR);
//	float sinY = sinf(halfY);
//	float cosY = cosf(halfY);
//#else
//	float sinP = arm_sin_f32(halfP);
//	float cosP = arm_cos_f32(halfP);
//	float sinR = arm_sin_f32(halfR);
//	float cosR = arm_cos_f32(halfR);
//	float sinY = arm_sin_f32(halfY);
//	float cosY = arm_cos_f32(halfY);
//#endif
//	pQ->q0 = cosY*cosR*cosP + sinY*sinR*sinP;
//	pQ->q1 = cosY*cosR*sinP - sinY*sinR*cosP;
//	pQ->q2 = cosY*sinR*cosP + sinY*cosR*sinP;
//	pQ->q3 = sinY*cosR*cosP - cosY*sinR*sinP;
//}
//
//
///*------------------------------------------
//函数功能: 四元数转欧拉角
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//          - Pointer of struct AHRS_EulerAngleTypeDef
//          - 输出欧拉角为角度制
//------------------------------------------*/
//void Quaternion_ToEuler(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
//{
//	float NumQ_T11 = pQ->q0*pQ->q0 + pQ->q1*pQ->q1 - pQ->q2*pQ->q2 - pQ->q3*pQ->q3;
//	float NumQ_T12 = 2.0f * (pQ->q0*pQ->q3 + pQ->q1*pQ->q2);
//	float NumQ_T13 = 2.0f * (pQ->q1*pQ->q3 - pQ->q0*pQ->q2);
//	float NumQ_T23 = 2.0f * (pQ->q0*pQ->q1 + pQ->q2*pQ->q3);
//	float NumQ_T33 = pQ->q0*pQ->q0 - pQ->q1*pQ->q1 - pQ->q2*pQ->q2 + pQ->q3*pQ->q3;
//
//	pE->Pitch = toDeg(asinf(NumQ_T13));
//	pE->Roll = toDeg(atan2f(NumQ_T23, NumQ_T33));
//	pE->Yaw = toDeg(atan2f(NumQ_T12, NumQ_T11)) /*+ 180.0f*/;
//}
//
//
///*------------------------------------------
//函数功能: 四元数乘法（矩阵乘法）
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//		  - Pointer of struct AHRS_QuaternionTypeDef
//------------------------------------------*/
////AHRS_QuaternionTypeDef Quaternion_Multiply(AHRS_QuaternionTypeDef NowQ, AHRS_QuaternionTypeDef OldQ)
////{
////	AHRS_QuaternionTypeDef NewQ;
////
////	NewQ.q0 = NowQ.q0*OldQ.q0 - NowQ.q1*OldQ.q1 - NowQ.q2*OldQ.q2 - NowQ.q3*OldQ.q3;
////	NewQ.q1 = NowQ.q0*OldQ.q1 + NowQ.q1*OldQ.q0 + NowQ.q2*OldQ.q3 - NowQ.q3*OldQ.q2;
////	NewQ.q2 = NowQ.q0*OldQ.q2 - NowQ.q1*OldQ.q3 + NowQ.q2*OldQ.q0 + NowQ.q3*OldQ.q1;
////	NewQ.q3 = NowQ.q0*OldQ.q3 + NowQ.q1*OldQ.q2 - NowQ.q2*OldQ.q1 + NowQ.q3*OldQ.q0;
////
////	Quaternion_Normalize(&NewQ);
////
////	return NewQ;
////}
//
//
///*------------------------------------------
//函数功能: 四元数归一化
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//------------------------------------------*/
//void Quaternion_Normalize(AHRS_QuaternionTypeDef *pQ)
//{
//	float Normalize = 0.0f;
//
//	Normalize = invSqrtf(squa(pQ->q0) + squa(pQ->q1) + squa(pQ->q2) + squa(pQ->q3));
//
//	pQ->q0 = pQ->q0 * Normalize;
//	pQ->q1 = pQ->q1 * Normalize;
//	pQ->q2 = pQ->q2 * Normalize;
//	pQ->q3 = pQ->q3 * Normalize;
//}
//
//
///*------------------------------------------
//函数功能: 一阶龙格库塔法更新四元数
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//          - GyrX (弧度)
//          - GyrY (弧度)
//          - GyrZ (弧度)
//          - halfTimes (采样周期的一半,单位秒)
//------------------------------------------*/
//void RungeKutta(AHRS_QuaternionTypeDef *pQ, float GyrX, float GyrY, float GyrZ, float halfTimes)
//{
//	pQ->q0 = pQ->q0 + (-pQ->q1*GyrX - pQ->q2*GyrY - pQ->q3*GyrZ) * halfTimes;
//	pQ->q1 = pQ->q1 + (pQ->q0*GyrX - pQ->q3*GyrY + pQ->q2*GyrZ) * halfTimes;
//	pQ->q2 = pQ->q2 + (pQ->q3*GyrX + pQ->q0*GyrY - pQ->q1*GyrZ) * halfTimes;
//	pQ->q3 = pQ->q3 + (-pQ->q2*GyrX + pQ->q1*GyrY + pQ->q0*GyrZ) * halfTimes;
//}
//
//
///*------------------------------------------
//函数功能: 初始化AHRS结构体
//函数参数: - Pointer of struct AHRS_QuaternionTypeDef
//- Pointer of struct AHRS_EulerAngleTypeDef
//------------------------------------------*/
////void AHRS_Init(AHRS_QuaternionTypeDef *pQ, AHRS_EulerAngleTypeDef *pE)
////{
////	pQ->q0 = 1.0f;
////	pQ->q1 = 0.0f;
////	pQ->q2 = 0.0f;
////	pQ->q3 = 0.0f;
////	pE->Pitch = 0.0f;
////	pE->Roll = 0.0f;
////	pE->Yaw = 0.0f;
////}
//
//
///*------------------------------------------
//函数功能: 更新IMU
//函数参数：Acc Gyro角度制
//------------------------------------------*/
//void IMU_update(float gx, float gy, float gz, float ax, float ay, float az)
//{
//    float tempX = 0, tempY = 0;
//    float hx, hy, hz;
//    float wx, wy, wz;
//    float bx, bz;
//    float MegX, MegY, MegZ;
//    float Mq11, Mq12, Mq13, Mq21, Mq22, Mq23, Mq31, Mq32, Mq33;
//
//    float Normalize;
//    float Gx, Gy, Gz;
//
//    float ErrX, ErrY, ErrZ;
//    float AccX, AccY, AccZ;
//    float GyrX, GyrY, GyrZ;
//
//    static float AngZ_Temp = 0.0f;
//    static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;
//
//    Mq11 = squa(QuadNumber.q0) + squa(QuadNumber.q1) - squa(QuadNumber.q2) - squa(QuadNumber.q3);
//    Mq12 = 2.0f*(QuadNumber.q1*QuadNumber.q2 + QuadNumber.q0*QuadNumber.q3);
//    Mq13 = 2.0f*(QuadNumber.q1 * QuadNumber.q3 - QuadNumber.q0 * QuadNumber.q2);
//    Mq21 = 2.0f*(QuadNumber.q1*QuadNumber.q2 - QuadNumber.q0*QuadNumber.q3);
//    Mq22 = squa(QuadNumber.q0) - squa(QuadNumber.q1) + squa(QuadNumber.q2) - squa(QuadNumber.q3);
//    Mq23 = 2.0f*(QuadNumber.q0 * QuadNumber.q1 + QuadNumber.q2 * QuadNumber.q3);
//    Mq31 = 2.0f*(QuadNumber.q0*QuadNumber.q2 + QuadNumber.q1*QuadNumber.q3);
//    Mq32 = 2.0f*(QuadNumber.q2*QuadNumber.q3 - QuadNumber.q0*QuadNumber.q1);
//    Mq33 = squa(QuadNumber.q0) - squa(QuadNumber.q1) - squa(QuadNumber.q2) + squa(QuadNumber.q3);
//    //Mq33 = 1 - 2 * (squa(QuadNumber.q1) + squa(QuadNumber.q2));
//
//    if (my_abs(ax) !=0 || my_abs(ay) != 0 && my_abs(az) != 0)
//    {
//        Normalize = invSqrtf(squa(ax) + squa(ay) + squa(az));
//        AccX = ax * Normalize;
//        AccY = ay * Normalize;
//        AccZ = az * Normalize;
//    }
//
//	if (ak8975.Mag_Val.x != 0 || ak8975.Mag_Val.y != 0 || ak8975.Mag_Val.z != 0)
//	{
//		Normalize = invSqrtf(squa(ak8975.Mag_Val.x) + squa(ak8975.Mag_Val.y) + squa(ak8975.Mag_Val.z));
//		MegX = ak8975.Mag_Val.x*Normalize;
//		MegY = ak8975.Mag_Val.y*Normalize;
//		MegZ = ak8975.Mag_Val.z*Normalize;
//	}
//
//	/* 等效重力向量 */
//	Gx = Mq13;
//	Gy = Mq23;
//	Gz = Mq33;
//
//	hx = MegX*Mq11 + MegY*Mq21 + MegZ*Mq31;
//	hy = MegX*Mq12 + MegY*Mq22 + MegZ*Mq32;
//	hz = MegX*Mq13 + MegY*Mq23 + MegZ*Mq33;
//
//	bx = sqrtf(squa(hx) + squa(hy));
//	bz = hz;
//
//	wx = bx*Mq11 + bz*Mq13;
//	wy = bx*Mq21 + bz*Mq23;
//	wz = bx*Mq31 + bz*Mq33;
//
//	ErrX = (AccY*Gz - AccZ*Gy) + (MegY*wz - MegZ*wy);
//	ErrY = (AccZ*Gx - AccX*Gz) + (MegZ*wx - MegX*wz);
//	ErrZ = (AccX*Gy - AccY*Gx) + (MegX*wy - MegY*wx);
//
//	exInt = exInt + ErrX*Ki;
//	eyInt = eyInt + ErrY*Ki;
//	ezInt = ezInt + ErrZ*Ki;
//
//	GyrX = toRad(gx);
//	GyrY = toRad(gy);
//	GyrZ = toRad(gz);
//
//	GyrX = GyrX + Kp*ErrX + exInt;
//	GyrY = GyrY + Kp*ErrY + eyInt;
//	GyrZ = GyrZ + Kp*ErrZ + ezInt;
//
//	/* 一阶龙格库塔法更新四元数 */
//	RungeKutta(&QuadNumber, GyrX, GyrY, GyrZ, MPU6050_SAMPLERATE_HALFPRI);
//	/* 四元数规一化 */
//	Quaternion_Normalize(&QuadNumber);
//	/* 四元数转欧拉角 */
//	Quaternion_ToEuler(&QuadNumber, &EulerAngle);
//
//	//tempX = (Mag.X*arm_cos_f32(Mag.EllipseSita) + Mag.Y*arm_sin_f32(Mag.EllipseSita)) / Mag.EllipseB;
//	//tempY = (-Mag.X*arm_sin_f32(Mag.EllipseSita) + Mag.Y*arm_cos_f32(Mag.EllipseSita)) / Mag.EllipseA;
//	//EulerAngle.Yaw = atan2f(tempX, tempY);
//
//	/* 互补滤波 */
//	#define CF_A 0.9f
//	#define CF_B 0.1f
//	AngZ_Temp = AngZ_Temp + GyrZ * MPU6050_SAMPLERATE_PRI;
//	AngZ_Temp = CF_A * AngZ_Temp + CF_B * EulerAngle.Yaw;
//
//	if (AngZ_Temp>360.0f)
//		EulerAngle.Yaw = AngZ_Temp - 360.0f;
//	else if (AngZ_Temp<0.0f)
//		EulerAngle.Yaw = AngZ_Temp + 360.0f;
//	else
//		EulerAngle.Yaw = AngZ_Temp;
//}
float pitch_offset = -0.0f;
float roll_offset = 6.0f;
/*-----------------------------------------------
函数功能:卡尔曼滤波姿态解算
函数说明:每5ms进入一次,采样率200Hz
-----------------------------------------------*/
void ahrs(void)
{
	uint8_t i = 0;
	float pitch_temp1 = 0;
	float roll_temp1 = 0;
	static float pitch_sum = 0.0f;
	static float roll_sum = 0.0f;
    //static float pitch_last = 0.0f;
    //static float roll_last = 0.0f;

	for (i = 0; i<3; i++)
	{
		/*根据加速度数值解算俯仰角Pitch、横滚角Roll。
		!注意这里静态误差*/
		pitch_temp1 = (atan(mpu6050.Acc_deg.y / mpu6050.Acc_deg.z)*57.2958f - pitch_offset);   //计算Pitch角度
        roll_temp1 = (atan(mpu6050.Acc_deg.x / mpu6050.Acc_deg.z)*57.2958f - roll_offset);  //计算Roll角度

		pitch_sum += pitch_temp1;
		roll_sum += roll_temp1;
	}

	pitch_temp1 = pitch_sum / 3.0f;	 //取出平均值
	roll_temp1 = roll_sum / 3.0f;	 //取出平均值

	pitch_sum = 0.0f;
	roll_sum = 0.0f;

	/* 卡尔曼滤波 */
    EulerAngle.Pitch = Kalman_Filter1(pitch_temp1, -mpu6050.Gyro_deg.y);     //卡尔曼滤波器
    EulerAngle.Roll = Kalman_Filter2(roll_temp1, -mpu6050.Gyro_deg.x);      //卡尔曼滤波器

    //float speed_time_duty = 0.0f;
    //speed_time_duty = Get_Cycle_T(2)*0.0000001f;
    //EulerAngle.Pitch_Acc = (EulerAngle.Pitch - pitch_last)/speed_time_duty;
    //EulerAngle.Roll_Acc = (EulerAngle.Roll - roll_last)/speed_time_duty;
    //pitch_last = EulerAngle.Pitch;
    //roll_last = EulerAngle.Roll;
}

//矫正
void arhs_calibratef(void)
{
    pitch_offset = -(EulerAngle.Pitch + pitch_offset);
    roll_offset = -(EulerAngle.Roll + roll_offset);

}

/*------------------------------------------
函数功能: 卡尔曼滤波1
函数参数: 无
------------------------------------------*/
float Kalman_Filter1(float Accel, float Gyro)
{
	//static float Gyro_y;   //Y轴陀螺仪数据暂存
	static float angle = 0.0;
	static float Q_bias = 0.0;
	static float angle_err = 0.0;
	static float Q_angle = 0.1;  //角度数据置信度
	static float Q_gyro = 0.1;   //角速度数据置信度
	static float R_angle = 0.5;
	static float dt = 0.005;	   //dt为滤波器采样时间(秒)
	static char  C_0 = 1;
	static float PCt_0 = 0, PCt_1 = 0, E = 0;
	static float K_0 = 0, K_1 = 0, t_0 = 0, t_1 = 0;
	static float Pdot[4] = { 0,0,0,0 };
	static float PP[2][2] = { { 1, 0 },{ 0, 1 } };

	angle += (Gyro - Q_bias) * dt; //先验估计
	angle_err = Accel - angle;	//zk-先验估计

	Pdot[0] = Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分
	Pdot[1] = -PP[1][1];
	Pdot[2] = -PP[1][1];
	Pdot[3] = Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + C_0 * PCt_0;

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle += K_0 * angle_err;	 //最优角度
	Q_bias += K_1 * angle_err;
	//Gyro_y = Gyro - Q_bias;	 //最优角速度

	return (angle);				 //最终角度
}

/*------------------------------------------
函数功能: 卡尔曼滤波2
函数参数: 无
------------------------------------------*/
float Kalman_Filter2(float Accel, float Gyro)
{
	//static float Gyro_y;   //Y轴陀螺仪数据暂存
	static float angle = 0.0;
	static float Q_bias = 0.0;
	static float angle_err = 0.0;
	static float Q_angle = 0.1;  //角度数据置信度
	static float Q_gyro = 0.1;   //角速度数据置信度
	static float R_angle = 0.5;
	static float dt = 0.005;	   //dt为滤波器采样时间(秒)
	static char  C_0 = 1;
	static float PCt_0 = 0, PCt_1 = 0, E = 0;
	static float K_0 = 0, K_1 = 0, t_0 = 0, t_1 = 0;
	static float Pdot[4] = { 0,0,0,0 };
	static float PP[2][2] = { { 1, 0 },{ 0, 1 } };

	angle += (Gyro - Q_bias) * dt; //先验估计
	angle_err = Accel - angle;	//zk-先验估计

	Pdot[0] = Q_angle - PP[0][1] - PP[1][0]; // Pk-先验估计误差协方差的微分
	Pdot[1] = -PP[1][1];
	Pdot[2] = -PP[1][1];
	Pdot[3] = Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + C_0 * PCt_0;

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle += K_0 * angle_err;	 //最优角度
	Q_bias += K_1 * angle_err;
	//Gyro_y = Gyro - Q_bias;	 //最优角速度

	return (angle);				 //最终角度
}

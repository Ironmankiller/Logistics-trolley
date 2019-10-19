/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
* 作者   ：匿名科创
* 文件名  ：imu.c
* 描述    ：姿态解算
* 官网    ：www.anotc.com
* 淘宝    ：anotc.taobao.com
* 技术Q群 ：190169595
**********************************************************************************/

#include "APP\Sensor_Param\Sensor_Param.h"
#include "imu.h"
#include "BSP\AK8975\AK8975.h"
#include "APP\Math\My_Math.h"
#include "ahrs.h"


#define Kp 0.6f                	// proportional gain governs rate of convergence to accelerometer/magnetometer
#define Ki 0.1f                	// 0.001  integral gain governs rate of convergence of gyroscope biases

#define ANGLE_TO_RADIAN 0.01745329f //*0.01745 = /57.3	角度转弧度
#define IMU_INTEGRAL_LIM  ( 2.0f *ANGLE_TO_RADIAN )
#define NORM_ACC_LPF_HZ 10  		//(Hz)
#define REF_ERR_LPF_HZ  1			//(Hz)

xyz_f_t reference_v;
ref_t 	ref;

xyz_f_t Gravity_Vec;  				//解算的重力向量

float Roll, Pitch, Yaw;    				//姿态角
float ref_q[4] = { 1,0,0,0 };  //四元数
float norm_acc, norm_q;
float norm_acc_lpf;

/* ANO三轴位姿更新程序 */
void IMUupdate(float half_T, float gx, float gy, float gz, float ax, float ay, float az, float *rol, float *pit, float *yaw)
{
    float ref_err_lpf_hz;
    static float mag_norm, mag_tmp_x, mag_tmp_y, yaw_mag;

    //mag_norm = my_sqrt(ak8975.Mag_Val.x * ak8975.Mag_Val.x + ak8975.Mag_Val.y * ak8975.Mag_Val.y);
    arm_sqrt_f32(ak8975.Mag_Val.x * ak8975.Mag_Val.x + ak8975.Mag_Val.y * ak8975.Mag_Val.y,&mag_norm);

    if (ak8975.Mag_Val.x != 0 && ak8975.Mag_Val.y != 0)
    {
        mag_tmp_x = (float)ak8975.Mag_Val.x / mag_norm;
        mag_tmp_y = (float)ak8975.Mag_Val.y / mag_norm;
        yaw_mag = fast_atan2(mag_tmp_y, mag_tmp_x) *57.3f;
    }
    //=============================================================================
    // 计算等效重力向量
    reference_v.x = 2 * (ref_q[1] * ref_q[3] - ref_q[0] * ref_q[2]);
    reference_v.y = 2 * (ref_q[0] * ref_q[1] + ref_q[2] * ref_q[3]);
    reference_v.z = 1 - 2 * (ref_q[1] * ref_q[1] + ref_q[2] * ref_q[2]);//ref_q[0]*ref_q[0] - ref_q[1]*ref_q[1] - ref_q[2]*ref_q[2] + ref_q[3]*ref_q[3];


    //这是把四元数换算成《方向余弦矩阵》中的第三列的三个元素。
    //根据余弦矩阵和欧拉角的定义，地理坐标系的重力向量，转到机体坐标系，正好是这三个元素。
    //所以这里的vx\y\z，其实就是当前的欧拉角（即四元数）的机体坐标参照系上，换算出来的重力单位向量。       
    //=============================================================================


    // 计算加速度向量的模
    //norm_acc = my_sqrt(ax*ax + ay*ay + az*az);
    arm_sqrt_f32(ax*ax + ay*ay + az*az, &norm_acc);
    norm_acc_lpf += NORM_ACC_LPF_HZ *(6.28f *half_T) *(norm_acc - norm_acc_lpf);  //10hz *3.14 * 2*0.001


    if (my_abs(ax)<4400 && my_abs(ay)<4400 && my_abs(az)<4400)
    {
        //把加计的三维向量转成单位向量。
        ax = ax / norm_acc; //4096.0f;
        ay = ay / norm_acc; //4096.0f;
        az = az / norm_acc; //4096.0f; 

        if (3800 < norm_acc && norm_acc < 4400)
        {
            /* 叉乘得到误差 */
            ref.err_tmp.x = ay*reference_v.z - az*reference_v.y;
            ref.err_tmp.y = az*reference_v.x - ax*reference_v.z;
            //ref.err_tmp.z = ax*reference_v.y - ay*reference_v.x;

            /* 误差低通 */
            ref_err_lpf_hz = REF_ERR_LPF_HZ *(6.28f *half_T);
            ref.err_lpf.x += ref_err_lpf_hz *(ref.err_tmp.x - ref.err_lpf.x);
            ref.err_lpf.y += ref_err_lpf_hz *(ref.err_tmp.y - ref.err_lpf.y);
            //ref.err_lpf.z += ref_err_lpf_hz *( ref.err_tmp.z  - ref.err_lpf.z );

            ref.err.x = ref.err_lpf.x;//
            ref.err.y = ref.err_lpf.y;//
            //ref.err.z = ref.err_lpf.z ;
        }
    }
    else
    {
        ref.err.x = 0;
        ref.err.y = 0;
        //ref.err.z = 0 ;
    }
    /* 误差积分 */
    ref.err_Int.x += ref.err.x *Ki * 2 * half_T;
    ref.err_Int.y += ref.err.y *Ki * 2 * half_T;
    ref.err_Int.z += ref.err.z *Ki * 2 * half_T;

    /* 积分限幅 */
    ref.err_Int.x = my_limit(ref.err_Int.x, -IMU_INTEGRAL_LIM, IMU_INTEGRAL_LIM);
    ref.err_Int.y = my_limit(ref.err_Int.y, -IMU_INTEGRAL_LIM, IMU_INTEGRAL_LIM);
    ref.err_Int.z = my_limit(ref.err_Int.z, -IMU_INTEGRAL_LIM, IMU_INTEGRAL_LIM);


    ref.g.x = gx*ANGLE_TO_RADIAN + (Kp*(ref.err.x + ref.err_Int.x));            //IN RADIAN
    ref.g.y = gy*ANGLE_TO_RADIAN + (Kp*(ref.err.y + ref.err_Int.y));		    //IN RADIAN
    ref.g.z = (gz - Kp *(0.3f) *To_180_degrees(yaw_mag - Yaw))*ANGLE_TO_RADIAN;//+ ( Kp*(ref.err.z + ref.err_Int.z) )  ;   //IN RADIAN
    //ref.g.z = (gz)*ANGLE_TO_RADIAN;//+ ( Kp*(ref.err.z + ref.err_Int.z) )  ;   //IN RADIAN

    /* 用叉积误差来做PI修正陀螺零偏 */

	/* 一阶龙格库塔法更新四元数 */
    ref_q[0] = ref_q[0] + (-ref_q[1] * ref.g.x - ref_q[2] * ref.g.y - ref_q[3] * ref.g.z)*half_T;
    ref_q[1] = ref_q[1] + (ref_q[0] * ref.g.x + ref_q[2] * ref.g.z - ref_q[3] * ref.g.y)*half_T;
    ref_q[2] = ref_q[2] + (ref_q[0] * ref.g.y - ref_q[1] * ref.g.z + ref_q[3] * ref.g.x)*half_T;
    ref_q[3] = ref_q[3] + (ref_q[0] * ref.g.z + ref_q[1] * ref.g.y - ref_q[2] * ref.g.x)*half_T;


    /* 四元数规一化 */
    norm_q = my_sqrt(ref_q[0] * ref_q[0] + ref_q[1] * ref_q[1] + ref_q[2] * ref_q[2] + ref_q[3] * ref_q[3]);
    ref_q[0] = ref_q[0] / norm_q;
    ref_q[1] = ref_q[1] / norm_q;
    ref_q[2] = ref_q[2] / norm_q;
    ref_q[3] = ref_q[3] / norm_q;


	/* 四元数转欧拉角 */
    *rol = fast_atan2(2 * (ref_q[0] * ref_q[1] + ref_q[2] * ref_q[3]), 1 - 2 * (ref_q[1] * ref_q[1] + ref_q[2] * ref_q[2])) *57.2958f;
    *pit = asinf(2 * (ref_q[1] * ref_q[3] - ref_q[0] * ref_q[2])) *57.2958f;
    //Yaw   = ( - fast_atan2(2*(ref_q[1]*ref_q[2] + ref_q[0]*ref_q[3]),ref_q[0]*ref_q[0] + ref_q[1]*ref_q[1] - ref_q[2]*ref_q[2] - ref_q[3]*ref_q[3]) )* 57.3;
    *yaw = fast_atan2(2 * (ref_q[1] * ref_q[2] - ref_q[0] * ref_q[3]), 2 * (ref_q[0] * ref_q[0] + ref_q[1] * ref_q[1]) - 1) *57.2958f;
}

/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/


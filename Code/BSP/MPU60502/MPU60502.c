/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
* 作者   ：匿名科创
* 文件名  ：mpu6050.c
* 描述    ：6轴传感器mpu6050驱动
* 官网    ：www.anotc.com
* 淘宝    ：anotc.taobao.com
* 技术Q群 ：190169595
**********************************************************************************/

#include "BSP\MPU60502\MPU60502.h"
#include "delay.h"
#include "BSP\MPU60502\MPU60502_Config.h"
#include "BSP\MCU\IIC2\IIC2.h"
#include "APP\Sensor_Param\sensor_param.h"

#define USE_ANO_PARAM (0)  //使用匿名参数

MPU60502_STRUCT mpu60502;

u8 mpu60502_buffer[14];

__INLINE void MPU60502_Read(void)
{
    I2C2_FastMode = 1;
    IIC2_Read_nByte(MPU60502_ADDR, MPU60502_RA_ACCEL_XOUT_H, 14, mpu60502_buffer);
}


/**************************实现函数********************************************
*函数原型:		u8 IICwriteBit(u8 dev, u8 reg, u8 bitNum, u8 data)
*功　　能:	  读 修改 写 指定设备 指定寄存器一个字节 中的1个位
输入	dev  目标设备地址
reg	   寄存器地址
bitNum  要修改目标字节的bitNum位
data  为0 时，目标位将被清0 否则将被置位
返回   成功 为1
失败为0
*******************************************************************************/
void IIC2writeBit(u8 dev, u8 reg, u8 bitNum, u8 data) {
    u8 b;
    IIC2_Read_nByte(dev, reg, 1, &b);
    b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
    IIC2_Write_1Byte(dev, reg, b);
}
/**************************实现函数********************************************
*函数原型:		u8 IICwriteBits(u8 dev,u8 reg,u8 bitStart,u8 length,u8 data)
*功　　能:	    读 修改 写 指定设备 指定寄存器一个字节 中的多个位
输入	dev  目标设备地址
reg	   寄存器地址
bitStart  目标字节的起始位
length   位长度
data    存放改变目标字节位的值
返回   成功 为1
失败为0
*******************************************************************************/
void IIC2writeBits(u8 dev, u8 reg, u8 bitStart, u8 length, u8 data)
{

    u8 b, mask;
    IIC2_Read_nByte(dev, reg, 1, &b);
    mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);
    data <<= (8 - length);
    data >>= (7 - bitStart);
    b &= mask;
    b |= data;
    IIC2_Write_1Byte(dev, reg, b);
}

/**************************实现函数********************************************
*函数原型:
*功　　能:	    设置 采样率
*******************************************************************************/
void MPU60502_set_SMPLRT_DIV(uint16_t hz)
{
    IIC2_Write_1Byte(MPU60502_ADDR, MPU60502_RA_SMPLRT_DIV, 1000 / hz - 1);
    //I2C_Single_Write(MPU6050_ADDRESS,MPU_RA_SMPLRT_DIV, (1000/sample_rate - 1));
}


/**************************实现函数********************************************
*函数原型:		void MPU6050_setClockSource(uint8_t source)
*功　　能:	    设置  MPU6050 的时钟源
* CLK_SEL | Clock Source
* --------+--------------------------------------
* 0       | Internal oscillator
* 1       | PLL with X Gyro reference
* 2       | PLL with Y Gyro reference
* 3       | PLL with Z Gyro reference
* 4       | PLL with external 32.768kHz reference
* 5       | PLL with external 19.2MHz reference
* 6       | Reserved
* 7       | Stops the clock and keeps the timing generator in reset
*******************************************************************************/
void MPU60502_setClockSource(uint8_t source)
{
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_PWR_MGMT_1, MPU60502_PWR1_CLKSEL_BIT, MPU60502_PWR1_CLKSEL_LENGTH, source);

}
/** Set full-scale gyroscope range.
* @param range New full-scale gyroscope range value
* @see getFullScaleRange()
* @see MPU6050_GYRO_FS_250
* @see MPU6050_RA_GYRO_CONFIG
* @see MPU6050_GCONFIG_FS_SEL_BIT
* @see MPU6050_GCONFIG_FS_SEL_LENGTH
*/
void MPU60502_setFullScaleGyroRange(uint8_t range) {
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_GYRO_CONFIG, MPU60502_GCONFIG_FS_SEL_BIT, MPU60502_GCONFIG_FS_SEL_LENGTH, range);
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_GYRO_CONFIG, 7, 3, 0x00);   //不自检
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setFullScaleAccelRange(uint8_t range)
*功　　能:	    设置  MPU6050 加速度计的最大量程
*******************************************************************************/
void MPU60502_setFullScaleAccelRange(uint8_t range) {
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_ACCEL_CONFIG, MPU60502_ACONFIG_AFS_SEL_BIT, MPU60502_ACONFIG_AFS_SEL_LENGTH, range);
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_ACCEL_CONFIG, 7, 3, 0x00);   //不自检
}
/**************************实现函数********************************************
*函数原型:		void MPU6050_setSleepEnabled(uint8_t enabled)
*功　　能:	    设置  MPU6050 是否进入睡眠模式
enabled =1   睡觉
enabled =0   工作
*******************************************************************************/
void MPU60502_setSleepEnabled(uint8_t enabled) {
    IIC2writeBit(MPU60502_ADDR, MPU60502_RA_PWR_MGMT_1, MPU60502_PWR1_SLEEP_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CMasterModeEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU60502_setI2CMasterModeEnabled(uint8_t enabled) {
    IIC2writeBit(MPU60502_ADDR, MPU60502_RA_USER_CTRL, MPU60502_USERCTRL_I2C_MST_EN_BIT, enabled);
}

/**************************实现函数********************************************
*函数原型:		void MPU6050_setI2CBypassEnabled(uint8_t enabled)
*功　　能:	    设置 MPU6050 是否为AUX I2C线的主机
*******************************************************************************/
void MPU60502_setI2CBypassEnabled(uint8_t enabled) {
    IIC2writeBit(MPU60502_ADDR, MPU60502_RA_INT_PIN_CFG, MPU60502_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}
/**************************实现函数********************************************
*函数原型:
*功　　能:	    设置低通滤波截止频率
*******************************************************************************/
void MPU60502_setDLPF(uint8_t mode)
{
    IIC2writeBits(MPU60502_ADDR, MPU60502_RA_CONFIG, MPU60502_CFG_DLPF_CFG_BIT, MPU60502_CFG_DLPF_CFG_LENGTH, mode);
}

void MPU60502_INT_Config()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    //	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOE, GPIO_Pin_8);

}
/**************************实现函数********************************************
*函数原型:		void MPU6050_initialize(void)
*功　　能:	    初始化 	MPU6050 以进入可用状态。
*******************************************************************************/
void MPU60502_Init(u16 lpf)
{
    u16 default_filter = 1;

    //MPU60502_INT_Config();

    switch (lpf)
    {
    case 5:
        default_filter = MPU60502_DLPF_BW_5;
        break;
    case 10:
        default_filter = MPU60502_DLPF_BW_10;
        break;
    case 20:
        default_filter = MPU60502_DLPF_BW_20;
        break;
    case 42:
        default_filter = MPU60502_DLPF_BW_42;
        break;
    case 98:
        default_filter = MPU60502_DLPF_BW_98;
        break;
    case 188:
        default_filter = MPU60502_DLPF_BW_188;
        break;
    case 256:
        default_filter = MPU60502_DLPF_BW_256;
        break;
    default:
        default_filter = MPU60502_DLPF_BW_42;
        break;
    }
    I2c2_Soft_Init();

    //设备复位
    //	IIC_Write_1Byte(MPU6050_ADDR,MPU6050_RA_PWR_MGMT_1, 0x80);

    MPU60502_setSleepEnabled(0); //进入工作状态
    delay_ms(10);
    MPU60502_setClockSource(MPU60502_CLOCK_PLL_ZGYRO); //设置时钟  0x6b   0x03
    delay_ms(10);
    MPU60502_set_SMPLRT_DIV(1000);  //1000hz
    delay_ms(10);

#if USE_ANO_PARAM
    MPU60502_setFullScaleGyroRange(MPU60502_GYRO_FS_2000);//陀螺仪最大量程 +-2000度每秒
#else
	MPU60502_setFullScaleGyroRange(MPU60502_GYRO_FS_250);//陀螺仪最大量程 +-250度每秒
#endif
    delay_ms(10);

#if USE_ANO_PARAM
	MPU60502_setFullScaleAccelRange(MPU60502_ACCEL_FS_8);	//加速度度最大量程 +-8G
#else
	MPU60502_setFullScaleAccelRange(MPU60502_ACCEL_FS_2);	//加速度度最大量程 +-2G
#endif
    delay_ms(10);

    MPU60502_setDLPF(default_filter);  //42hz
    delay_ms(10);
    MPU60502_setI2CMasterModeEnabled(0);	 //不让MPU6050 控制AUXI2C
    delay_ms(10);
    MPU60502_setI2CBypassEnabled(1);	 //主控制器的I2C与	MPU6050的AUXI2C	直通。控制器可以直接访问HMC5883L
    delay_ms(10);
}

s32 sum_temp2[7] = { 0,0,0,0,0,0,0 };
u16 acc_sum_cnt2 = 0, gyro_sum_cnt2 = 0;
void MPU60502_Data_Offset()
{
#ifdef ACC_ADJ_EN

    if (mpu60502.Acc_CALIBRATE == 1)
    {
        acc_sum_cnt2++;
        sum_temp2[A_X] += mpu60502.Acc_I16.x;
        sum_temp2[A_Y] += mpu60502.Acc_I16.y;
        //sum_temp[A_Z] += mpu6050.Acc_I16.z - 65536 / 16;   // +-8G
        sum_temp2[A_Z] += mpu60502.Acc_I16.z - 65536 / 4;   // +-2G
        sum_temp2[TEM] += mpu60502.Tempreature;

        if (acc_sum_cnt2 >= OFFSET_AV_NUM)
        {
            mpu60502.Acc_Offset.x = sum_temp2[A_X] / OFFSET_AV_NUM;
            mpu60502.Acc_Offset.y = sum_temp2[A_Y] / OFFSET_AV_NUM;
            mpu60502.Acc_Offset.z = sum_temp2[A_Z] / OFFSET_AV_NUM;
            mpu60502.Temprea_Offset = sum_temp2[TEM] / OFFSET_AV_NUM;
            acc_sum_cnt2 = 0;
            mpu6050.Acc_CALIBRATE = 0;
            Param_SaveAccelOffset(&mpu60502.Acc_Offset);
            sum_temp2[A_X] = sum_temp2[A_Y] = sum_temp2[A_Z] = sum_temp2[TEM] = 0;
        }
    }

#endif

    if (mpu60502.Gyro_CALIBRATE)
    {
        gyro_sum_cnt2++;
        sum_temp2[G_X] += mpu60502.Gyro_I16.x;
        sum_temp2[G_Y] += mpu60502.Gyro_I16.y;
        sum_temp2[G_Z] += mpu60502.Gyro_I16.z;
        sum_temp2[TEM] += mpu60502.Tempreature;

        if (gyro_sum_cnt2 >= OFFSET_AV_NUM2)
        {
            mpu60502.Gyro_Offset.x = (float)sum_temp2[G_X] / OFFSET_AV_NUM2;
            mpu60502.Gyro_Offset.y = (float)sum_temp2[G_Y] / OFFSET_AV_NUM2;
            mpu60502.Gyro_Offset.z = (float)sum_temp2[G_Z] / OFFSET_AV_NUM2;
            mpu60502.Temprea_Offset = sum_temp2[TEM] / OFFSET_AV_NUM2;
            gyro_sum_cnt2 = 0;
            if (mpu60502.Gyro_CALIBRATE == 1)
                Param_SaveGyroOffset(&mpu60502.Gyro_Offset);
            mpu60502.Gyro_CALIBRATE = 0;
            sum_temp2[G_X] = sum_temp2[G_Y] = sum_temp2[G_Z] = sum_temp2[TEM] = 0;
        }
    }
}

void Transform2(float itx, float ity, float itz, float *it_x, float *it_y, float *it_z)
{
    *it_x = itx;
    *it_y = ity;
    *it_z = itz;

}

s16 FILT_BUF2[ITEMS][(FILTER_NUM2 + 1)];
uint8_t filter_cnt2 = 0, filter_cnt_old2 = 0;

float mpu60502_tmp[ITEMS];
float tmp2[ITEMS];
float test_ang2 = 0, test_ang_old2 = 0, test_ang_d2, test_fli_a2, test_i2;

void MPU60502_Data_Prepare(float T)
{
    u8 i;
    s32 FILT_TMP2[ITEMS] = { 0,0,0,0,0,0,0 };
    //	float auto_offset_temp[3];
    float Gyro_tmp[3];

    MPU60502_Data_Offset(); //校准函数

    /*读取buffer原始数据*/
    mpu60502.Acc_I16.x = ((((int16_t)mpu60502_buffer[0]) << 8) | mpu60502_buffer[1]);
    mpu60502.Acc_I16.y = ((((int16_t)mpu60502_buffer[2]) << 8) | mpu60502_buffer[3]);
    mpu60502.Acc_I16.z = ((((int16_t)mpu60502_buffer[4]) << 8) | mpu60502_buffer[5]);

    mpu60502.Gyro_I16.x = ((((int16_t)mpu60502_buffer[8]) << 8) | mpu60502_buffer[9]);
    mpu60502.Gyro_I16.y = ((((int16_t)mpu60502_buffer[10]) << 8) | mpu60502_buffer[11]);
    mpu60502.Gyro_I16.z = ((((int16_t)mpu60502_buffer[12]) << 8) | mpu60502_buffer[13]);

    Gyro_tmp[0] = mpu60502.Gyro_I16.x;//
    Gyro_tmp[1] = mpu60502.Gyro_I16.y;//
    Gyro_tmp[2] = mpu60502.Gyro_I16.z;//

    mpu60502.Tempreature = ((((int16_t)mpu60502_buffer[6]) << 8) | mpu60502_buffer[7]); //tempreature
    mpu60502.TEM_LPF += 2 * 3.14f *T *(mpu60502.Tempreature - mpu60502.TEM_LPF);
    mpu60502.Ftempreature = mpu60502.TEM_LPF / 340.0f + 36.5f;

    //======================================================================
    if (++filter_cnt2 > FILTER_NUM2)
    {
        filter_cnt2 = 0;

    }
    else
    {
        filter_cnt_old2 = (filter_cnt2 == FILTER_NUM2) ? 0 : (filter_cnt2 + 1);
    }
    //10 170 4056
    /* 得出校准后的数据 */
    mpu60502_tmp[A_X] = (mpu60502.Acc_I16.x - mpu60502.Acc_Offset.x);
    mpu60502_tmp[A_Y] = (mpu60502.Acc_I16.y - mpu60502.Acc_Offset.y);
    mpu60502_tmp[A_Z] = (mpu60502.Acc_I16.z - mpu60502.Acc_Offset.z);
    mpu60502_tmp[G_X] = Gyro_tmp[0] - mpu60502.Gyro_Offset.x;//
    mpu60502_tmp[G_Y] = Gyro_tmp[1] - mpu60502.Gyro_Offset.y;//
    mpu60502_tmp[G_Z] = Gyro_tmp[2] - mpu60502.Gyro_Offset.z;//


    /* 更新滤波滑动窗口数组 */
    FILT_BUF2[A_X][filter_cnt2] = mpu60502_tmp[A_X];
    FILT_BUF2[A_Y][filter_cnt2] = mpu60502_tmp[A_Y];
    FILT_BUF2[A_Z][filter_cnt2] = mpu60502_tmp[A_Z];
    FILT_BUF2[G_X][filter_cnt2] = mpu60502_tmp[G_X];
    FILT_BUF2[G_Y][filter_cnt2] = mpu60502_tmp[G_Y];
    FILT_BUF2[G_Z][filter_cnt2] = mpu60502_tmp[G_Z];

    for (i = 0; i<FILTER_NUM2; i++)
    {
        FILT_TMP2[A_X] += FILT_BUF2[A_X][i];
        FILT_TMP2[A_Y] += FILT_BUF2[A_Y][i];
        FILT_TMP2[A_Z] += FILT_BUF2[A_Z][i];
        FILT_TMP2[G_X] += FILT_BUF2[G_X][i];
        FILT_TMP2[G_Y] += FILT_BUF2[G_Y][i];
        FILT_TMP2[G_Z] += FILT_BUF2[G_Z][i];
    }


    tmp2[A_X] = (float)(FILT_TMP2[A_X]) / (float)FILTER_NUM2;
    tmp2[A_Y] = (float)(FILT_TMP2[A_Y]) / (float)FILTER_NUM2;
    tmp2[A_Z] = (float)(FILT_TMP2[A_Z]) / (float)FILTER_NUM2;


    tmp2[G_X] = (float)(FILT_TMP2[G_X]) / (float)FILTER_NUM2;
    tmp2[G_Y] = (float)(FILT_TMP2[G_Y]) / (float)FILTER_NUM2;
    tmp2[G_Z] = (float)(FILT_TMP2[G_Z]) / (float)FILTER_NUM2;


    /*坐标转换*/
    Transform2(tmp2[A_X], tmp2[A_Y], tmp2[A_Z], &mpu60502.Acc.x, &mpu60502.Acc.y, &mpu60502.Acc.z);
    Transform2(tmp2[G_X], tmp2[G_Y], tmp2[G_Z], &mpu60502.Gyro.x, &mpu60502.Gyro.y, &mpu60502.Gyro.z);

	mpu60502.Gyro_deg.x = ((float)mpu60502.Gyro.x / GyroAxis_Sensitive2);
	mpu60502.Gyro_deg.y = ((float)mpu60502.Gyro.y / GyroAxis_Sensitive2);
	mpu60502.Gyro_deg.z = ((float)mpu60502.Gyro.z / GyroAxis_Sensitive2);

	mpu60502.Acc_deg.x = ((float)mpu60502.Acc.x / AccAxis_Sensitive2);
	mpu60502.Acc_deg.y = ((float)mpu60502.Acc.y / AccAxis_Sensitive2);
	mpu60502.Acc_deg.z = ((float)mpu60502.Acc.z / AccAxis_Sensitive2);
    //======================================================================
}


/**********************************************
* 陀螺仪矫正
**********************************************/
void MPU60502_GYRO_Calibrate(void)
{
    mpu60502.Gyro_CALIBRATE = 1;
}

/**********************************************
* 加速度计矫正
**********************************************/
void MPU60502_ACC_Calibrate(void)
{
    mpu60502.Acc_CALIBRATE = 1;
}


/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/

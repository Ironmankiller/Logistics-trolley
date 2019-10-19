#include <stdint.h>
#include "Sensor_Param.h"
#include <string.h>
#include "BSP\MPU6050\MPU6050.h"


static sensor_setup_t sensor_setup;

void Sensor_Param_Init(void)
{
    memset(&sensor_setup, 0, sizeof(sensor_setup_t));
}

void Param_SaveAccelOffset(xyz_f_t *offset)
{
    memcpy(&mpu6050.Acc_Offset, offset, sizeof(xyz_f_t));
    memcpy(&sensor_setup.Offset.Accel, offset, sizeof(xyz_f_t));
    //Para_WriteSettingToFile();
}

void Param_SaveGyroOffset(xyz_f_t *offset)
{
    memcpy(&mpu6050.Gyro_Offset, offset, sizeof(xyz_f_t));
    memcpy(&sensor_setup.Offset.Gyro, offset, sizeof(xyz_f_t));
    //Para_WriteSettingToFile();
}

void Param_SaveMagOffset(xyz_f_t *offset)
{
    //memcpy(&ak8975.Mag_Offset,offset,sizeof(xyz_f_t));
    //memcpy(&sensor_setup.Offset.Mag, offset,sizeof(xyz_f_t));
    //Para_WriteSettingToFile();
}

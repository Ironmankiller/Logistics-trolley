#ifndef _SENSOR_PARAM_H
#define	_SENSOR_PARAM_H

#include <stdint.h>

typedef struct
{
    float x;
    float y;
    float z;
}xyz_f_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;

}xyz_s16_t;

typedef union
{
	uint8_t raw_data[64];
	struct
	{
		xyz_f_t Accel;
		xyz_f_t Gyro;
		xyz_f_t Mag;
		uint32_t Baro;
		float Temperature;
	}Offset;
}sensor_setup_t;


void Sensor_Param_Init(void);
void Param_SaveAccelOffset(xyz_f_t *offset);
void Param_SaveGyroOffset(xyz_f_t *offset);
void Param_SaveMagOffset(xyz_f_t *offset);

#endif

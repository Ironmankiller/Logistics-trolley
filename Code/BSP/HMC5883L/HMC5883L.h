#ifndef __HMC5883L_H
#define __HMC5883L_H

#include "stm32f4xx.h"
#include "BSP\HMC5883L\IOI2C.h"
#include "delay.h"

#define HMC58X3_ADDR 0x3C // 7 bit address of the HMC58X3 used with the Wire library
#define HMC_POS_BIAS 1
#define HMC_NEG_BIAS 2
// HMC58X3 register map. For details see HMC58X3 datasheet
#define HMC58X3_R_CONFA 0
#define HMC58X3_R_CONFB 1
#define HMC58X3_R_MODE 2
#define HMC58X3_R_XM (3)  //!< Register address for XM.
#define HMC58X3_R_XL (4)  //!< Register address for XL.
#define HMC58X3_R_YM (7)  //!< Register address for YM.
#define HMC58X3_R_YL (8)  //!< Register address for YL.
#define HMC58X3_R_ZM (5)  //!< Register address for ZM.
#define HMC58X3_R_ZL (6)  //!< Register address for ZL.

#define HMC58X3_R_STATUS 9
#define HMC58X3_R_IDA 10
#define HMC58X3_R_IDB 11
#define HMC58X3_R_IDC 12

typedef struct 
{
    int16_t x;
    int16_t y;
    int16_t z;
    float yaw;
}hmc5883_typedef;

void HMC5883L_SetUp(void);	//³õÊ¼»¯
void HMC58X3_getID(char id[3]);	//¶ÁÐ¾Æ¬ID
void HMC58X3_getRaw(int16_t *x, int16_t *y, int16_t *z);  //¶ÁÈ¡

#endif

//------------------End of File----------------------------




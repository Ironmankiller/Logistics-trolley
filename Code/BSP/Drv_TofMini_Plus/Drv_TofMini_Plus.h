#ifndef _DRV_TOFMINI_PLUS_H_
#define _DRV_TOFMINI_PLUS_H_

#include "include.h"


extern u8 TofMiniPlus_LINKOK;
extern u16 TofMiniPlus_height_cm;

u8 Drv_TofMiniPlus_Init(void);
void Drv_TofMiniPlus_RunTask(u8 data);


#endif

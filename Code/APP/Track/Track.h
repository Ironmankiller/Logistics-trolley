#ifndef __TRACK_H_
#define __TRACK_H_
#include "sys.h"
#include "stm32f4xx.h"

void Track_Init(void);

void Track_Read(void);

u16 Track_Read_2Bytes(void);

#endif /* __TRACK_H_ */

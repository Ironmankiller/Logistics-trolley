#ifndef __OPENMV_H
#define __OPENMV_H

#include <stdint.h>

//Openmv 数据接收处理
void OpenMV_Data_Handle(char * data_buf, uint16_t len);
extern int center;
#endif // !__OPENMV_H

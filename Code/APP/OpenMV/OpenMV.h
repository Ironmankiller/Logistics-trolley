#ifndef __OPENMV_H
#define __OPENMV_H

#include <stdint.h>
#include "sys.h"

//Openmv ���ݽ��մ���
void OpenMV_Data_Handle(vu8 * data_buf);
void OpenMV_Read_Grab(void);
void OpenMV_Read_Color(void);
#endif // !__OPENMV_H

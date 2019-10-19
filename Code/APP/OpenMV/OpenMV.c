#include "OpenMV.h"
#include "My_Flag.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "APP\Control\Control.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
//Openmv 数据接收处理
int center = 116;
void OpenMV_Data_Handle(char * data_buf, uint16_t len)
{
    if (((*(data_buf) < '0') || (*(data_buf) > '9') )&& (*(data_buf)!= '-') && (*(data_buf) != 'N'))
    {
        return;
    }
    else if (*(data_buf) == 'N')
    {
        return;
    }

    memset(data_buf, 0, len);
}




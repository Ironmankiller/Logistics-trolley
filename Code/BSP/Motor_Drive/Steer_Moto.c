#include "BSP\Motor_Drive\Steer_Moto.h"
#include "BSP/MCU/UART/usart2.h"
#include <stdio.h>
#include <string.h>
char buf[10];

void setmotor_pwm(unsigned short num,unsigned short pwm,unsigned short time)
{
	sprintf(buf,"#%dP%dT%d\r\n",num,pwm,time);
	Usart_SendString(USART1,buf);
}


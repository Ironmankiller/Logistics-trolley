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

//面向抓取区域从左到右依次为1、2、3
void arm_grab(int num, int domain)
{
    if(num == 1){
        arm1(75);
    }
    if(num == 2){
        arm1(90);
    }
    if(num == 3){
        arm1(105);
    }
}

//面向加工区从左到右依次为1、2、3
void arm_place(int num){
}

//机械臂转过一定角度
void arm_turn(float degree){
}

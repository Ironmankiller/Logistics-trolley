#include "BSP\OLED\OLED.h"
#include "BSP\Time\Time.h"
#include "APP\IMU\ahrs.h"
#include <stdio.h>
#include "APP\Control\Control.h"
#include "BSP\Encoder\Encoder.h"
#include "APP\Control\param.h"
#include "BSP\Mecanum\Mecanum.h"

/**********************************************
* 显示时间
**********************************************/
void Display_TIME(void)
{

    OLED_ShowString(10, 10, "Minute: ");
    OLED_ShowString(10, 30, "Second: ");
    OLED_ShowString(10, 50, "MillSe: ");

    OLED_ShowNumber(60, 10, System_Time.Minutes, 3, 12);
    OLED_ShowNumber(60, 30, System_Time.Seconds, 3, 12);
    OLED_ShowNumber(60, 50, System_Time.Milliseconds, 3, 12);

    //int32_t enc = 0;
    //enc = Read_Encoder(2);

    //OLED_ShowNumber(60, 0, enc, 3, 12);

    OLED_Refresh_Gram();
}


void Display_IMU(void)
{
    OLED_ShowString(10, 30, "Minute: ");
    OLED_ShowString(10, 40, "Second: ");
    OLED_ShowString(10, 50, "MillSe: ");

    OLED_ShowNumber(60, 30, System_Time.Minutes, 3, 12);
    OLED_ShowNumber(60, 40, System_Time.Seconds, 3, 12);
    OLED_ShowNumber(60, 50, System_Time.Milliseconds, 3, 12);

	OLED_ShowString(10, 0, "Pitch: ");
	OLED_ShowString(10, 10, "Roll : ");
	//OLED_ShowString(10, 30, "Ano-P: ");
	//OLED_ShowString(10, 40, "Ano-R: ");
	//OLED_ShowString(10, 50, "Ano-Y: ");

	OLED_ShowFloat(60, 0, EulerAngle.Pitch);
	OLED_ShowFloat(60, 10, EulerAngle.Roll);
	//OLED_ShowFloat(60, 30, EulerAngle.Pitch);
	//OLED_ShowFloat(60, 40, EulerAngle.Roll);
	//OLED_ShowFloat(60, 50, EulerAngle.Yaw);

    OLED_Refresh_Gram();
}

///**********************************************
//* 显示ADC
//**********************************************/
//void Display_ADC(uint16_t adc)
//{
//    OLED_ShowString(10, 10, "ADC: ");
//    OLED_ShowNumber(60, 10, adc, 6, 12);
//    OLED_Refresh_Gram();
//}

///**********************************************
//* 显示倒立摆参数
//**********************************************/
//void Display_Handstand(void)
//{
//    //=============第1行显示角度PD控制P参数Position_KP=======================//	
//    OLED_ShowString(00, 00, "A-KP");
//    OLED_ShowNumber(40, 00, pidangleparam[0], 4, 12);
//    OLED_ShowString(63, 00, ".");
//    OLED_ShowNumber(69, 00, (int)(pidangleparam[0] * 10) % 10, 1, 12);
//    OLED_ShowNumber(75, 00, (int)(pidangleparam[0] * 100) % 10, 1, 12);

//    //=============第2行显示角度PD控制D参数=======================//	
//    OLED_ShowString(00, 10, "A-KD");
//    OLED_ShowNumber(40, 10, pidangleparam[2], 3, 12);
//    OLED_ShowString(57, 10, ".");
//    OLED_ShowNumber(63, 10, (int)(pidangleparam[2] * 10) % 10, 1, 12);
//    OLED_ShowNumber(69, 10, (int)(pidangleparam[2] * 100) % 10, 1, 12);

//    //=============第3行显示编码器Position_KP=======================//	
//    OLED_ShowString(00, 20, "S-KP");
//    OLED_ShowNumber(40, 20, pidspeedparam[0], 3, 12);
//    OLED_ShowString(57, 20, ".");
//    OLED_ShowNumber(63, 20, (int)(pidspeedparam[0] * 10) % 10, 1, 12);
//    OLED_ShowNumber(69, 20, (int)(pidspeedparam[0] * 100) % 10, 1, 12);


//    //=============第4行显示编码器1=======================//	
//    OLED_ShowString(00, 30, "S-KI");
//    OLED_ShowNumber(40, 30, pidspeedparam[1], 3, 12);
//    OLED_ShowString(57, 30, ".");
//    OLED_ShowNumber(63, 30, (int)(pidspeedparam[1] * 10) % 10, 1, 12);
//    OLED_ShowNumber(69, 30, (int)(pidspeedparam[1] * 100) % 10, 1, 12);
//	OLED_ShowString(00, 40, "A-KI");
//    //=============第五行显示电压和目标位置=======================//			
//    OLED_ShowString(0, 40, "state:");
//    if(Car.state == forward)
//        OLED_ShowString(50, 40, "forward");
//    else if(Car.state == back)
//        OLED_ShowString(50, 40, "back   ");
//    else if(Car.state == left)
//        OLED_ShowString(50, 40, "left   ");
//    else if(Car.state == right)
//        OLED_ShowString(50, 40, "right  ");
//    else if(Car.state == stop)
//        OLED_ShowString(50, 40, "stop   ");

//    //=============第六行显示角位移传感器和编码器数据=======================//
//    OLED_ShowString(60, 50, "spe:");  OLED_ShowNumber(75, 50, Car.velocity, 4, 12);
//    OLED_ShowString(0, 50, "ang:");
//    OLED_ShowNumber(25, 50, Car.angle, 3, 12);
//    OLED_ShowString(42, 50, ".");
//    OLED_ShowNumber(48, 50, (int)(Car.angle * 10) % 10, 1, 12);

//    //=============刷新=======================//
//    OLED_Refresh_Gram();
//}

void Display_Mecanum()
{
	//=============第1行显示3轴角度===============//	
  	OLED_ShowString(0,0,"X:");
	if(EulerAngle.Pitch<0)		OLED_ShowNumber(15,0,EulerAngle.Pitch+360,3,12);
	else			OLED_ShowNumber(15,0,EulerAngle.Pitch,3,12);	
       
  	OLED_ShowString(40,0,"Y:");
	if(EulerAngle.Roll<0)		OLED_ShowNumber(55,0,EulerAngle.Roll+360,3,12);
	else			OLED_ShowNumber(55,0,EulerAngle.Roll,3,12);	
	
	OLED_ShowString(80,0,"Z:");
	if(EulerAngle.Yaw<0)		OLED_ShowNumber(95,0,EulerAngle.Yaw+360,3,12);
	else		    OLED_ShowNumber(95,0,EulerAngle.Yaw,3,12);		

	    //=============显示电机A的状态=======================//	
	    if(Mecanum.Target_A<0)  OLED_ShowString(00,10,"-"),
		                        OLED_ShowNumber(15,10,-Mecanum.Target_A,5,12);
		else                    OLED_ShowString(0,10,"+"),
		                        OLED_ShowNumber(15,10, Mecanum.Target_A,5,12); 
		
		if(Mecanum.Encoder_A<0) OLED_ShowString(80,10,"-"),
		                        OLED_ShowNumber(95,10,-Mecanum.Encoder_A,4,12);
		else                 	OLED_ShowString(80,10,"+"),
		                        OLED_ShowNumber(95,10, Mecanum.Encoder_A,4,12);
		//=============显示电机B的状态=======================//	
		if(Mecanum.Target_B<0)  OLED_ShowString(00,20,"-"),
		                        OLED_ShowNumber(15,20,-Mecanum.Target_B,5,12);
		else                 	OLED_ShowString(0,20,"+"),
		                        OLED_ShowNumber(15,20, Mecanum.Target_B,5,12); 
		  
		if(Mecanum.Encoder_B<0) OLED_ShowString(80,20,"-"),
		                        OLED_ShowNumber(95,20,-Mecanum.Encoder_B,4,12);
		else                 	OLED_ShowString(80,20,"+"),
		                        OLED_ShowNumber(95,20, Mecanum.Encoder_B,4,12);
 		//=============显示电机C的状态=======================//	
		if(Mecanum.Target_C<0)  OLED_ShowString(00,30,"-"),
		                        OLED_ShowNumber(15,30,-Mecanum.Target_C,5,12);
		else                 	OLED_ShowString(0,30,"+"),
		                        OLED_ShowNumber(15,30, Mecanum.Target_C,5,12); 
		  
		if(Mecanum.Encoder_C<0) OLED_ShowString(80,30,"-"),
		                        OLED_ShowNumber(95,30,-Mecanum.Encoder_C,4,12);
		else                 	OLED_ShowString(80,30,"+"),
		                        OLED_ShowNumber(95,30, Mecanum.Encoder_C,4,12);	
		//=============显示电机D的状态=======================//	
		if(Mecanum.Target_D<0)  OLED_ShowString(00,40,"-"),
		                        OLED_ShowNumber(15,40,-Mecanum.Target_D,5,12);
		else                 	OLED_ShowString(0,40,"+"),
		                        OLED_ShowNumber(15,40, Mecanum.Target_D,5,12); 
		
		if(Mecanum.Encoder_D<0) OLED_ShowString(80,40,"-"),
		                        OLED_ShowNumber(95,40,-Mecanum.Encoder_D,4,12);
		else                 	OLED_ShowString(80,40,"+"),
		                        OLED_ShowNumber(95,40, Mecanum.Encoder_D,4,12);
		//=============第五行显示电压=======================//

		//=============刷新=======================//
		OLED_Refresh_Gram();	
	}

/**********************************************
* 显示编码器读数
**********************************************/
void Display_Encoder(int32_t enc1, int32_t enc2)
{
    OLED_ShowString(20, 10, "Enc1: ");
    OLED_ShowString(50, 10, "     ");   OLED_ShowInt(50, 10, enc1);
    OLED_ShowString(20, 20, "Enc2: ");
    OLED_ShowString(50, 20, "     ");   OLED_ShowInt(50, 20, enc2);
    OLED_Refresh_Gram();
}

/**********************************************
* 显示错误信息
**********************************************/
void Display_Error(void)
{
    OLED_ShowString(10, 30, "I'm Dead ~");
    OLED_Refresh_Gram();
}

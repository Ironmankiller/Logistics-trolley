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
	//=============第1行显示显示扫到的颜色===============//	
  	OLED_ShowString(0,0,"color:");
    if(Mecanum.color_order_ready == MY_TRUE) {
        switch(Mecanum.Color){
            case RGB: OLED_ShowString(40,0,"RGB"); break;
            case RBG: OLED_ShowString(40,0,"RBG"); break;
            case GBR: OLED_ShowString(40,0,"GBR"); break;
            case GRB: OLED_ShowString(40,0,"GRB"); break;
            case BRG: OLED_ShowString(40,0,"BRG"); break;
            case BGR: OLED_ShowString(40,0,"BGR"); break;
        }
    } else {
        OLED_ShowString(40,0,"   ");
    }
    
    //=============第2行显示显示二维码标识的顺序===============//	
    OLED_ShowString(0,10,"grab:");
    if(Mecanum.grab_order_ready == MY_TRUE) {
        switch(Mecanum.Grab){
            case RGB: OLED_ShowString(40,10,"RGB"); break;
            case RBG: OLED_ShowString(40,10,"RBG"); break;
            case GBR: OLED_ShowString(40,10,"GBR"); break;
            case GRB: OLED_ShowString(40,10,"GRB"); break;
            case BRG: OLED_ShowString(40,10,"BRG"); break;
            case BGR: OLED_ShowString(40,10,"BGR"); break;
        }
    } else {
        OLED_ShowString(40,10,"   ");
    }

    //=============第3显示当前运行状态===============//
    OLED_ShowString(0,20,"state:");
    switch(Mecanum.state){
        case ready: OLED_ShowString(40,20,"ready    "); break;
        case scan: OLED_ShowString(40,20,"scan     "); break;
        case goToDeparture: OLED_ShowString(40,20,"goToD    "); break;
        case grabFromDeparture: OLED_ShowString(40,20,"grabFromD"); break;
        case goToProcessing: OLED_ShowString(40,20,"goToP    "); break;
        case placeToProcessing: OLED_ShowString(40,20,"placeToP "); break;
        case backToDeparture: OLED_ShowString(40,20,"backToD  "); break;
        case grabFromProcessing: OLED_ShowString(40,20,"grabFromP"); break;
        case goToFinish: OLED_ShowString(40,20,"goToF    "); break;
        case placeToFinish: OLED_ShowString(40,20,"placeToF "); break;
        case backToProcessing: OLED_ShowString(40,20,"backToP  "); break;
        case back: OLED_ShowString(40,20,"back     "); break;
    }
    
    //=============第4显示当前位置坐标===============//
    OLED_ShowString(0,30,"X:");
    OLED_ShowNumber(30,30,Mecanum.X_Length,3,12);
    OLED_ShowString(60,30,"Y:");
    OLED_ShowNumber(90,30,Mecanum.Y_Length,3,12);
    
    //=============第4显示当前位置坐标===============//
    OLED_ShowString(0,40,"AB:");
    OLED_ShowNumber(20,40,Mecanum.side_AB,2,12);
    OLED_ShowString(40,40,"BC:");
    OLED_ShowNumber(60,40,Mecanum.side_BC,2,12);
    OLED_ShowString(80,40,"CD:");
    OLED_ShowNumber(100,40,Mecanum.side_CD,2,12);
    OLED_ShowString(0,50,"DA:");
    OLED_ShowNumber(20,50,Mecanum.side_DA,2,12);
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

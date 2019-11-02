#include "OpenMV.h"
#include "My_Flag.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "APP\Control\Control.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#include "BSP\Mecanum\Mecanum.h"
#include "BSP\BEEP\BEEP.h"
#include "BSP\MCU\UART\usart2.h"
#include "BSP\MCU\SysTick\SysTick.h"
#define bgr 1
#define rbg 2
#define grb 3
#define brg 4
#define gbr 5
#define rgb 6

void OpenMV_Read_Color(void) {        //读取物料摆放顺序
    u2_printf("OK\r\n");
    delay_ms(5);
    u2_printf("WL\r\n");  //物料
}

void OpenMV_Read_Grab(void) {         //读取二维码
    u2_printf("OK\r\n");
    delay_ms(5);
    u2_printf("EW\r\n");
}



//Openmv 数据接收处理
void OpenMV_Data_Handle(vu8 * data_buf)
{
    static u8 Color = 0;              //从右往左的摆放顺序
    static u8 Grab = 0;               //颜色的抓取顺序
    if(data_buf[0] == 'W'){
        if(data_buf[1]=='1'&&data_buf[2]=='2'&&data_buf[3]=='3') {Color = rgb; Mecanum.Color = RGB; Mecanum.color_order_ready = MY_TRUE;}
        if(data_buf[1]=='1'&&data_buf[2]=='3'&&data_buf[3]=='2') {Color = rbg; Mecanum.Color = RBG; Mecanum.color_order_ready = MY_TRUE;}
        if(data_buf[1]=='2'&&data_buf[2]=='1'&&data_buf[3]=='3') {Color = grb; Mecanum.Color = GRB; Mecanum.color_order_ready = MY_TRUE;}
        if(data_buf[1]=='2'&&data_buf[2]=='3'&&data_buf[3]=='1') {Color = gbr; Mecanum.Color = GBR; Mecanum.color_order_ready = MY_TRUE;}
        if(data_buf[1]=='3'&&data_buf[2]=='1'&&data_buf[3]=='2') {Color = brg; Mecanum.Color = BRG; Mecanum.color_order_ready = MY_TRUE;}
        if(data_buf[1]=='3'&&data_buf[2]=='2'&&data_buf[3]=='1') {Color = bgr; Mecanum.Color = BGR; Mecanum.color_order_ready = MY_TRUE;}
        //Beep_Play(100);
    } else if(data_buf[0]=='1'||data_buf[0]=='2'||data_buf[0]=='3'){
        if(data_buf[0]=='1'&&data_buf[1]=='2'&&data_buf[2]=='3') {Grab = rgb; Mecanum.Place_Order = rcl; Mecanum.Grab = RGB;}
        if(data_buf[0]=='1'&&data_buf[1]=='3'&&data_buf[2]=='2') {Grab = rbg; Mecanum.Place_Order = rlc; Mecanum.Grab = RBG;}
        if(data_buf[0]=='2'&&data_buf[1]=='1'&&data_buf[2]=='3') {Grab = grb; Mecanum.Place_Order = crl; Mecanum.Grab = GRB;}
        if(data_buf[0]=='2'&&data_buf[1]=='3'&&data_buf[2]=='1') {Grab = gbr; Mecanum.Place_Order = clr; Mecanum.Grab = GBR;}
        if(data_buf[0]=='3'&&data_buf[1]=='1'&&data_buf[2]=='2') {Grab = brg; Mecanum.Place_Order = lrc; Mecanum.Grab = BRG;}
        if(data_buf[0]=='3'&&data_buf[1]=='2'&&data_buf[2]=='1') {Grab = bgr; Mecanum.Place_Order = lcr; Mecanum.Grab = BGR;}
        if(Color!=0&&Grab!=0) {
            if(Color==Grab) Mecanum.Departure_Order = rcl;
            else if(Color+Grab==7) Mecanum.Departure_Order = lcr;
            else if(Color==rgb&&Grab==rbg||Color==rbg&&Grab==rgb||Color==bgr&&Grab==brg||Color==brg&&Grab==bgr||Color==grb&&Grab==gbr||Color==gbr&&Grab==grb) Mecanum.Departure_Order = rlc;
            else if(Color==rgb&&Grab==grb||Color==grb&&Grab==rgb||Color==bgr&&Grab==gbr||Color==gbr&&Grab==bgr||Color==grb&&Grab==rgb||Color==rgb&&Grab==grb) Mecanum.Departure_Order = crl;
            else if(Color==rgb&&Grab==brg||Color==brg&&Grab==gbr||Color==bgr&&Grab==rbg||Color==rbg&&Grab==bgr||Color==grb&&Grab==bgr||Color==bgr&&Grab==grb) Mecanum.Departure_Order = lrc;
            else if(Color==rgb&&Grab==gbr||Color==gbr&&Grab==rgb||Color==bgr&&Grab==grb||Color==grb&&Grab==bgr||Color==grb&&Grab==rbg||Color==rbg&&Grab==grb) Mecanum.Departure_Order = crl;
            //Beep_Play(100);
            Mecanum.grab_order_ready = MY_TRUE;
        }
        Color = 0;
        Grab = 0;
    } else if(data_buf[0]=='4'){
        Beep_Play(500);
        delay_ms(500);
        
    }
    
    //memset(data_buf, 0, len);
}




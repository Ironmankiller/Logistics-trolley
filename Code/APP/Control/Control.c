#include "BSP\Motor_Drive\Moto_Drive.h"
#include "BSP\Encoder\Encoder.h"
#include "BSP\MCU\ADC\ADC.h"
#include "BSP\Motor_Drive\Steer_Moto.h"
#include "My_Flag.h"
#include "BSP\KEY\KEY.h"
#include "BSP\Time\Time.h"
#include <stdio.h>
#include "APP\Control\pid.h"
#include "APP\Control\control.h"
#include "forward.h"
#include "param.h"
#include "Algorithm.h"
#include "BSP\SYSTEM\My_Flag.h"
#include "APP\Math\My_Math.h"
#include "BSP\MCU\UART\usart3.h"
#include "APP\Math\Vect\Vect.h"
#include "BSP\LED\LED.h"
#include "BSP\Mecanum\Mecanum.h"
#include "BSP\MCU\UART\usart2.h"
#include "BSP\BEEP\BEEP.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "delay.h"
#include "math.h"
#include "APP\Track\Track.h"

#define RIGHT 2
#define CENTER 1
#define LEFT 3

Flag Init_flag = MY_TRUE; //初始化标志位
u8 step=0;                //当前搬运第几个五块
u8 order[3];              //搬运顺序
float Target_Velocity = 0;//小车目标速度
#define Z -2.2           //抵消小车的轴向旋转
/************************************************************************
Car Control
************************************************************************/
static void grab_order_set(u8 *order);                         //设置抓取顺序
static void put_order_set(u8 *order);                          //设置放置顺序
static void stop_control(u8 *Flag_Run);                                //小车停止
static void goToXY_control(int x,int y,u8 *Flag_Run);          //XY方向
static void calibrateXY_control(float x, float y, u8 *Flag_Run);//感光条校准
static void grab_hand(int16_t *grab);                          //按照传入的参数控制机械臂抓取
static void turn_hand(int16_t *turn);                          //按照传入的参数控制机械臂转向
static void place_hand(int16_t * place);                       //控制机械臂放置    

void ready_control(void){

    if(Init_flag == MY_TRUE) {
        //grab_hand(grab_center);
        //turn_hand(turn_departure);
        //place_hand(place_processing);
        
        Init_flag = MY_FALSE;
    }
    if(PS2_ON_Flag == MY_TRUE)
        PS2_Usart_Control();             //PS2手柄控制小车，如果已经初始化PS2则执行
    //calibrateXY_control(7.5,7.5,&Flag_Run);
    //stop_control(&Flag_Run);
    //grab_hand(grab_right);
    Kinematic_Analysis(0,0,Z);           //小车运动学分析
    pid_setup_mecanum_speed();           //小车各轮子速度期望
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);   //pid控制
}

void goToDeparture_control(void) {
    
    static u8 Flag_Run=0;         //标识当前运行阶段
 
    if(Flag_Run == 0) {
        goToXY_control(109,30, &Flag_Run); 
    } else if(Flag_Run == 1) {
        goToXY_control(109,4,&Flag_Run);
    } else if(Flag_Run == 2) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3){
        //calibrateXY_control(7.5f,6.0f,&Flag_Run);
        stop_control(&Flag_Run);
    } else if(Flag_Run == 4) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 5) {
        Init_flag = MY_TRUE;
        Mecanum.state = grab;
        Flag_Run = 0;
    }
}

void grab_control(void) {
    
    if(Init_flag == MY_TRUE) {
        grab_order_set(order);
        Init_flag = MY_FALSE;
    }
    MOTO_A_Set(0);
    MOTO_B_Set(0);
    MOTO_C_Set(0);
    MOTO_D_Set(0);
    if(order[step] == RIGHT){
        grab_hand(grab_right);
    } else if(order[step] == LEFT){
        grab_hand(grab_left);
    } else if(order[step] == CENTER){
        grab_hand(grab_center);
    }

    Mecanum.state = goToDestination;
    Init_flag = MY_TRUE;
}



void goToDestination_control(void) {
    
    static u8 Flag_Run=0;

    if(Init_flag == MY_TRUE){
        put_order_set(order);
        Init_flag = MY_FALSE;
        turn_hand(turn_destination);
    }

    if(Flag_Run == 0) {
        switch(order[step]){
            case RIGHT:
                goToXY_control(136,181,&Flag_Run);
                break;
            case LEFT:
                goToXY_control(83,181,&Flag_Run);
                break;
            case CENTER:
                goToXY_control(109,163,&Flag_Run);
                break;
        }
    } else if(Flag_Run == 1) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 2) {
        Mecanum.state = place;
        Flag_Run = 0;
        step++;
    }
}

void place_control(void) {
    
    MOTO_A_Set(0);
    MOTO_B_Set(0);
    MOTO_C_Set(0);
    MOTO_D_Set(0);
    place_hand(place_processing);
    
    if(step == 3){
        Mecanum.state = back;
    }
    else {
        Mecanum.state = backToDeparture;
    }
    Init_flag = MY_TRUE;
}

void backToDeparture_control(void) {
    
    static u8 Flag_Run=0;
    if(Init_flag == MY_TRUE){
        turn_hand(turn_departure);
        Init_flag = MY_FALSE;
    }

    if(Flag_Run == 0) {
        goToXY_control(109,4,&Flag_Run);
    } else if(Flag_Run == 1) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 2) {
        //calibrateXY_control(7.5f,6.0f,&Flag_Run);
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 4) {
        Mecanum.state = grab;
        Init_flag = MY_TRUE;
        Flag_Run = 0;
    }       
}

void back_control(void) {
    
    static u8 Flag_Run=0;
    if(Init_flag == MY_TRUE){
        Init_flag = MY_FALSE;
        turn_hand(turn_departure);
    }
    
    if(Flag_Run == 0) {
        goToXY_control(8,4,&Flag_Run);
    } else if(Flag_Run == 1) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 2) {
        Mecanum.state = ready;
        Flag_Run = 0;
    }
    
}

static void stop_control(u8 *Flag_Run) {
    
    static u8 counter=0;
    if(Mecanum.Encoder_A==0&&Mecanum.Encoder_B==0&&Mecanum.Encoder_C==0&&Mecanum.Encoder_D==0) {
        counter++;
        if(counter==10){
            counter = 0;
            MOTO_A_Set(0);
            MOTO_B_Set(0);
            MOTO_C_Set(0);
            MOTO_D_Set(0);
            //Beep_Play(100); 
            (*Flag_Run)++;
        }
    } else {
        counter = 0;
    }
    Kinematic_Analysis(0,0,Z);           //小车运动学分析
    pid_setup_mecanum_speed();           //小车各轮子速度期望
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);   //pid控制
}

static void put_order_set(u8 *order) {
    switch(Mecanum.Destination_Order) {
        case rcl: order[0] = RIGHT; order[1] = CENTER; order[2] = LEFT; break;
        case rlc: order[0] = RIGHT; order[1] = LEFT; order[2] = CENTER; break;
        case lrc: order[0] = LEFT; order[1] = RIGHT; order[2] = CENTER; break;
        case lcr: order[0] = LEFT; order[1] = CENTER; order[2] = RIGHT; break;
        case clr: order[0] = CENTER; order[1] = LEFT; order[2] = RIGHT; break;
        case crl: order[0] = CENTER; order[1] = RIGHT; order[2] = LEFT; break;
    }
}

static void grab_order_set(u8 *order) {
    switch(Mecanum.Departure_Order) {
        case rcl: order[0] = RIGHT; order[1] = CENTER; order[2] = LEFT; break;
        case rlc: order[0] = RIGHT; order[1] = LEFT; order[2] = CENTER; break;
        case lrc: order[0] = LEFT; order[1] = RIGHT; order[2] = CENTER; break;
        case lcr: order[0] = LEFT; order[1] = CENTER; order[2] = RIGHT; break;
        case clr: order[0] = CENTER; order[1] = LEFT; order[2] = RIGHT; break;
        case crl: order[0] = CENTER; order[1] = RIGHT; order[2] = LEFT; break;
    }
}
static void goToXY_control(int x,int y,u8 *Flag_Run) {
    
    float D,Dx,Dy;
    
    Dy = y - Mecanum.Y_Length;
    Dx = x - Mecanum.X_Length;

    D = sqrt((double)(Dx*Dx + Dy*Dy));
    
    if(D>50){
        Target_Velocity=40;
    }
    else if(D>15){
        Target_Velocity=D - 10.0f;
    }
    else {
        Target_Velocity=5; 
    } 
    
//    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.3f;
//        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity-=0.3f;
    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.8f;
        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity=Target_Velocity;
    
    Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D;
    Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D;
    Mecanum.Move_Z = Z;
    
    
    if(Dy==0 && Dx==0) {
        (*Flag_Run)++;        
        Mecanum.Move_X = 0;
        Mecanum.Move_Y = 0;
        Mecanum.Move_Z = 0;
    }
    Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析
    pid_setup_mecanum_speed();
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
}

static void calibrateXY_control(float x, float y, u8 *Flag_Run) {
    
    float Dx,Dy,Dz;
    Mecanum.Gyro_K=0.0065f;                   //校正时允许绕z轴转动
//    Dx = Mecanum.side_BC - x;
//    Dy = (Mecanum.side_AB) - y;
//    Dz = 15.0f-(Mecanum.side_AB + Mecanum.side_CD);
    
    Track_Read();

    Dx = Mecanum.side_BC + (15.0f-Mecanum.side_DA) -x*2;                              //x方向偏差
    Dy = Mecanum.side_AB + (15.0f-Mecanum.side_CD) - y*2;                             //y方向偏差
    
    //D = sqrt((double)(Dx*Dx + Dy*Dy));
    Dz =  (Mecanum.side_BC+Mecanum.side_DA) - (Mecanum.side_AB+Mecanum.side_CD);
    
    if((int)Mecanum.side_BC==0||(int)Mecanum.side_DA==0){
        Dz =  15.0f - (Mecanum.side_AB+Mecanum.side_CD);                                  //z方向偏差
        Dx = 0;
    }    
    if((int)Mecanum.side_AB==0||(int)Mecanum.side_CD==0){
        Dz =  (Mecanum.side_BC+Mecanum.side_DA) - 15.0f;                                  //z方向偏差
        Dy = 0;
    }
    
    
    Mecanum.Move_Y = 1.0f*Dy;
    Mecanum.Move_X = 1.0f*Dx;
    Mecanum.Move_Z = 0.0f*Dz;
    u2_printf("Dx=%.2f\tDy=%.2f\tDz=%.2f\r\n",Dx,Dy,Dz);
    Dz = 0;
    if(Dy<0.6f && Dy>-0.6f && Dx<0.6f && Dx>-0.6f && Dz>-0.6f && Dz<0.6f) { 
        Mecanum.Gyro_K=0.0065;
        (*Flag_Run)++;
        Mecanum.Move_Y = 0;
        Mecanum.Move_X = 0;
        Mecanum.Move_Z = 0;
    }
    Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析
    pid_setup_mecanum_speed();
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
}


static void grab_hand(int16_t *grab) {
    
    arm1(grab[0]);
    delay_ms(250);
    arm3(grab[2]);
    delay_ms(100);
    arm2(grab[1]);
    delay_ms(800);
    arm4(grab[3]);
    delay_ms(500);
}

static void turn_hand(int16_t *turn) {
    
    arm2(turn[1]);//-10 10 -60
    delay_ms(50);
    arm3(turn[2]);  //20 5 -20
    delay_ms(500);
    arm1(turn[0]);//0 35 0
    delay_ms(500);
}

static void place_hand(int16_t * place) {
    
    arm1(place[0]);//0 35 0
    delay_ms(50);
    arm2(place[1]);//-10 10 -60
    delay_ms(50);
    arm3(place[2]);  //20 5 -20
    delay_ms(1000);
    arm4(place[3]); //0 0 -20
    delay_ms(1000);
}

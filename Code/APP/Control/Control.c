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
#include "APP\OpenMV\OpenMV.h"

#define RIGHT 2
#define CENTER 1
#define LEFT 3

Flag Init_flag = MY_TRUE; //初始化标志位
u8 step=0;                //当前搬运第几个五块
u8 order[3];              //搬运顺序
float Target_Velocity = 0;//小车目标速度
#define Z -2.32           //抵消小车的轴向旋转
/************************************************************************
Car Control
************************************************************************/
static void order_set(u8 *order,enum Order_Set nowOrder);      //设置顺序
static void stop_control(u8 *Flag_Run);                        //小车停止
static void goToXY_control(int16_t x,int16_t y,u8 *Flag_Run);          //XY方向
//static void calibrateXY_control(float x, float y, u8 *Flag_Run);//感光条校准
static void grab_hand(int16_t *grab);                          //按照传入的参数控制机械臂抓取
static void turn_hand(int16_t *turn);                          //按照传入的参数控制机械臂转向
static void place_hand(int16_t * place);                       //控制机械臂放置    
static void move_hand(int16_t *move);                          //快速转动机械臂

void ready_control(void){
    
    if(PS2_ON_Flag == MY_TRUE)
        PS2_Usart_Control();             //PS2手柄控制小车，如果已经初始化PS2则执行
    Kinematic_Analysis(0,0,Z);           //小车运动学分析
    pid_setup_mecanum_speed();           //小车各轮子速度期望
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);   //pid控制
}

void scan_control(void) {
    
    static u8 left_right_flag=0;
    static u8 Flag_Run=0;
    
    if(Flag_Run == 0) {                     //边跑边检测颜色
        goToXY_control(posScanQR1.x,posScanQR2.y, &Flag_Run);
    } else if(Flag_Run == 1) {
        if(Init_flag == MY_TRUE&&Mecanum.X_Length>60) {
            move_hand(move_scan_color);           //伸手检测颜色
            OpenMV_Read_Color();
            Init_flag = MY_FALSE;
        }
        
        goToXY_control(posScanQR2.x,posScanQR2.y, &Flag_Run);
    } else if(Flag_Run == 2) {
        Init_flag = MY_TRUE;
        goToXY_control(posScanQR3.x,posScanQR3.y, &Flag_Run);
    } else if(Flag_Run == 3) {
        move_hand(move_scan_grab);            //伸手准备扫二维码
        Flag_Run++;
    } else if(Flag_Run == 4) {
        Init_flag = MY_TRUE;
        stop_control(&Flag_Run);                  //停车
    } else if(Flag_Run == 5) {
        if(Init_flag == MY_TRUE){
            OpenMV_Read_Grab();                   //扫码
            Init_flag = MY_FALSE;
        }
        if(Mecanum.grab_order_ready == MY_TRUE) { //扫到了
            Flag_Run++;
            move_hand(move_ready);
            Mecanum.state = goToDeparture;
        } else {                                  //转头
            if(left_right_flag == 0){
                setmotor_pwm(1, 30 + 1885, 1200);
                delay_ms(1000);
                left_right_flag = 1;
            }else{
                setmotor_pwm(1, -30 + 1885, 1200);
                delay_ms(1000);
                left_right_flag = 0;
            }
        }
    }
}

void goToDeparture_control(void) {
    
    static u8 Flag_Run=0;         //标识当前运行阶段
        
    if(Flag_Run == 0) {
        goToXY_control(posDeparture1.x,posDeparture1.y, &Flag_Run);
    } else if(Flag_Run == 1) {
        goToXY_control(posDeparture2.x,posDeparture2.y,&Flag_Run);
    } else if(Flag_Run == 2) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3) {
        Init_flag = MY_TRUE;
        Mecanum.state = grabFromDeparture;
        Flag_Run = 0;
    }
}

void grabFromDeparture_control(void) {
    
    if(Init_flag == MY_TRUE) {
        order_set(order,Mecanum.Departure_Order);
        Init_flag = MY_FALSE;
    }
    if(order[step] == RIGHT){
        grab_hand(grab_departure_right);
    } else if(order[step] == LEFT){
        grab_hand(grab_departure_left);
    } else if(order[step] == CENTER){
        grab_hand(grab_departure_center);
    }

    Mecanum.state = goToProcessing;
    Init_flag = MY_TRUE;
}



void goToProcessing_control(void) {
    
    static u8 Flag_Run=0;


    if(Init_flag == MY_TRUE){
        order_set(order,Mecanum.Place_Order);
        Init_flag = MY_FALSE;
        turn_hand(turn_processing);
    }

    if(Flag_Run == 0) {
        switch(order[step]){
            case RIGHT:
                goToXY_control(posProcessR1.x,posProcessR1.y,&Flag_Run);
                break;
            case LEFT:
                goToXY_control(posProcessL1.x,posProcessL1.y,&Flag_Run);
                break;
            case CENTER:
                goToXY_control(posProcessC1.x,posProcessC1.y,&Flag_Run);
                break;
        }
    } else if(Flag_Run == 1) {
        goToXY_control(Mecanum.X_Length,Mecanum.Y_Length+5,&Flag_Run);
        if(Mecanum.side_BC>12) Flag_Run++;
    } else if(Flag_Run == 2) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3) {
        goToXY_control(Mecanum.X_Length-5,Mecanum.Y_Length,&Flag_Run);
        if(Mecanum.side_AB>12) Flag_Run++;
    } else if(Flag_Run == 4) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 5) {
        Mecanum.state = placeToProcessing;
        Flag_Run = 0;
        step++;
    }
}

void placeToProcessing_control(void) {
    
    place_hand(place_processing);
    
    if(step == 3){
        Mecanum.state = backToProcessing;
        step = 0;
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
        goToXY_control(posDeparture2.x,posDeparture2.y,&Flag_Run);
    } else if(Flag_Run == 1) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 2) {
        Mecanum.state = grabFromDeparture;
        Init_flag = MY_TRUE;
        Flag_Run = 0;
    }       
}

void backToProcessing_control(void) {
    
    static u8 Flag_Run = 0;
    if(Init_flag == MY_TRUE){
        order_set(order,Mecanum.Place_Order);
        turn_hand(turn_processing);
        Init_flag = MY_FALSE;
    }
    
    if(Flag_Run == 0) {
        switch(order[step]){
            case RIGHT:
                goToXY_control(posProcessR1.x,posProcessR1.y,&Flag_Run);
                break;
            case LEFT:
                goToXY_control(posProcessL1.x,posProcessL1.y,&Flag_Run);
                break;
            case CENTER:
                goToXY_control(posProcessC1.x,posProcessC1.y,&Flag_Run);
                break;
        }
    } else if(Flag_Run == 1) {
        goToXY_control(Mecanum.X_Length,Mecanum.Y_Length+5,&Flag_Run);
        if(Mecanum.side_BC>12) Flag_Run++;
    } else if(Flag_Run == 2) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3) {
        //Track_Read();
        goToXY_control(Mecanum.X_Length-5,Mecanum.Y_Length,&Flag_Run);
        if(Mecanum.side_AB>12) Flag_Run++;
    } else if(Flag_Run == 4) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 5) {
        Mecanum.state = grabFromProcessing;
        Flag_Run = 0;
    }
}

void grabFromProcessing_control(void) {
    
    grab_hand(grab_processing);
    Mecanum.state = goToFinish;
    Init_flag = MY_TRUE;
}

void goToFinish_control(void) {
    
    static u8 Flag_Run=0;

    if(Init_flag == MY_TRUE){
        order_set(order,Mecanum.Place_Order);
        Init_flag = MY_FALSE;
        turn_hand(turn_finish);
    }

    if(Flag_Run == 0) {
        switch(order[step]){
            case RIGHT:
                goToXY_control(posFinishR.x,posFinishR.y,&Flag_Run);
                break;
            case LEFT:
                goToXY_control(posFinishL.x,posFinishL.y,&Flag_Run);
                break;
            case CENTER:
                goToXY_control(posFinishC.x,posFinishC.y,&Flag_Run);
                break;
        }
    } else if(Flag_Run == 1) {
        goToXY_control(Mecanum.X_Length+5,Mecanum.Y_Length,&Flag_Run);
        if(Mecanum.side_CD>10) Flag_Run++;
    } else if(Flag_Run == 2) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 3) {
        //Track_Read();
        goToXY_control(Mecanum.X_Length,Mecanum.Y_Length-5,&Flag_Run);
        if(Mecanum.side_DA>10) Flag_Run++;
    } else if(Flag_Run == 4) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 5) {
        Mecanum.state = placeToFinish;
        Flag_Run = 0;
        step++;  
    }
}

void placeToFinish_control(void) {
    
    place_hand(place_finish);
    
    if(step == 3){
        Mecanum.state = back;
        step = 0;
    }
    else {
        Mecanum.state = backToProcessing;
    }
    Init_flag = MY_TRUE;
}

void back_control(void) {
    
    static u8 Flag_Run=0;
    if(Init_flag == MY_TRUE){
        Init_flag = MY_FALSE;
        turn_hand(turn_departure);
    }
    
    if(Flag_Run == 0) {
        goToXY_control(4,3,&Flag_Run);
    } else if(Flag_Run == 1) {
        stop_control(&Flag_Run);
    } else if(Flag_Run == 2) {
        Mecanum.state = ready;
        Flag_Run = 0;
    }
    
}

static void stop_control(u8 *Flag_Run) {
    
//    static u8 counter=0;
//    if(Mecanum.Encoder_A==0&&Mecanum.Encoder_B==0&&Mecanum.Encoder_C==0&&Mecanum.Encoder_D==0) {
//        counter++;
//        if(counter==10){
//            counter = 0;
//            MOTO_A_Set(0);
//            MOTO_B_Set(0);
//            MOTO_C_Set(0);
//            MOTO_D_Set(0);
//            (*Flag_Run)++;
//        }
//    } else {
//        counter = 0;
//        Kinematic_Analysis(0,0,Z);           //小车运动学分析
//        pid_setup_mecanum_speed();           //小车各轮子速度期望
//        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);   //pid控制
//    }
    MOTO_A_Set(0);
    MOTO_B_Set(0);
    MOTO_C_Set(0);
    MOTO_D_Set(0);
    (*Flag_Run)++;
}

static void order_set(u8 *order,enum Order_Set nowOrder) {
    switch(nowOrder) {
        case rcl: order[0] = RIGHT; order[1] = CENTER; order[2] = LEFT; break;
        case rlc: order[0] = RIGHT; order[1] = LEFT; order[2] = CENTER; break;
        case lrc: order[0] = LEFT; order[1] = RIGHT; order[2] = CENTER; break;
        case lcr: order[0] = LEFT; order[1] = CENTER; order[2] = RIGHT; break;
        case clr: order[0] = CENTER; order[1] = LEFT; order[2] = RIGHT; break;
        case crl: order[0] = CENTER; order[1] = RIGHT; order[2] = LEFT; break;
    }
}

static void goToXY_control(int16_t x,int16_t y,u8 *Flag_Run) {
    
    float D,Dx,Dy;
    
    Dy = y - Mecanum.Y_Length;
    Dx = x - Mecanum.X_Length;

    D = sqrt((double)(Dx*Dx + Dy*Dy));
    
    if(D>45){
        Target_Velocity=45;
    }
    else if(D>5){
        Target_Velocity=D;
    }
    else {
        Target_Velocity=5; 
    } 
    
    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.5f;
        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity=Target_Velocity;
    
    
    Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D;
    Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D;
    Mecanum.Move_Z = Z;
    
    if(*Flag_Run == 1&& Mecanum.state == scan) {
        Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D*0.35f;
        Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D*0.35f;
        Mecanum.Move_Z = Z;
    }
    
    if(Dy==0 && Dx==0) {
        (*Flag_Run)++;        
        MOTO_A_Set(0);
        MOTO_B_Set(0);
        MOTO_C_Set(0);
        MOTO_D_Set(0);
    } else {
        Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析
        pid_setup_mecanum_speed();
        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
    }
}

//static void calibrateXY_control(float x, float y, u8 *Flag_Run) {    //垃圾东西 千万别用
//    
//    float Dx,Dy,Dz;
//    Mecanum.Gyro_K=0.0065f;                   //校正时允许绕z轴转动
//    
//    Track_Read();

//    Dx = Mecanum.side_BC + (15.0f-Mecanum.side_DA) -x*2;                              //x方向偏差
//    Dy = Mecanum.side_AB + (15.0f-Mecanum.side_CD) - y*2;                             //y方向偏差
//    
//    //D = sqrt((double)(Dx*Dx + Dy*Dy));
//    Dz =  (Mecanum.side_BC+Mecanum.side_DA) - (Mecanum.side_AB+Mecanum.side_CD);
//    
//    if((int)Mecanum.side_BC==0||(int)Mecanum.side_DA==0){
//        Dz =  15.0f - (Mecanum.side_AB+Mecanum.side_CD);                                  //z方向偏差
//        Dx = 0;
//    }    
//    if((int)Mecanum.side_AB==0||(int)Mecanum.side_CD==0){
//        Dz =  (Mecanum.side_BC+Mecanum.side_DA) - 15.0f;                                  //z方向偏差
//        Dy = 0;
//    }
//    
//    
//    Mecanum.Move_Y = 1.0f*Dy;
//    Mecanum.Move_X = 1.0f*Dx;
//    Mecanum.Move_Z = 0.0f*Dz;
//    u2_printf("Dx=%.2f\tDy=%.2f\tDz=%.2f\r\n",Dx,Dy,Dz);
//    Dz = 0;
//    if(Dy<0.6f && Dy>-0.6f && Dx<0.6f && Dx>-0.6f && Dz>-0.6f && Dz<0.6f) { 
//        Mecanum.Gyro_K=0.0065;
//        (*Flag_Run)++;
//        Mecanum.Move_Y = 0;
//        Mecanum.Move_X = 0;
//        Mecanum.Move_Z = 0;
//    }
//    Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析
//    pid_setup_mecanum_speed();
//    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
//}


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
    
    arm2(turn[1]);
    delay_ms(100);
    arm3(turn[2]);
    delay_ms(400);
    arm1(turn[0]);
    delay_ms(1200);
    //delay_ms(1000);
}

static void place_hand(int16_t * place) {
    
    arm1(place[0]);
    delay_ms(50);
    arm2(place[1]);
    delay_ms(50);
    arm3(place[2]);
    delay_ms(1000);
    arm4(place[3]);
    delay_ms(1000);
}

static void move_hand(int16_t *move) {
    
    //arm1(move[0]);
    //delay_ms(20);
    arm2(move[1]);
    delay_ms(5);
    arm3(move[2]);
    delay_ms(5);
    arm4(move[3]);
}

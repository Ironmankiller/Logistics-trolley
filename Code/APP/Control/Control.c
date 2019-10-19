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
#define Z -1.5            //抵消小车的轴向旋转
/************************************************************************
Control
************************************************************************/
static void grab_order_set(u8 *order);                         //设置抓取顺序
static void put_order_set(u8 *order);                          //设置放置顺序
static void stop_control(void);                                //小车停止
static void goToX_control(int x,u8 *Flag_Run);                 //向X方向行驶,目标位置为x(从上向下看为向左行驶)
static void goToY_control(int y,u8 *Flag_Run);                 //向Y方向行驶
static void goToXY_control(int x,int y,u8 *Flag_Run);          //XY方向
static void calibrate_control(float x, float y, u8 *Flag_Run); //感光条校准
static void grab_hand(int16_t *grab);                          //按照传入的参数控制机械臂抓取
static void turn_hand(int16_t *turn);                          //按照传入的参数控制机械臂转向
static void place_hand(int16_t * place);                       //控制机械臂放置    

void ready_control(void){
    if(PS2_ON_Flag == MY_TRUE)
        PS2_Usart_Control();             //PS2手柄控制小车，如果已经初始化PS2则执行

    Kinematic_Analysis(0,0,0);           //小车运动学分析
    pid_setup_mecanum_speed();           //小车各轮子速度期望
    pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);   //pid控制
}

void goToDeparture_control(void) {
    
    static u8 Flag_Run=0;         //标识当前运行阶段
 
    if(Flag_Run == 0) {
        goToXY_control(114,40, &Flag_Run); 
    } else if(Flag_Run == 1) {
        goToXY_control(114,2,&Flag_Run);
    } else if(Flag_Run == 2) {
        Track_Read();
        calibrate_control(7.5f,6.0f,&Flag_Run);
    } else if(Flag_Run == 3){
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

    if(Flag_Run == 0){
        goToXY_control(113,162,&Flag_Run);
    } else if(Flag_Run == 1) {
        switch(order[step]){
            case RIGHT:
                goToX_control(141,&Flag_Run);
                break;
            case LEFT:
                goToX_control(84,&Flag_Run);
                break;
            case CENTER:
                goToX_control(113,&Flag_Run);
                break;
        }
    } else if(Flag_Run == 2) {
        Mecanum.state = place;
        stop_control();
        Flag_Run = 0;
        step++;
    }
}

void place_control(void) {

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

    if(Flag_Run == 0){
        goToX_control(113,&Flag_Run);
    } else if(Flag_Run == 1) {
        goToXY_control(113,2,&Flag_Run);
    } else if(Flag_Run == 2) {
        Track_Read();
        calibrate_control(7.5f,6.0f,&Flag_Run);
    } else if(Flag_Run == 3) {
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
        Mecanum.state = ready;
        stop_control();
        Flag_Run = 0;
    }
    
}

static void stop_control(void) {
    Mecanum.Move_X = 0.0f;
    Mecanum.Move_Y = 0.0f;
    Mecanum.Move_Z = 0.0f;
    MOTO_A_Set(0);
    MOTO_B_Set(0);
    MOTO_C_Set(0);
    MOTO_D_Set(0);
    Target_Velocity = 0;
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
static void goToX_control(int x,u8 *Flag_Run) {          //控制X方向运动
    
    float D,Dx,Dy;
    
    Dx = x - Mecanum.X_Length;
    Dy = 0;
    if(Dx<2 && Dx>-2) Dx = 0;             //若满足则认为到达该位置
    D = sqrt((double)(Dx*Dx + Dy*Dy));    //算出目标位移
    
    if(D>60){
        Target_Velocity=40;
    }
    else if(D>25){
        Target_Velocity=30.0f/35.0f*D - 80.0/7.0;
    }
    else {
        Target_Velocity=10; 
    } 
    
    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.6f;
        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity-=0.5f;
    
    Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D;
    Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D;
    Mecanum.Move_Z = 0.0f;
    
    if(Dx==0) { 
        stop_control();  
        (*Flag_Run)++;         //切换下一状态           
        delay_ms(500);
    }else{
        Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Z);  //进行运动学分析
        pid_setup_mecanum_speed();
        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
    }
}
    
static void goToY_control(int y,u8 *Flag_Run) {   //控制Y方向运动
    
    float D,Dx,Dy;
    
    Dy = y-Mecanum.Y_Length;
    Dx = 0;
    if(Dy<2 && Dy>-2) Dy = 0;
    D = sqrt((double)(Dx*Dx + Dy*Dy));
    
    if(D>60){
        Target_Velocity=40;
    }
    else if(D>25){
        Target_Velocity=30.0f/35.0f*D - 80.0/7.0;
    }
    else {
        Target_Velocity=10; 
    } 
    
    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.6f;
        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity-=0.5f;
    
    Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D;
    Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D;
    Mecanum.Move_Z = 0.0f;
    
    if(Dy==0) {
        stop_control();
        (*Flag_Run)++;         //切换下一状态
        delay_ms(500);
    }else{
        Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Z);  //进行运动学分析
        pid_setup_mecanum_speed();
        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
    }
}
static void goToXY_control(int x,int y,u8 *Flag_Run) {
    
    float D,Dx,Dy;
    
    Dy = y - Mecanum.Y_Length;
    Dx = x - Mecanum.X_Length;
    if(Dy<2 && Dy>-2) Dy = 0;
    if(Dx<2 && Dx>-2) Dx = 0;
    D = sqrt((double)(Dx*Dx + Dy*Dy));
    
    if(D>60){
        Target_Velocity=40;
    }
    else if(D>25){
        Target_Velocity=30.0f/35.0f*D - 80.0/7.0;
    }
    else {
        Target_Velocity=10; 
    } 
    
    if(Target_Velocity>Mecanum.RC_Velocity) Mecanum.RC_Velocity+=0.6f;
        else if(Target_Velocity<Mecanum.RC_Velocity) Mecanum.RC_Velocity-=0.5f;
    
    Mecanum.Move_X = Mecanum.RC_Velocity*Dx/D;
    Mecanum.Move_Y = Mecanum.RC_Velocity*Dy/D;
    Mecanum.Move_Z = 0.0f;
    
    if(Dy==0 && Dx==0) {
        stop_control();
        (*Flag_Run)++;        
        delay_ms(500);
    }else{
        Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Z);  //进行运动学分析
        pid_setup_mecanum_speed();
        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
    }
}

static void calibrate_control(float x, float y, u8 *Flag_Run) {
    
    float D,Dx,Dy,Dz;
    Mecanum.Gyro_K=0;
    Dx = Mecanum.side_BC - x;
    Dy = (Mecanum.side_AB) - y;
    Dz = 15.0f-(Mecanum.side_AB + Mecanum.side_CD);
    D = sqrt((double)(Dx*Dx + Dy*Dy));
    
    if((int)Mecanum.side_BC==0)
        Dx = 0;
    if((int)Mecanum.side_AB==0)
        Dy = 0;
    
    Mecanum.Move_Y = 4.0f*Dy/D;
    Mecanum.Move_X = 4.0f*Dx/D;
    Mecanum.Move_Z = Dz;
    //u1_printf("AB=%.2f\tBC=%.2f\tMove_X=%.2f\tMove_Y=%.2f\tDx=%.2f\tDy=%.2f\r\n",Mecanum.side_AB,Mecanum.side_BC,Mecanum.Move_X,Mecanum.Move_Y,Dx,Dy);
    if(Dy<0.3f && Dy>-0.3f && Dx<0.3f & Dx>-0.3f) { 
            stop_control();
            Mecanum.Gyro_K=0.0045;
            (*Flag_Run)++;
            delay_ms(500);
    }else{
        Kinematic_Analysis(Mecanum.Move_X,Mecanum.Move_Y,Mecanum.Move_Z);  //进行运动学分析
        pid_setup_mecanum_speed();
        pid_ctr_mecanum_speed(Mecanum.Encoder_A,Mecanum.Encoder_B,Mecanum.Encoder_C,Mecanum.Encoder_D);
    }
}

static void grab_hand(int16_t *grab) {
    
    arm1(grab[0]);
    delay_ms(250);
    arm2(grab[1]);
    delay_ms(100);
    arm3(grab[2]);
    delay_ms(800);
    arm4(grab[3]);
    delay_ms(300);
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
    
    arm1(place[3]);//0 35 0
    delay_ms(50);
    arm2(place[2]);//-10 10 -60
    delay_ms(50);
    arm3(place[1]);  //20 5 -20
    delay_ms(1000);
    arm4(place[0]); //0 0 -20
    delay_ms(1000);
}

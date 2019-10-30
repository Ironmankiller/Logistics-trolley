#include "stm32f4xx.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "Task_Loop.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\Time\Time.h"
#include "APP\Display\Display.h"
#include "BSP\LED\LED.h"
#include "APP\Control\Control.h"
#include "BSP\KEY\KEY.h"
#include "BSP\key\Key_Board.h"
#include "BSP\BEEP\BEEP.h"
#include "BSP\Motor_Drive\Moto_Drive.h"
#include "BSP\Encoder\Encoder.h"
#include "APP\Control\Algorithm.h"
#include "APP\IMU\ahrs.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\BEEP\BEEP.h"
#include "APP\IMU\ahrs.h"
#include "APP\Control\pid.h"
#include "BSP\MPU60502\MPU60502.h"
#include "BSP\NRF24L01\NRF24L01.h"
#include "BSP\MPU6050\MPU6050.h"
#include "BSP\PS2\PS2.h"
#include "BSP\GY953\GY953.h"
#include "BSP\MCU\UART\usart5.h"
#include "BSP\Mecanum\Mecanum.h"
#include "BSP\HMI\HMI.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#include "BSP\MCU\ADC\ADC.h"
#include "BSP\MCU\UART\uart4.h"
#include "BSP\Motor_Drive\Servo_Moto.h"
#include "BSP\Motor_Drive\Steer_Moto.h"
#include "APP\Track\Track.h"
#include "BSP\74HC165\74HC165.h"

/* 任务循环计时结构体 */
typedef struct {
    uint8_t check_flag;
    uint8_t err_flag;
    uint8_t cnt_1ms;
    uint8_t cnt_2ms;
    uint8_t cnt_5ms;
    uint8_t cnt_10ms;
    uint8_t cnt_20ms;
    uint8_t cnt_50ms;
}Loop_Time_TypeDef;

static Loop_Time_TypeDef Loop_Time;    //任务循环计时

__INLINE static void Task_1ms(void);
__INLINE static void Task_2ms(void);
__INLINE static void Task_5ms(void);
__INLINE static void Task_10ms(void);
__INLINE static void Task_20ms(void);
__INLINE static void Task_50ms(void);

/**********************************************
* 清空循环计数标志
**********************************************/
void Loop_Time_Clear(void)
{
    memset(&Loop_Time, 0, sizeof(Loop_Time_TypeDef));
}

/**********************************************
* 任务循环
* 由主函数初始化完成后调用
**********************************************/
void Task_Loop(void)
{
    if (Loop_Time.check_flag >= 1)
    {
        Task_1ms();                     //周期1ms的任务
        if (Loop_Time.cnt_2ms >= 2)
        {
            Loop_Time.cnt_2ms = 0;
            Task_2ms();	 			//周期2ms的任务
        }
        if (Loop_Time.cnt_5ms >= 5)//
        {
            Loop_Time.cnt_5ms = 0;
            Task_5ms();                 //周期5ms的任务
        }
        if (Loop_Time.cnt_10ms >= 10)
        {
            Loop_Time.cnt_10ms = 0;
            Task_10ms();                //周期10ms的任务
        }
        if (Loop_Time.cnt_20ms >= 20)
        {
            Loop_Time.cnt_20ms = 0;
            Task_20ms();                //周期20ms的任务
        }
        if (Loop_Time.cnt_50ms >= 50)
        {
            Loop_Time.cnt_50ms = 0;
            Task_50ms();                //周期50ms的任务
        }


        Loop_Time.check_flag = 0;       //循环运行完毕标志
    }
}

/**********************************************
* 1ms任务
**********************************************/
__INLINE static void Task_1ms(void)
{   
    /* 执行上位机指令 */
    ANO_DT_Do();
}

/**********************************************
* 2ms任务
**********************************************/
__INLINE static void Task_2ms(void)
{
    float outer_loop_time;

    outer_loop_time = Get_Cycle_T(0);			//获取外环准确的执行周期

    MPU6050_Read();
    MPU6050_Data_Prepare(outer_loop_time);
    //MPU60502_Read();
    //MPU60502_Data_Prepare(outer_loop_time);

    HMI_Data_Handle((uint8_t*)USART_RX_BUF);
}
/**********************************************
* 5ms任务
**********************************************/
__INLINE static void Task_5ms(void)
{
    /*EulerAngle Updeate*/
    ahrs();
}

Flag GYRO_Calibrate = MY_TRUE; //陀螺仪校准标志位
/**********************************************
* 10ms任务               
**********************************************/
__INLINE static void Task_10ms(void)
{

    Mecanum_Update(-Read_Encoder(8), -Read_Encoder(5), -Read_Encoder(3), Read_Encoder(4));

    //u1_printf("%d\r\n",Mecanum.state);

    switch (Mecanum.state)
    {
    case ready:
        ready_control();
        break;
    case goToDeparture:
        goToDeparture_control();
        break;
    case grabFromDeparture:
        grabFromDeparture_control();
        break;
    case goToProcessing:
        goToProcessing_control();
        break;
    case placeToProcessing:
        placeToProcessing_control();
        break;
    case backToDeparture:
        backToDeparture_control(); 
        break;
    case grabFromProcessing:
        grabFromProcessing_control();
        break;
    case goToFinish:
        goToFinish_control();
        break;
    case placeToFinish:
        placeToFinish_control();
        break;
    case backToProcessing:
        backToProcessing_control();
        break;
    case back:
        back_control();
        break;

    default:
        break;
    }
    
}
/**********************************************
* 20ms任务
**********************************************/
__INLINE static void Task_20ms(void)
{
    /* PS2 */
    Remote();
    //u1_printf("X:%d\tY:%d\r\n",Mecanum.X_Length,Mecanum.Y_Length);
    /* 发送传感器参数到上位机 */
//    ANO_DT_Send_Senser(
//    Mecanum.Target_A, Mecanum.Encoder_A, Mecanum.Target_B,
//	Mecanum.Encoder_B ,Mecanum.Target_C , Mecanum.Encoder_C, Mecanum.Target_D,
//    Mecanum.Encoder_D, 0, 0);

//    ANO_DT_Send_Status(EulerAngle.Roll, EulerAngle.Pitch, EulerAngle.Yaw, 0, 0, 1);
}

/**********************************************
* 50ms任务
**********************************************/
extern AHRS_EulerAngleTypeDef  EulerAngle;
__INLINE static void Task_50ms(void)
{
    /* LED闪烁 */
//    LED_Blink(LED_2);
//    LED_Blink(LED_3);
//    LED_Blink(LED_4);
    LED_Blink(LED_1);

    /* 显示 */
//    Display_Mecanum();
    
    //HC165_Show();
      //  Track_Read();
    //u2_printf("AB=%.2f\tBC=%.2f\tCD=%.2f\tDA=%.2f\r\n",Mecanum.side_AB,Mecanum.side_BC,Mecanum.side_CD,Mecanum.side_DA);
    u2_printf("x=%d\ty=%d\r\n",Mecanum.X_Length,Mecanum.Y_Length);
    
    /* 独立按键（调试用 比赛时慎用） */
    switch (KEY_Scan())
    {
    case 2:
        LED_SWITCH(LED_1);
        break;
    case 3:
        LED_SWITCH(LED_2);
        break;
    case 4:
        //Mecanum.state = goToDeparture;
        LED_SWITCH(LED_3);
        break;
    case 5:
        Mecanum.state = goToDeparture;
        arm1(0);//0 35 0
        delay_ms(50);
        arm2(-70);//-10 10 -60
        delay_ms(50);
        arm3(-20);  //20 5 -20
        delay_ms(50);
        arm4(-10); //0 0 -20
		LED_SWITCH(LED_4);
        break;
      default:      break;
    }
}


/**********************************************
* 任务循环计时
* 由1ms定时中断调用
**********************************************/
void Loop_Time_Check(void)
{
    Loop_Time.cnt_2ms++;
    Loop_Time.cnt_5ms++;
    Loop_Time.cnt_10ms++;
    Loop_Time.cnt_20ms++;
    Loop_Time.cnt_50ms++;

    if (Loop_Time.check_flag >= 2)
    {
        Loop_Time.err_flag++;     //每累加一次，证明代码在预定周期2ms内没有跑完。
    }
    else
    {
        Loop_Time.check_flag += 1;	//该标志位在循环的最后被清零
    }
}

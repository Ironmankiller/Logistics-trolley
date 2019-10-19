/*
 * Uart_Command_Dispose.c
 *
 *  Created on: 2016年11月26日
 *      Author: DGVY
 */
 
#include <stdint.h>
#include "Uart_Command_Dispose.h"
#include <stdio.h>
#include "stm32f4xx.h"
#include <string.h>
#include "PID_Parameter.h"
#include "ANO_DT.h"
#include <stdlib.h>

/************************************************************************/
//  指令格式：
//          Command_Head+Command_Body=([xxxx[,xxxx[,...]]])
/************************************************************************/

const u8 Command_Head[][COMMOND_HEAD_LEN] = {
    "SYSTEM",
    "RUNTO",
    "SEND",
    "SET"
};


const u8 Command_Body[][10][COMMOND_BODY_LEN] = {

    // Command_SYSTEM_Body
    {
        "TEST",
        "RESET"
    },

    // Command_RUNTO_Body
    {
        "LOCATION",
        "GO",
        "STOP",
        "ORIGIN"
    },

    // Command_SEND_Body
    {
        "COORDS",
        "ANGLE",
        "ALL",
        "CONTINUE_COORDS",
        "CONTINUE_ANGLE",
        "CONTINUE_ALL",
        "CLOSE"
    },

    // Command_MODE_Body
    {
        "MODE",
        "GROUND"
    }

};

 

// 函数指针数组
void(*Command_Function[][10])(uint8_t *parameter) = {

    // SYSTEM
    {
        SYSTEM_TEST,
        SYSTEM_RESET
    },

    // RUNTO
    {
        RUNTO_LOCATION,
        RUNTO_GO,
        RUNTO_STOP,
        RUNTO_ORIGIN
    },

    // SEND
    {
        SEND_COORDS,
        SEND_ANGLE,
        SEND_ALL,
        SEND_CONTINUE_COORDS,
        SEND_CONTINUE_ANGLE,
        SEND_CONTINUE_ALL,
        SEND_CLOSE
    },

    //MODE
    {
        SET_MODE,
        SET_GROUND
    }
};


//-----------------------------------------------------------------------------
// 命令执行函数
//-----------------------------------------------------------------------------

//=====System======================================
// SYSTEM+TEST=()
// Input:NONE
// 测试系统功能是否正常
void SYSTEM_TEST(uint8_t *parameter)
{
    printf("\r\nEverything is OK!\r\n\r\n");
}

// SYSTEM+RESET=()
// Input:NONE
// 系统复位
void SYSTEM_RESET(uint8_t *parameter)
{
    printf("# Reseting...\r\n");
    NVIC_SystemReset(); //复位函数
}

//=====RUNTO=======================================
// RUNTO+LOCATION=(?,?,?)
// Input:(x,y,angle)    [目的点坐标和角度]
// 通过命令控制底盘行动到指定地点
// 执行此命令后，需要执行 RUNTO+GO=() 命令
// 才能让车子动起来
void RUNTO_LOCATION(uint8_t *parameter)
{
 //   //TODO: 修改目的点位置，车子不动
 //   char * ch_p1 = 0;          //第一个','的地址
 //   ch_p1 = strchr((char*)parameter, ',');  
 //   size_t ch_x_len = 0;        //x字符串长度
 //   ch_x_len = (size_t)((uint64_t)ch_p1 - (uint64_t)parameter);   

 //   char * ch_p2 = 0;          //第二个','的地址
 //   ch_p2 = strrchr((char*)parameter, ','); 
 //   size_t ch_y_len = 0;        //y字符串长度
 //   ch_y_len = (size_t)((uint64_t)ch_p2 - (uint64_t)ch_p1 - 1);

 //   size_t ch_ang_len = 0;      //ang字符串长度
 //   ch_ang_len = strlen((char *)parameter) - ch_x_len - ch_y_len - 1-1;
 //   
 //   uint8_t ch_x[10] = { 0 };                    //x坐标字符串
 //   memcpy(ch_x, parameter, ch_x_len);

 //   uint8_t ch_y[10] = { 0 };                   //y坐标字符串
 //   memcpy(ch_y, ch_p1+1, ch_y_len);

 //   uint8_t ch_ang[10] = { 0 };
 //   memcpy(ch_ang, ch_p2+1, ch_ang_len);

 //   int32_t xloca = 0;
 //   xloca = atoi((char* )ch_x);
 //   int32_t yloca = 0;
 //   yloca = atoi((char *)ch_y);
 //   int32_t angloca = 0;
 //   angloca = atoi((char *)ch_ang);

 //   printf("angle: %d\r(%d, %d)\r\n", angloca,xloca,yloca);
 //   
	//PointTypeDef target;
 //   memset(&target, 0, sizeof(PointTypeDef));
 //   target.Angle = (float)angloca;
 //   target.Coords.x = xloca;
 //   target.Coords.y = yloca;
 //   Robot_Set_Target_Locaiton(&tar_pos,&target);
}

 //SET+GROUND=(RED/BLUE)
 //Input:1 2
 //选择红蓝场。1：红场 2：蓝场
const uint8_t g_color[2][15] = { "GROUND_RED","GROUND_BLUE" };
void SET_GROUND(uint8_t *parameter)
{
    uint8_t i = 0;
    while (strcmp((const char*)g_color[i], (char*)parameter) != 0)
    {
        i++;
        if (i > 2)
        {
            printf("\tError ground colour! Please try again!\r\n");
            return;
        }
    }
    i++;
    
    switch (i)
    {
    case 1: 
        Robot_Ground_Set(Ground_Red); 
        printf("$ Set Ground Red\r\n");
        break;
    case 2:
        Robot_Ground_Set(Ground_Blue);
        printf("$ Set Ground Blue\r\n");
        break;
    default:
        break;
    }

}

// RUNTO+GO=()
// Input:NONE
// 执行完 RUNTO+LOCATION=(?,?,?) 后
// 需要执行这一条命令才能让车子动起来
void RUNTO_GO(uint8_t *parameter)
{
    //TODO: 车子动起来，跑到上个指令写入的目的点位置
    Robot_Run();
	printf("\r\nThe old driver never overturned!\r\n");
}

// RUNTO+STOP=()
// Input:NONE
// 老司机奥义-紧急刹车
void RUNTO_STOP(uint8_t *parameter)
{
    Robot_Emergency_Braking();
    printf("\r\nSTOP!\r\n");
}

// RUNTO+ORIGIN=()
// Input:NONE
// 老司机奥义-回到原点
void RUNTO_ORIGIN(uint8_t *parameter)
{
 //   PointTypeDef target;
 //   memset(&target, 0, sizeof(PointTypeDef));
 //   target.Angle = (float)0;
 //   target.Coords.x = 0;
 //   target.Coords.y = 0;
	////int32_t target[3] = { 0, 0, 0 };
 //   Robot_Set_Target_Locaiton(&tar_pos,&target);
}


//=====SEND========================================
// SEND+COORDS=()
// Input:NONE
// 向上位机发送一次当前坐标值
void SEND_COORDS(uint8_t *parameter)
{
    printf("(%d,  %d)\r\n", G_Robot_Master.Now_Position.Coords.x, G_Robot_Master.Now_Position.Coords.y);
}

// SEND+ANGLE=()
// Input:NONE
// 向上位机发送一次当前角度值
void SEND_ANGLE(uint8_t *parameter)
{
    printf("angle: %f\r\n", G_Robot_Master.Now_Position.Angle);
}

// SEND+ALL=()
// Input:NONE
// 向上位机发送一次当前坐标和角度值
void SEND_ALL(uint8_t *parameter)
{
    printf("angle: %f\r\n", G_Robot_Master.Now_Position.Angle);
    printf("(%d, %d)\r\n", G_Robot_Master.Now_Position.Coords.x, G_Robot_Master.Now_Position.Coords.y);
}

// SEND+CONTINUE_COORDS=()
// Input:NONE
// 向上位机持续发送当前坐标值
// 执行 SEND+CLOSE=() 可停止发送
void SEND_CONTINUE_COORDS(uint8_t *parameter)
{
    printf("\r\nOK\r\n\r\n");
}

// SEND+CONTINUE_ANGLE=()
// Input:NONE
// 向上位机持续发送当前角度值
// 执行 SEND+CLOSE=() 可停止发送
void SEND_CONTINUE_ANGLE(uint8_t *parameter)
{

}

// SEND+CONTINUE_ALL=()
// Input:NONE
// 向上位机持续发送当前坐标和角度值
// 执行 SEND+CLOSE=() 可停止发送
void SEND_CONTINUE_ALL(uint8_t *parameter)
{
    printf("\r\nOK\r\n\r\n");
}

// SEND+CLOSE=()
// Input:NONE
// 停止发送数据指令
void SEND_CLOSE(uint8_t *parameter)
{
    printf("\r\nOK\r\n\r\n");
}

//=====SET========================================
// SET+MODE=()
// Input:NONE
// 设定模式
#define G_MODE_LEN 5
const uint8_t g_mode[][15] = { "TEST_ALL","RUNTO_JUNCTION","TEST_STRAIGHT","COMPETITION" };
void SET_MODE(uint8_t *parameter)
{
 //   uint8_t i = 0;
 //   while (strcmp((const char*)g_mode[i], (char*)parameter) != 0)
 //   {
 //       i++;

 //       if (i > G_MODE_LEN)
 //       {
 //           printf("\tMode is not existent! Please try again!\r\n");
 //           return;
 //       }
 //   }
 //   i++;

 //   switch (i)
 //   {
 //   case 1:
 //       G_Robot_Master.Mode = Test_All;
 //       break;
 //   case 2:
 //       G_Robot_Master.Mode = Runto_Junction;
 //       break;
 //   case 3:
 //       G_Robot_Master.Mode = Test_Prepare;
 //       break;
 //   case 4:
 //       G_Robot_Master.Mode = Runto_Target;
 //       break;
	//case 5:
	//	G_Robot_Master.Mode = Line_Adjust;
	//	break;
 //   default:
 //       break;
 //   }
    
}

// SET+GROUND=(RED/BLUE)
// Input:RED or BLUE
// 选择红蓝场
//const uint8_t g_color[2][15] = { "GROUND_RED","GROUND_BLUE" };
//void SET_GROUND(uint8_t *parameter)
//{
//    uint8_t i = 0;
//    while (strcmp((const char*)g_color[i], (char*)parameter) != 0)
//    {
//        i++;
//        if (i > 2)
//        {
//            printf("\tError ground colour! Please try again!\r\n");
//            return;
//        }
//    }
//    i++;
//    
//    switch (i)
//    {
//    case 1: 
//        Robot_Ground_Set(Ground_Red); 
//        printf("$ Set Ground Red\r\n");
//        break;
//    case 2:
//        Robot_Ground_Set(Ground_Blue);
//        printf("$ Set Ground Blue\r\n");
//        break;
//    default:
//        break;
//    }
//
//}

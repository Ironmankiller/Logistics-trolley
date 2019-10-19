/*
 * Task_Setting.h
 *
 *  Created on: 2016年11月29日
 *      Author: DGVY
 */
    
 #include "includes.h"


 /******************************************************************************/
 //任务函数
void start_task(void *p_arg);

//任务优先级
extern const OS_PRIO START_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE START_STK_SIZE;

//任务控制块
extern OS_TCB StartTaskTCB;

//任务堆栈
extern __align(8) CPU_STK START_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Uart_Command_Task(void *p_arg);

//任务优先级
extern const OS_PRIO UART_COMMAND_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE UART_COMMAND_STK_SIZE;

//任务控制块
extern OS_TCB UartCommandTaskTCB;

//任务堆栈
extern __align(8) CPU_STK UART_COMMAND_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Half_Second_task(void *p_arg);

//任务优先级
extern const OS_PRIO HALF_SECOND_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE HALF_SECOND_STK_SIZE;

//任务控制块
extern OS_TCB HalfSecondTaskTCB;

//任务堆栈
extern __align(8) CPU_STK HALF_SECOND_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Waiting_Connect_Task(void *p_arg);

//任务优先级
extern const OS_PRIO WAITING_CONNECT_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE WAITING_CONNECT_STK_SIZE;

//任务控制块
extern OS_TCB WAITINGCONNECTaskTCB;

//任务堆栈
extern __align(8) CPU_STK WAITING_CONNECT_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Robot_Location_Update_Task(void *p_arg);

//任务优先级
extern const OS_PRIO ROBOT_LOCATION_UPDATE_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE ROBOT_LOCATION_UPDATE_STK_SIZE;

//任务控制块
extern OS_TCB RobotLocationUpdateTaskTCB;

//任务堆栈
extern __align(8) CPU_STK ROBOT_LOCATION_UPDATE_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Line_Adjust_Task(void *p_arg);

//任务优先级
extern const OS_PRIO LINE_ADJUST_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE LINE_ADJUST_STK_SIZE;

//任务控制块
extern OS_TCB LINE_ADJUST_TASK_TCB;

//任务堆栈
extern __align(8) CPU_STK LINE_ADJUST_TASK_STK[];
/******************************************************************************/



/******************************************************************************/
//任务函数
void Runto_Junction_Task(void *p_arg);

//任务优先级
extern const OS_PRIO RUNTO_JUNCTION_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE RUNTO_JUNCTION_STK_SIZE;

//任务控制块
extern OS_TCB RUNTO_JUNCTION_TASK_TCB;

//任务堆栈
extern __align(8) CPU_STK RUNTO_JUNCTION_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void ANO_DT_Task(void *p_arg);

//任务优先级
extern const OS_PRIO ANO_DT_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE ANO_DT_STK_SIZE;

//任务控制块
extern OS_TCB ANODTTaskTCB;

//任务堆栈
extern __align(8) CPU_STK ANO_DT_TASK_STK[];
/******************************************************************************/


/******************************************************************************/
//任务函数
void Locak_Point_Task(void *p_arg);

//任务优先级
extern const OS_PRIO LOCK_POINT_TASK_PRIO;

//任务堆栈大小
extern const CPU_STK_SIZE LOCK_POINT_STK_SIZE;

//任务控制块
extern OS_TCB TLOCK_POINTTaskTCB;

//任务堆栈
extern __align(8) CPU_STK LOCK_POINT_TASK_STK[];
/******************************************************************************/

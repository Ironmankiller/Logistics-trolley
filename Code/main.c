#include <stdint.h>
#include <stdio.h>
#include "BSP\My_BSP.h"
#include "APP\Task_Loop\Task_Loop.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\Time\Time.h"
#include "BSP\Motor_Drive\Step_Moto.h"
#include "BSP\GY953\GY953.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\Mecanum\Mecanum.h"

static void System_Init(void);       // 系统初始化
/**********************************************
* 主函数 
**********************************************/
int main(void)
 {
    System_Init();	    // 系统初始化
    while (1)
    {
        Task_Loop();    // 任务循环
    }
}
 
/********************************************
* 系统初始化
* 包括硬件驱动层的初始化和软件层的初始化
*********************************************/
static void System_Init(void)
{
    /* 硬件驱动层初始化 */
    BSP_Init(115200, 168);
    /* 软件层初始化 */
    //Argument_Init();    // 将数据存入Flash
    //Argument_Load();    // 从Flash中读取参数
    Mecanum_Param_Init();
    Loop_Time_Clear();
    Cycle_Time_Init();
}

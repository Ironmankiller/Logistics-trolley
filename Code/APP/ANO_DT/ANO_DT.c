/**
******************************************************************************
* @file    ANO_DT_N.c
* @author  DGVY
* @version v1.0
* @date    2016-11-25
* @brief   匿名地面站的单片机端驱动
*		 	   功能1：通过地面站调节参数，并将修改后的参数存入FLASH
*		 	   功能2：发送变化的参数到地面站，以波形的形式显示出来
*		 	   功能3：将地面站的一些按钮，对应到系统不同功能上
******************************************************************************
* @attention
*	相关联的文件：
*		1.USART - "Code/BSP/MCU/USART/Usartx.h/"
*		1.FLSAH - "Code/BSP/MCU/FLASH/stmflash.h/"
******************************************************************************
* @history
*
******************************************************************************
**/

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>
#include "delay.h"
#include "includes.h"
#include "APP\ANO_DT\ANO_DT_User_Settings.h"
#include "APP\ANO_DT\ANO_DT.h"
#include "BSP\MCU\FLASH\stmflash.h"
#include "BSP\SYSTEM\My_Flag.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct //是否发送标志位
{
    uint8_t send_pid1;
    uint8_t send_pid2;
    uint8_t send_pid3;
    uint8_t send_pid4;
    uint8_t send_pid5;
    uint8_t send_pid6;

    uint8_t angle_calibrate_flag;
    uint8_t acc_xyz_calibrate_flag;

    uint8_t ACC_CALIBRATED;   //加速度计和陀螺仪校准
    uint8_t GYRO_CALIBRATED;

}dt_flag_t;

typedef struct //PID
{
	u32 kp;
	u32 ki;
	u32 kd;
}dt_pid_t;

typedef struct //数据存放区
{
    dt_pid_t PID[18];
}dt_ctrl_t;


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

// 通信UART口定义
#if (ANO_DT_UART_PORT_SELECT == 1)
#include "BSP\SYSTEM\usart\Printf_Uart.h"
#define ANO_DT_UART_PORT USART1
#elif (ANO_DT_UART_PORT_SELECT == 2)
#include "BSP/MCU/UART/usart2.h"
#define ANO_DT_UART_PORT USART2
#elif (ANO_DT_UART_PORT_SELECT == 3)
#include "BSP/MCU/UART/usart3.h"
#define ANO_DT_UART_PORT USART3
#elif (ANO_DT_UART_PORT_SELECT == 4)
#include "BSP/MCU/UART/uart4.h"
#define ANO_DT_UART_PORT UART4
#else
#error "Please define ANO_DT_UART_PORT!!!"
#endif

//数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等
//需要把数据拆分成单独字节进行发送
#define BYTE0(dwTemp)       ( *( (char *)(&dwTemp) + 0) )
#define BYTE1(dwTemp)       ( *( (char *)(&dwTemp) + 1) )
#define BYTE2(dwTemp)       ( *( (char *)(&dwTemp) + 2) )
#define BYTE3(dwTemp)       ( *( (char *)(&dwTemp) + 3) )


// 参数缩放比例的倒数
#define ANO_DT_PARAMETER_SCALINT_RECIPROCAL (1/ANO_DT_PARAMETER_SCALING)
#define ANO_DT_CTRL_TYPE      (uint32_t)   //发送到上位机的参数类型


/* Private variables ---------------------------------------------------------*/

enum pid { //数据数组的序号。
    PID1 = 0,
    PID2,
    PID3,
    PID4,
    PID5,
    PID6,
    PID7,
    PID8,
    PID9,
    PID10,
    PID11,
    PID12,
    PID13,
    PID14,
    PID15,
    PID16,
    PID17,
    PID18
};

static dt_flag_t f;				    //需要发送数据的标志
static dt_ctrl_t ctrl_1;       	    //存放数据--中转
static uint8_t data_to_send[50];	//发送数据缓存
static uint8_t ANO_DT_RxBuffer[50]; //接收数据缓存
static uint8_t ANO_DT_Rx_Cnt = 0;   //接收数据计数
Flag Flag_ANO_DT_Data = Not_Ready;  //执行标志位


/* Private function prototypes -----------------------------------------------*/

static void ANO_DT_Data_Exchange(void);
static uint8_t ANO_DT_Data_Receive_Prepare(uint8_t data);
static void ANO_DT_Data_Receive_Anl(uint8_t *data_buf, uint8_t num);
static void ANO_DT_Send_Data(uint8_t *dataToSend, uint8_t length);
static void ANO_DT_Send_PID(uint8_t group,
                            float p1_p, float p1_i, float p1_d,
                            float p2_p, float p2_i, float p2_d,
                            float p3_p, float p3_i, float p3_d);
static void Argument_Save(void);
static void Argument_To_Ctrl(void);
static void Ctrl_To_Argument(void);
static void Ctrl_Set_Zoro(void);
static void Flash_ReadPID(void);
static void Flash_SavePID(void);


/* Private functions ---------------------------------------------------------*/


/*****************************************************
* 执行函数
* 在主函数中重复调用此函数进行查询
* 执行上位机发送过来的指令
*****************************************************/
void ANO_DT_Do(void)
{
    if (Flag_ANO_DT_Data == Ready)
    {
        ANO_DT_Data_Receive_Anl(ANO_DT_RxBuffer, ANO_DT_Rx_Cnt);
        ANO_DT_Rx_Cnt = 0;
        memset(&ANO_DT_RxBuffer, 0, sizeof(uint8_t) * 50);
        Flag_ANO_DT_Data = Not_Ready;
    }
}

/*****************************************************
* 初始化参数
* 此函数在第一次刷数据到Flash时调用，
* 之后就调用 void Argument_Load(void) 函数即可
*****************************************************/
void Argument_Init(void)
{
    Ctrl_Set_Zoro();
    Argument_To_Ctrl();
    Flash_SavePID();
}

/**
* @name   Argument_Load
* @param  None
* @retval None
* @brief  从FLASH里读取PID数据到中间变量，并将中间变量的值赋给控制系统
* 		当FLASH中刷过数据后，单片机系统初始化调用此函数
*/
void Argument_Load(void)
{
    Ctrl_Set_Zoro();
    Flash_ReadPID();
    Ctrl_To_Argument();
}

/*********************************************
* 通过指令保存参数
*********************************************/
void Commond_Save(void)
{
    Argument_To_Ctrl();
    Flash_SavePID();
}

/**
* @name   ANO_DT_Send_Data
* @param  数据帧及长度
* @retval None
* @brief  发送数据
*			移植时，用户应根据自身应用的情况，根据使用的通信方式，实现此函数
*/
static void ANO_DT_Send_Data(uint8_t *dataToSend, uint8_t length)
{
#if (ANO_DT_UART_PORT_SELECT == 1)
    u1_putbuff(data_to_send, length);       //通过UART1发送
#elif (ANO_DT_UART_PORT_SELECT == 2)
    u2_putbuff(data_to_send, length);       //通过UART2发送
#elif (ANO_DT_UART_PORT_SELECT == 3)
    u3_putbuff(data_to_send, length);       //通过UART3发送
#elif (ANO_DT_UART_PORT_SELECT == 4)
    u4_putbuff(data_to_send, length);       //通过UART4发送
#endif
}

////------------------------------------------------------------------------------
//// Interrupt Function
//// 中断函数
////------------------------------------------------------------------------------
//#if (ANO_DT_UART_PORT_SELECT == 1)
//void USART1_IRQHandler(void)
//#elif (ANO_DT_UART_PORT_SELECT == 2)
//void USART2_IRQHandler(void)
//#elif (ANO_DT_UART_PORT_SELECT == 3)
//void USART3_IRQHandler(void)
//#elif (ANO_DT_UART_PORT_SELECT == 4)
//void USART4_IRQHandler(void)
//#endif
//{
//    if (USART_GetFlagStatus(ANO_DT_UART_PORT, USART_FLAG_RXNE) != RESET)  //接收到数据
//    {
//        uint16_t res = 0;
//        res = USART_ReceiveData(ANO_DT_UART_PORT);
//        Flag_ANO_DT_Data = (Flag)ANO_DT_Data_Receive_Prepare((uint8_t)res);
//    }
//        USART_ClearFlag(ANO_DT_UART_PORT, USART_FLAG_RXNE);
//        USART_GetITStatus(ANO_DT_UART_PORT, USART_IT_RXNE);
//}

/**
* @name   ANO_DT_Data_Exchange
* @param  None
* @retval None
* @brief  函数处理各种数据发送请求
*/
static void ANO_DT_Data_Exchange(void)
{

    if (f.send_pid1)
    {
        f.send_pid1 = 0;
        ANO_DT_Send_PID(1,
            ctrl_1.PID[PID1].kp, ctrl_1.PID[PID1].ki, ctrl_1.PID[PID1].kd,

            ctrl_1.PID[PID2].kp, ctrl_1.PID[PID2].ki, ctrl_1.PID[PID2].kd,

            ctrl_1.PID[PID3].kp, ctrl_1.PID[PID3].ki, ctrl_1.PID[PID3].kd);
    }

    if (f.send_pid2)
    {
        f.send_pid2 = 0;
        ANO_DT_Send_PID(2,
            ctrl_1.PID[PID4].kp, ctrl_1.PID[PID4].ki, ctrl_1.PID[PID4].kd,

            ctrl_1.PID[PID5].kp, ctrl_1.PID[PID5].ki, ctrl_1.PID[PID5].kd,

            ctrl_1.PID[PID6].kp, ctrl_1.PID[PID6].ki, ctrl_1.PID[PID6].kd);
    }

    if (f.send_pid3)
    {
        f.send_pid3 = 0;
        ANO_DT_Send_PID(3,
            ctrl_1.PID[PID7].kp, ctrl_1.PID[PID7].ki, ctrl_1.PID[PID7].kd,

            ctrl_1.PID[PID8].kp, ctrl_1.PID[PID8].ki, ctrl_1.PID[PID8].kd,

            ctrl_1.PID[PID9].kp, ctrl_1.PID[PID9].ki, ctrl_1.PID[PID9].kd);
    }

    if (f.send_pid4)
    {
        f.send_pid4 = 0;
        ANO_DT_Send_PID(4,
            ctrl_1.PID[PID10].kp, ctrl_1.PID[PID10].ki, ctrl_1.PID[PID10].kd,

            ctrl_1.PID[PID11].kp, ctrl_1.PID[PID11].ki, ctrl_1.PID[PID11].kd,

            ctrl_1.PID[PID12].kp, ctrl_1.PID[PID12].ki, ctrl_1.PID[PID12].kd);
    }

    if (f.send_pid5)
    {
        f.send_pid5 = 0;
        ANO_DT_Send_PID(5,
            ctrl_1.PID[PID13].kp, ctrl_1.PID[PID13].ki, ctrl_1.PID[PID13].kd,

            ctrl_1.PID[PID14].kp, ctrl_1.PID[PID14].ki, ctrl_1.PID[PID14].kd,

            ctrl_1.PID[PID15].kp, ctrl_1.PID[PID15].ki, ctrl_1.PID[PID15].kd);
    }

    if (f.send_pid6)
    {
        f.send_pid6 = 0;
        ANO_DT_Send_PID(6,
            ctrl_1.PID[PID16].kp, ctrl_1.PID[PID16].ki, ctrl_1.PID[PID16].kd,

            ctrl_1.PID[PID17].kp, ctrl_1.PID[PID17].ki, ctrl_1.PID[PID17].kd,

            ctrl_1.PID[PID18].kp, ctrl_1.PID[PID18].ki, ctrl_1.PID[PID18].kd);
    }

}

/**
* @name   ANO_DT_Send_Check
* @param  输入参数均为校验值
* @retval None
* @brief  为发送的数据添加校验位
*/
static void ANO_DT_Send_Check(uint8_t head, uint8_t check_sum)
{
    data_to_send[0] = 0xAA;
    data_to_send[1] = 0xAA;
    data_to_send[2] = 0xEF;
    data_to_send[3] = 2;
     data_to_send[4] = head;
    data_to_send[5] = check_sum;


    uint8_t sum = 0;
    for (uint8_t i = 0; i<6; i++)
        sum += data_to_send[i];
    data_to_send[6] = sum;

    ANO_DT_Send_Data(data_to_send, 7);
}

/**
* @name   ANO_DT_Data_Receive_Prepare
* @param  协议帧
* @retval None
* @brief  协议预解析，根据协议的格式，
* 		将收到的数据进行一次格式性解析，
* 		格式正确的话再进行数据解析。
* 		移植时，此函数应由用户根据自身使用的通信方式自行调用，
* 		比如串口每收到一字节数据，则调用此函数一次
*/
uint8_t ANO_DT_Data_Receive_Prepare(uint8_t data)
{
    static uint8_t _data_len = 0;
    static uint8_t state = 0;

    if (state == 0 && data == 0xAA)
    {
        state = 1;
        ANO_DT_RxBuffer[0] = data;
    }
    else if (state == 1 && data == 0xAF)
    {
        state = 2;
        ANO_DT_RxBuffer[1] = data;
    }
    else if (state == 2 && data<0XF1)
    {
        state = 3;
        ANO_DT_RxBuffer[2] = data;
    }
    else if (state == 3 && data<50)
    {
        state = 4;
        ANO_DT_RxBuffer[3] = data;
        _data_len = data;
        ANO_DT_Rx_Cnt = 0;
    }
    else if (state == 4 && _data_len>0)
    {
        _data_len--;
        ANO_DT_RxBuffer[4 + ANO_DT_Rx_Cnt++] = data;
        if (_data_len == 0)
            state = 5;
    }
    else if (state == 5)
    {
        state = 0;
        ANO_DT_RxBuffer[4 + ANO_DT_Rx_Cnt] = data;
        //ANO_DT_Data_Receive_Anl(ANO_DT_RxBuffer, ANO_DT_Rx_Cnt);
        return Ready;

    }
    else
        state = 0;

    return Not_Ready;
}

/**
* @name   ANO_DT_Data_Receive_Anl
* @param  函数参数是符合协议格式的一个数据帧及其长度
* @retval None
* @brief  该函数会首先对协议数据进行校验
* 		校验通过后对数据进行解析，实现相应功能。
* 		此函数不需要用户自行调用，由函数ANO_DT_Data_Receive_Prepare自动调用
*/
static void ANO_DT_Data_Receive_Anl(uint8_t *data_buf, uint8_t num)
{
    num = num + 5;
    uint8_t sum = 0;
    for (uint8_t i = 0; i<(num - 1); i++)
        sum += *(data_buf + i);
    if (!(sum == *(data_buf + num - 1)))		return;		//判断sum
    if (!(*(data_buf) == 0xAA && *(data_buf + 1) == 0xAF))		return;		//判断帧头

    if (*(data_buf + 2) == 0X01)//校准，没有用到，避免报错先注释
    {
        if (*(data_buf + 4) == 0X01)  //01：加速度校准
        {
            ANO_DT_ACCELEROMETER_ADJUSTING();
            f.ACC_CALIBRATED = 1;
        }
        if (*(data_buf + 4) == 0X02)  //02：陀螺仪校准
        {
            ANO_DT_GYRO_CALIBRATED();
            f.GYRO_CALIBRATED = 1;
        }
        if (*(data_buf + 4) == 0X03)  //03：6面校准
        {
            ANO_DT_3D_CALIBRATED();
        }
        if (*(data_buf + 4) == 0X04)  //04：罗盘校准
        {
            ANO_DT_COMPASS_CALIBRATED();
            f.acc_xyz_calibrate_flag = 1;  //上位机控制  加速度 XYZ 三轴标定。。
        }
        if (*(data_buf + 4) == 0X05)  //05：气压计校准 用于---角度校准
        {
            ANO_DT_BAROMETER_CALIBRATED();
            f.angle_calibrate_flag = 1;  //上位机控制目标角度校准
        }
        if (*(data_buf + 4) == 0XA0)  //飞控锁定
        {
            //CarControlStop();   //---停车
        }
        if (*(data_buf + 4) == 0XA1)  //飞控解锁
        {
            //CarControlStart();    //上位机控制发车
        }
    }

    if (*(data_buf + 2) == 0X02)//上位机读取数据，重要！！！
    {
        if (*(data_buf + 4) == 0X01)//读取六组PID的值
        {
            //Argument_Load();//加载flash的值，并且分配
            f.send_pid1 = 1;//18组PID 分成 6次 发送
            f.send_pid2 = 1;
            f.send_pid3 = 1;
            f.send_pid4 = 1;
            f.send_pid5 = 1;
            f.send_pid6 = 1;
            ANO_DT_Data_Exchange();//原来是放在中断里面的，由于与上位机的数据交换在发车之前，中断未开启，所以函数可以放在这里
        }
        if (*(data_buf + 4) == 0X02)//没有用到
        {

        }
        if (*(data_buf + 4) == 0XA0)	//读取版本信息
        {
            //f.send_version = 1;
        }
        if (*(data_buf + 4) == 0XA1)	//恢复默认参数
        {
            //未定义回复默认参数函数
            Argument_Init();
            f.send_pid1 = 1;//恢复默认设置后发送数据到上位机。
            f.send_pid2 = 1;
            f.send_pid3 = 1;
            f.send_pid4 = 1;
            f.send_pid5 = 1;
            f.send_pid6 = 1;
            ANO_DT_Data_Exchange();
            //CtrlPID_ResetToFactorySetup();
        }
    }

    if (*(data_buf + 2) == 0X10)	//写入修改 1~3 组PID				//PID1
    {
        ctrl_1.PID[PID1].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID1].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID1].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID2].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID2].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID2].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID3].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID3].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID3].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();//存到flash里面
    }
    if (*(data_buf + 2) == 0X11)//写入修改 4~6 组PID					//PID2
    {
        ctrl_1.PID[PID4].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID4].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID4].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID5].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID5].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID5].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID6].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID6].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID6].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();
    }
    if (*(data_buf + 2) == 0X12)//写入修改 7~9 组PID					//PID3
    {
        ctrl_1.PID[PID7].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID7].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID7].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID8].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID8].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID8].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID9].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID9].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID9].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();
    }
    if (*(data_buf + 2) == 0X13)								//PID4
    {
        ctrl_1.PID[PID10].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID10].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID10].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID11].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID11].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID11].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID12].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID12].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID12].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();
    }
    if (*(data_buf + 2) == 0X14)								//PID5
    {
        ctrl_1.PID[PID13].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID13].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID13].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID14].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID14].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID14].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID15].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID15].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID15].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();
    }
    if (*(data_buf + 2) == 0X15)								//PID6
    {
        ctrl_1.PID[PID16].kp = ((vu16)(*(data_buf + 4) << 8) | *(data_buf + 5));
        ctrl_1.PID[PID16].ki = ((vu16)(*(data_buf + 6) << 8) | *(data_buf + 7));
        ctrl_1.PID[PID16].kd = ((vu16)(*(data_buf + 8) << 8) | *(data_buf + 9));
        ctrl_1.PID[PID17].kp = ((vu16)(*(data_buf + 10) << 8) | *(data_buf + 11));
        ctrl_1.PID[PID17].ki = ((vu16)(*(data_buf + 12) << 8) | *(data_buf + 13));
        ctrl_1.PID[PID17].kd = ((vu16)(*(data_buf + 14) << 8) | *(data_buf + 15));
        ctrl_1.PID[PID18].kp = ((vu16)(*(data_buf + 16) << 8) | *(data_buf + 17));
        ctrl_1.PID[PID18].ki = ((vu16)(*(data_buf + 18) << 8) | *(data_buf + 19));
        ctrl_1.PID[PID18].kd = ((vu16)(*(data_buf + 20) << 8) | *(data_buf + 21));
        ANO_DT_Send_Check(*(data_buf + 2), sum);
        Argument_Save();
        ANO_DT_DATA_RECEIVE_WRITE_SUCCEED();		//全部数据成功写入单片机
    }
}

/**
* @name   ANO_DT_Send_PID
* @param  要发送的PID数据
* @retval None
* @brief  向上位机发送PID参数
*/
static void ANO_DT_Send_PID(uint8_t group,
    float p1_p, float p1_i, float p1_d,
    float p2_p, float p2_i, float p2_d,
    float p3_p, float p3_i, float p3_d)
{
    uint8_t _cnt = 0;
    vs16 _temp;

    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0x10 + group - 1;
    data_to_send[_cnt++] = 0;


    _temp = p1_p;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p1_i;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p1_d;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_p;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_i;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p2_d;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_p;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_i;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = p3_d;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    data_to_send[3] = _cnt - 4;

    uint8_t sum = 0;
    for (uint8_t i = 0; i<_cnt; i++)
        sum += data_to_send[i];

    data_to_send[_cnt++] = sum;

    ANO_DT_Send_Data(data_to_send, _cnt);
}

/**
* @name   ANO_DT_Send_Senser
* @param  各传感器的数据
* @retval None
* @brief  传感器数据---此处用于给示波器发送数据
*/
void ANO_DT_Send_Senser(s16 a_x, s16 a_y, s16 a_z,
    s16 g_x, s16 g_y, s16 g_z,
    s16 m_x, s16 m_y, s16 m_z
    , s32 bar)
{
    uint8_t _cnt = 0;
    vs16 _temp;

    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0x02;
    data_to_send[_cnt++] = 0;

    _temp = a_x;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = a_y;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = a_z;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    _temp = g_x;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = g_y;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = g_z;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    _temp = m_x;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = m_y;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = m_z;
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    data_to_send[3] = _cnt - 4;

    uint8_t sum = 0;
    for (uint8_t i = 0; i<_cnt; i++)
        sum += data_to_send[i];
    data_to_send[_cnt++] = sum;

    ANO_DT_Send_Data(data_to_send, _cnt);
}

/**
* @name   ANO_DT_Send_Status
* @param  尚未看懂
* @retval None
* @brief  发送 姿态
*/
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw,
    s32 alt, uint8_t fly_model, uint8_t armed)
{
    uint8_t _cnt = 0;
    vs16 _temp;
    vs32 _temp2 = alt;

    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0xAA;
    data_to_send[_cnt++] = 0x01;
    data_to_send[_cnt++] = 0;

    _temp = (int)(angle_rol * 100);
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = (int)(angle_pit * 100);
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);
    _temp = (int)(angle_yaw * 100);
    data_to_send[_cnt++] = BYTE1(_temp);
    data_to_send[_cnt++] = BYTE0(_temp);

    data_to_send[_cnt++] = BYTE3(_temp2);
    data_to_send[_cnt++] = BYTE2(_temp2);
    data_to_send[_cnt++] = BYTE1(_temp2);
    data_to_send[_cnt++] = BYTE0(_temp2);

    data_to_send[_cnt++] = fly_model;

    data_to_send[_cnt++] = armed;

    data_to_send[3] = _cnt - 4;

    uint8_t sum = 0;
    for (uint8_t i = 0; i<_cnt; i++)
        sum += data_to_send[i];
    data_to_send[_cnt++] = sum;

    ANO_DT_Send_Data(data_to_send, _cnt);
}

/**
* @name   Flash_SavePID
* @param  None
* @retval None
* @brief  储存PID数据到FLASH里
*/
static void Flash_SavePID(void)
{
    u8 var;

    u32 pid_flash_buf[54];
    memset(pid_flash_buf, 0, sizeof(pid_flash_buf));

    //数据缓存到pid_flash_buf中
    for (var = 0; var < 18; var++)
    {
        *(pid_flash_buf + 3 * var + 0) = ctrl_1.PID[var].kp;
        *(pid_flash_buf + 3 * var + 1) = ctrl_1.PID[var].ki;
        *(pid_flash_buf + 3 * var + 2) = ctrl_1.PID[var].kd;
    }

    //写入flash操作,擦除数据已经包含在函数内
    STMFLASH_Write(ANO_DT_PID_DATA_SAVE_FLASH_ADDRESS, pid_flash_buf, 54);
}

/**
* @name   Flash_ReadPID
* @param  None
* @retval None
* @brief  从FLASH里读取PID数据
*/
static void Flash_ReadPID(void)
{
    u8 var;

    u32 pid_flash_buf[54];
    memset(pid_flash_buf, 0, sizeof(pid_flash_buf));

    STMFLASH_Read(ANO_DT_PID_DATA_SAVE_FLASH_ADDRESS, pid_flash_buf, 54);

    //从缓存中读取数据
    for (var = 0; var < 18; var++)
    {
		ctrl_1.PID[var].kp = (u32) * (pid_flash_buf + 3 * var + 0);
		ctrl_1.PID[var].ki = (u32) * (pid_flash_buf + 3 * var + 1);
		ctrl_1.PID[var].kd = (u32) * (pid_flash_buf + 3 * var + 2);
    }
}

/**
* @name   Argument_Save
* @param  None
* @retval None
* @brief  将PID数据存入FLASH
*/
static void Argument_Save(void)
{
    Flash_SavePID();

    Ctrl_To_Argument();
}

// 设置Ctrl_1为0
static void Ctrl_Set_Zoro(void)
{
    memset(&ctrl_1, 0, sizeof(dt_ctrl_t));
}

/**
* @name   Argument_To_Ctrl
* @param  None
* @retval None
* @brief  单片机内的值给中间变量
*/

static void Argument_To_Ctrl(void)
{
#if defined(USE_PID_01)
    ctrl_1.PID[PID1].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_01_P); //PID1
    ctrl_1.PID[PID1].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_01_I);
    ctrl_1.PID[PID1].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_01_D);
#endif

#if defined(USE_PID_02)
    ctrl_1.PID[PID2].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_02_P); //PID2
    ctrl_1.PID[PID2].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_02_I);
    ctrl_1.PID[PID2].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_02_D);
#endif

#if defined(USE_PID_03)
    ctrl_1.PID[PID3].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_03_P); //PID3
    ctrl_1.PID[PID3].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_03_I);
    ctrl_1.PID[PID3].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_03_D);
#endif

#if defined(USE_PID_04)
    ctrl_1.PID[PID4].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_04_P); //PID4
    ctrl_1.PID[PID4].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_04_I);
    ctrl_1.PID[PID4].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_04_D);
#endif

#if defined(USE_PID_05)
    ctrl_1.PID[PID5].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_05_P); //PID5
    ctrl_1.PID[PID5].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_05_I);
    ctrl_1.PID[PID5].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_05_D);
#endif

#if defined(USE_PID_06)
    ctrl_1.PID[PID6].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_06_P); //PID6
    ctrl_1.PID[PID6].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_06_I);
    ctrl_1.PID[PID6].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_06_D);
#endif

#if defined(USE_PID_07)
    ctrl_1.PID[PID7].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_07_P); //PID7
    ctrl_1.PID[PID7].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_07_I);
    ctrl_1.PID[PID7].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_07_D);
#endif

#if defined(USE_PID_08)
    ctrl_1.PID[PID8].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_08_P); //PID8
    ctrl_1.PID[PID8].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_08_I);
    ctrl_1.PID[PID8].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_08_D);
#endif

#if defined(USE_PID_09)
    ctrl_1.PID[PID9].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_09_P); //PID9
    ctrl_1.PID[PID9].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_09_I);
    ctrl_1.PID[PID9].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_09_D);
#endif

#if defined(USE_PID_10)
    ctrl_1.PID[PID10].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_10_P); //PID10
    ctrl_1.PID[PID10].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_10_I);
    ctrl_1.PID[PID10].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_10_D);
#endif

#if defined(USE_PID_11)
    ctrl_1.PID[PID11].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_11_P); //PID11
    ctrl_1.PID[PID11].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_11_I);
    ctrl_1.PID[PID11].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_11_D);
#endif

#if defined(USE_PID_12)
    ctrl_1.PID[PID12].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_12_P); //PID12
    ctrl_1.PID[PID12].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_12_I);
    ctrl_1.PID[PID12].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_12_D);
#endif

#if defined(USE_PID_13)
    ctrl_1.PID[PID13].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_13_P); //PID13
    ctrl_1.PID[PID13].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_13_I);
    ctrl_1.PID[PID13].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_13_D);
#endif

#if defined(USE_PID_14)
    ctrl_1.PID[PID14].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_14_P); //PID14
    ctrl_1.PID[PID14].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_14_I);
    ctrl_1.PID[PID14].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_14_D);
#endif

#if defined(USE_PID_15)
    ctrl_1.PID[PID15].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_15_P); //PID15
    ctrl_1.PID[PID15].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_15_I);
    ctrl_1.PID[PID15].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_15_D);
#endif

#if defined(USE_PID_16)
    ctrl_1.PID[PID16].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_16_P); //PID16
    ctrl_1.PID[PID16].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_16_I);
    ctrl_1.PID[PID16].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_16_D);
#endif

#if defined(USE_PID_17)
    ctrl_1.PID[PID17].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_17_P); //PID17
    ctrl_1.PID[PID17].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_17_I);
    ctrl_1.PID[PID17].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_17_D);
#endif

#if defined(USE_PID_18)
    ctrl_1.PID[PID18].kp = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_18_P); //PID18
    ctrl_1.PID[PID18].ki = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_18_I);
    ctrl_1.PID[PID18].kd = ANO_DT_CTRL_TYPE(ANO_DT_PARAMETER_SCALING * PID_18_D);
#endif
}


/**
* @name   Ctrl_To_Argument
* @param  None
* @retval None
* @brief  中间变量的值给单片机
*/

static void Ctrl_To_Argument(void)
{
#if defined(USE_PID_01)
    //PID1
	PID_01_P = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID1].kp * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
	PID_01_I = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID1].ki * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
	PID_01_D = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID1].kd * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_02)
    //PID2
	PID_02_P = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID2].kp * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
	PID_02_I = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID2].ki * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
	PID_02_D = ANO_DT_PARAMETER_TYPE((double)ctrl_1.PID[PID2].kd * (double)ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_03)
    //PID3
    PID_03_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID3].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_03_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID3].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_03_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID3].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_04)
    //PID4
    PID_04_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID4].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_04_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID4].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_04_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID4].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_05)
    //PID5
    PID_05_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID5].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_05_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID5].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_05_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID5].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_06)
    //PID6
    PID_06_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID6].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_06_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID6].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_06_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID6].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_07)
    //PID7
    PID_07_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID7].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_07_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID7].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_07_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID7].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_08)
    //PID8
    PID_08_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID8].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_08_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID8].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_08_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID8].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_09)
    //PID9
    PID_09_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID9].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_09_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID9].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_09_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID9].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_10)
    //PID10
    PID_10_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID10].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_10_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID10].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_10_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID10].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_11)
    //PID11
    PID_11_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID11].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_11_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID11].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_11_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID11].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_12)
    //PID12
    PID_12_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID12].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_12_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID12].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_12_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID12].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_13)
    //PID13
    PID_13_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID13].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_13_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID13].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_13_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID13].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_14)
    //PID14
    PID_14_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID14].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_14_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID14].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_14_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID14].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_15)
    //PID15
    PID_15_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID15].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_15_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID15].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_15_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID15].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_16)
    //PID16
    PID_16_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID16].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_16_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID16].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_16_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID16].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_17)
    //PID17
    PID_17_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID17].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_17_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID17].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_17_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID17].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif

#if defined(USE_PID_18)
    //PID18
    PID_18_P = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID18].kp * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_18_I = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID18].ki * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
    PID_18_D = ANO_DT_PARAMETER_TYPE((float)ctrl_1.PID[PID18].kd * ANO_DT_PARAMETER_SCALINT_RECIPROCAL);
#endif
}


/******************* (C) COPYRIGHT 2016 ANO TECH *******END OF FILE************/

#ifndef _PSP_HANDER_H_
#define _PSP_HANDER_H_
#include "sys.h"
#include "stdbool.h"
#include "delay.h"


//关于  PS2 手柄的 IO定义
//SPI片选
#define PS2_CS_GPIO_PORT               GPIOE
#define PS2_CS_GPIO_CLK                RCC_AHB1Periph_GPIOE
#define PS2_CS_GPIO_PIN                GPIO_Pin_10
//SPI时钟
#define PS2_CLK_GPIO_PORT              GPIOE
#define PS2_CLK_GPIO_CLK               RCC_AHB1Periph_GPIOE
#define PS2_CLK_GPIO_PIN               GPIO_Pin_8
//SPI D0
#define PS2_DO_GPIO_PORT               GPIOE
#define PS2_DO_GPIO_CLK                RCC_AHB1Periph_GPIOE
#define PS2_DO_GPIO_PIN                GPIO_Pin_12
//SPI D1
#define PS2_DI_GPIO_PORT               GPIOE
#define PS2_DI_GPIO_CLK                RCC_AHB1Periph_GPIOE
#define PS2_DI_GPIO_PIN                GPIO_Pin_14

#define CS_H   PEout(10)=1   // DS
#define CS_L   PEout(10) = 0
#define CLK_H  PEout(8)=1   // CLK
#define CLK_L  PEout(8) = 0
#define DO     PEout(12)    // Command
#define DI     PEin(14)      //DATA
#define DO_H   DO=1
#define DO_L   DO=0

//摇杆
#define Anolog_R_X     Data[5]
#define Anolog_R_Y     Data[6]
#define Anolog_L_X     Data[7]
#define Anolog_L_Y     Data[8]

//These are our button constants
#define PSB_SELECT      1
#define PSB_L3          2
#define PSB_R3          3
#define PSB_START       4
#define PSB_PAD_UP      5
#define PSB_PAD_RIGHT   6
#define PSB_PAD_DOWN    7
#define PSB_PAD_LEFT    8
#define PSB_L2                  9
#define PSB_R2          10
#define PSB_L1          11
#define PSB_R1          12
#define PSB_TRIANGLE    13      //三角形
#define PSB_CIRCLE      14      //圆圈
#define PSB_CROSS       15      //叉叉
#define PSB_SQUARE      16      //方框

#define Anolog_R_X     Data[5]
#define Anolog_R_Y     Data[6]
#define Anolog_L_X     Data[7]
#define Anolog_L_Y     Data[8]
#define  Anolog_Up_Board     200
#define  Anolog_Down_Board   50

#define DelayMs(n)  delay_ms(n)
#define Delay(n) delay_us(n)

extern u8 PS2_ON_Flag;       //PS2开启标志位
extern u8 Data[9];

void InitPS2(void);
u8 PS2_RedLight(void);//判断是否为红灯模式
void PS2_ReadData(void);
void PS2_Cmd(u8 CMD);             //
u8 PS2_DataKey(void);             //键值读取
u8 PS2_AnologData(u8 button); //得到一个摇杆的模拟量
void PS2_ClearData(void);         //清除数据缓冲区
void PS2_Vibration(u8 motor1, u8 motor2);//振动设置motor1  0xFF开，其他关，motor2  0x40~0xFF

void PS2_EnterConfing(void);     //进入配置
void PS2_TurnOnAnalogMode(void); //发送模拟量
void PS2_VibrationMode(void);    //振动设置
void PS2_ExitConfing(void);          //完成配置
void PS2_SetInit(void);              //配置初始化


bool PS2_NewButtonState( u16 button );
bool PS2_Button( u16 button );
bool PS2_ButtonPressed( u16 button );
bool PS2_ButtonReleased( u16 button );

void Remote(void);

#endif

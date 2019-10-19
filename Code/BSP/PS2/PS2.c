/*
 * @Author: spy 
 * @Date: 2019-08-01 20:23:03 
 * @Last Modified by:   spy 
 * @Last Modified time: 2019-08-01 20:23:03 
 */

#include "PS2.h"
#include "sys.h"
#include "BSP\LED\LED.h"
#include "BSP\MCU\UART\usart5.h"
#include "BSP\MCU\UART\usart3.h"
#include "BSP\Mecanum\Mecanum.h"
#include "BSP\Motor_Drive\Moto_Drive.h"
#include "BSP\Motor_Drive\Steer_Moto.h"

u8 PS2_ON_Flag=0;       //PS2开启标志位

u16 Handkey;
u8 Comd[2] = { 0x01, 0x42 };                                            //开始命令。请求数据
u8 Data[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  //数据存储数组
u16 MASK[] =
{
	PSB_SELECT,
	PSB_L3,
	PSB_R3,
	PSB_START,
	PSB_PAD_UP,
	PSB_PAD_RIGHT,
	PSB_PAD_DOWN,
	PSB_PAD_LEFT,
	PSB_L2,
	PSB_R2,
	PSB_L1,
	PSB_R1,
	PSB_TRIANGLE,
	PSB_CIRCLE,
	PSB_CROSS,
	PSB_SQUARE
};              //按键值与按键明
void Remote(void)
{
	short int temp = 0;

	temp = PS2_DataKey();
	switch (temp)
	{
	case PSB_SELECT:                //           1
        Mecanum.state = back;
		break;

	case PSB_START:                //            4
        Mecanum.state = goToDeparture;
		break;
	case PSB_PAD_UP:                //           5
		Mecanum.Target_A = 100;
		Mecanum.Target_B = 100;
		Mecanum.Target_C = 100;
		Mecanum.Target_D = 100;
		break;

	case PSB_PAD_RIGHT:    //             6
		Mecanum.Target_A = -100;
		Mecanum.Target_B = 100;
		Mecanum.Target_C = -100;
		Mecanum.Target_D = 100;
		break;

	case PSB_PAD_DOWN:                //              7
		Mecanum.Target_A = -100;
		Mecanum.Target_B = -100;
		Mecanum.Target_C = -100;
		Mecanum.Target_D = -100;
		break;

	case PSB_PAD_LEFT:                //              8
		Mecanum.Target_A = 100;
		Mecanum.Target_B = -100;
		Mecanum.Target_C = 100;
		Mecanum.Target_D = -100;
		break;

	case PSB_L1:                  //                  9
        arm3(0);
		break;

	case PSB_R2:                //                    10
        arm2(0);
		break;

	case PSB_L2:                //                    11
        arm3(50);
		break;

	case PSB_R1:                //                    12
        arm2(40);
		break;

	case PSB_TRIANGLE:                //              13
        arm4(0);
		break;

	case PSB_CIRCLE:                //                14
        arm1(-90);
		break;

	case PSB_CROSS:                  //               15
        arm4(40);
		break;

	case PSB_SQUARE:                //                 16
        arm1(90);
		break;

	default:
		break;
	}

    //*************以上为 按键部分处理 接下来是摇杆的处理******************/

	
    if(Anolog_R_X > Anolog_Up_Board)//  右手 X方向 右拉
	{
        
        
	}else if(Anolog_R_X < Anolog_Down_Board)//   右手 X 左拉
	{


	}else if(Anolog_R_Y < Anolog_Down_Board) //   右手 Y方向上拉
	{

	}
	else if(Anolog_R_Y > Anolog_Up_Board)// 仅仅 右手 Y方向 下拉
	{

	}

	if(Anolog_L_X > Anolog_Up_Board)// 左手摇杆 右拉
	{
        
	}else if(Anolog_L_X < Anolog_Down_Board) //  左手摇杆  左拉
	{

	}
}


void InitPS2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(PS2_CS_GPIO_CLK | PS2_CLK_GPIO_CLK | PS2_DO_GPIO_CLK, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_Pin = PS2_CS_GPIO_PIN;
	GPIO_Init(PS2_CS_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PS2_CLK_GPIO_PIN;
	GPIO_Init(PS2_CLK_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PS2_DO_GPIO_PIN;
	GPIO_Init(PS2_DO_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = PS2_DI_GPIO_PIN;
	GPIO_Init(PS2_DI_GPIO_PORT, &GPIO_InitStructure);
	PS2_SetInit();           //配配置初始化,配置“红绿灯模式”，并选择是否可以修改
	PS2_ON_Flag = 1;
}


//向手柄发送命令
void PS2_Cmd(u8 CMD)
{
	volatile u16 ref = 0x01;

	Data[1] = 0;
	for (ref = 0x01; ref < 0x0100; ref <<= 1)
	{
		if (ref&CMD) {
			DO_H;                   //输出以为控制位
		}else {
			DO_L;
		}
		Delay(10);
		CLK_L;
		Delay(40);
		CLK_H;
		if (DI) {
			Data[1] = ref|Data[1];
		}
		Delay(10);
	}
}


//判断是否为红灯模式
//返回值；0，红灯模式
//		  其他，其他模式
u8 PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);       //开始命令
	PS2_Cmd(Comd[1]);       //请求数据
	CS_H;
	if (Data[1] == 0X73) {
		return (0);
	}else{
		return (1);
	}
}


//读取手柄数据
void PS2_ReadData(void)
{
	volatile u8 byte;
	volatile u16 ref;

	CS_L;
	Delay(10);
	PS2_Cmd(Comd[0]);                       //开始命令
	PS2_Cmd(Comd[1]);                       //请求数据
	for (byte = 2; byte < 9; byte++)        //开始接受数据
	{
		for (ref = 0x01; ref < 0x100; ref <<= 1)
		{
			CLK_L;
			Delay(50);
			CLK_H;
			if (DI) {
				Data[byte] = ref|Data[byte];
			}
			Delay(20);
		}
		Delay(40);
	}
	CS_H;
}


//对读出来的PS2的数据进行处理      只处理了按键部分         默认数据是红灯模式  只有一个按键按下时
//按下为0， 未按下为1

u16 LastHandkey = 0xFFFF;
u8 PS2_DataKey()
{
	u8 index;

	PS2_ClearData();
	PS2_ReadData();
	LastHandkey = Handkey;
	Handkey = (Data[4]<<8)|Data[3];     //这是16个按键  按下为0， 未按下为1
	if (Handkey == 0xff) {
		return (0);
	}
	for (index = 0; index < 16; index++)
	{
		if ((Handkey&(1<<(MASK[index]-1))) == 0) {
			return (index+1);
		}
	}
	return (0);         //没有任何按键按下
}

bool PS2_NewButtonState( u16 button )
{
	button = 0x0001u << ( button - 1 ); //输入的button的值是 该按键在数据中所在bit的值+1， 例如 PSB_SELECT 宏的值 是 1， 在数据中的位是0位， 如此类推，
	return ( ( ( LastHandkey ^ Handkey ) & button ) > 0 ); //将上次的按键数据和这次的按键数据进行异或运算，结果就是两次不同的部分会是1，就得到了状态发生了变化的按键
	                                                       //然后在与我们想要检测的按键进行与运算，如果这个按键发生了变化，那么结果就是1， 大于0，所以返回就是true
}

bool PS2_Button( u16 button )
{
	button = 0x0001u << ( button - 1 ); //输入的button的值是 该按键在数据中所在bit的值+1， 例如 PSB_SELECT 宏的值 是 1， 在数据中的位是0位， 如此类推，
	return ( ( (~Handkey) & button ) > 0 ); //按键按下则对应位为0，没按下为1， 将按键数据取反之后，就变成了按键为1，没按下为0
	//再与我们想要检测的按键做与运算，若这个按键被按下，对应位就是1，没按下就是0，返回与0比较的结果，
}

bool PS2_ButtonPressed( u16 button )
{
	return (PS2_NewButtonState( button ) && PS2_Button( button )); //按键被按住，并且这个是按键的一个新的状态，那么就是按键刚被按下
}

bool PS2_ButtonReleased( u16 button )
{
	return ( PS2_NewButtonState( button ) && !PS2_Button( button )); //按键没被按住，并且这个是按键的一个新的状态，那么就是按键刚被松开
}

//得到一个摇杆的模拟量	 范围0~255
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//清除数据缓冲区
void PS2_ClearData()
{
	u8 a;
	for(a=0; a<9; a++)
		Data[a]=0x00;
}
/******************************************************
   Function:    void PS2_Vibration(u8 motor1, u8 motor2)
   Description: 手柄震动函数，
   Calls:		 void PS2_Cmd(u8 CMD);
   Input: motor1:右侧小震动电机 0x00关，其他开
           motor2:左侧大震动电机 0x40~0xFF 电机开，值越大 震动越大
******************************************************/
void PS2_Vibration(u8 motor1, u8 motor2)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01); //开始命令
	PS2_Cmd(0x42);  //请求数据
	PS2_Cmd(0X00);
	PS2_Cmd(motor1);
	PS2_Cmd(motor2);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	Delay(50);
}
//short poll
void PS2_ShortPoll(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x42);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x00);
	CS_H;
	Delay(50);
}
//进入配置
void PS2_EnterConfing(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01);
	PS2_Cmd(0x00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	Delay(50);
}
//发送模式设置
void PS2_TurnOnAnalogMode(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01); //analog=0x01;digital=0x00  软件设置发送模式
	PS2_Cmd(0x03); //Ox03锁存设置，即不可通过按键“MODE”设置模式。
	//0xEE不锁存软件设置，可通过按键“MODE”设置模式。
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	Delay(50);
}
//振动设置
void PS2_VibrationMode(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x4D);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0X01);
	CS_H;
	Delay(50);
}
//完成并保存配置
void PS2_ExitConfing(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x43);
	PS2_Cmd(0X00);
	PS2_Cmd(0x00);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	PS2_Cmd(0x5A);
	CS_H;
	Delay(50);
}
//手柄配置初始化
void PS2_SetInit(void)
{
	DelayMs(10);
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();             //进入配置模式
	PS2_TurnOnAnalogMode(); //“红绿灯”配置模式，并选择是否保存
	PS2_VibrationMode();    //开启震动模式
	PS2_ExitConfing();              //完成并保存配置
}
/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

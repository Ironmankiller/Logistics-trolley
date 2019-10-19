//-----------------------------------------------------------------------------
// Drv_TofMini_Plus.c
//
//  Created on	: 2018-8-01
//      Author	: Divenire
//		version	: V1.0
//		brief	: TF-Mini-Plus 
//-----------------------------------------------------------------------------
// Attention:
//
//	大功率激光使用UART7 对应地板中的 USART2
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Drv_TofMini_Plus.h"
#include "Drv_time.h"
#include "Drv_uart.h"
#include "Drv_uart_User_Setting.h"


//大功率激光使用UART通信
u8 TofMiniPlus_LINKOK = 0;	//0:无效，1：有效，2：测试中
u16 TofMiniPlus_height_cm;

u8 Drv_TofMiniPlus_Init(void)
{

	Uart7_Init(115200);
	TofMiniPlus_LINKOK = 2;
	for (u16 i = 0; i < 1000; i++)
	{
		if (TofMiniPlus_LINKOK == 1)
			break;
		else
			delay_ms(1);
}
	if (TofMiniPlus_LINKOK == 2)
		TofMiniPlus_LINKOK = 0;
	sens_hd_check.tof_ok = TofMiniPlus_LINKOK;
	return TofMiniPlus_LINKOK;

}

//大功率激光
void Drv_TofMiniPlus_RunTask(u8 data)
{
	static u8 tmp[9];
	static u8 sta = 0;
	static u8 cnt = 0;

	if (sta == 0 && data == 0x59)
	{
		tmp[0] = 0x59;
		sta = 1;
	}
	else if (sta == 1)
	{
		if (data == 0x59)
		{
			tmp[1] = 0x59;
			sta = 2;
			cnt = 2;
		}
		else
			sta = 0;
	}
	else if (sta == 2)
	{
		tmp[cnt++] = data;
		if (cnt >= 9)
		{
			u8 sum = 0;
			for (u8 i = 0; i < 8; i++)
				sum += tmp[i];
			if (sum == tmp[8])	//校验通过
			{
				if (TofMiniPlus_LINKOK == 2)
					TofMiniPlus_LINKOK = 1;
				TofMiniPlus_height_cm = (tmp[2] + ((u16)tmp[3] << 8));
			}
			sta = 0;
			cnt = 0;
		}
	}
}

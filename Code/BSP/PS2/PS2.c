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

u8 PS2_ON_Flag=0;       //PS2������־λ

u16 Handkey;
u8 Comd[2] = { 0x01, 0x42 };                                            //��ʼ�����������
u8 Data[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };  //���ݴ洢����
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
};              //����ֵ�밴����
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

    //*************����Ϊ �������ִ��� ��������ҡ�˵Ĵ���******************/

	
    if(Anolog_R_X > Anolog_Up_Board)//  ���� X���� ����
	{
        
        
	}else if(Anolog_R_X < Anolog_Down_Board)//   ���� X ����
	{


	}else if(Anolog_R_Y < Anolog_Down_Board) //   ���� Y��������
	{

	}
	else if(Anolog_R_Y > Anolog_Up_Board)// ���� ���� Y���� ����
	{

	}

	if(Anolog_L_X > Anolog_Up_Board)// ����ҡ�� ����
	{
        
	}else if(Anolog_L_X < Anolog_Down_Board) //  ����ҡ��  ����
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
	PS2_SetInit();           //�����ó�ʼ��,���á����̵�ģʽ������ѡ���Ƿ�����޸�
	PS2_ON_Flag = 1;
}


//���ֱ���������
void PS2_Cmd(u8 CMD)
{
	volatile u16 ref = 0x01;

	Data[1] = 0;
	for (ref = 0x01; ref < 0x0100; ref <<= 1)
	{
		if (ref&CMD) {
			DO_H;                   //�����Ϊ����λ
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


//�ж��Ƿ�Ϊ���ģʽ
//����ֵ��0�����ģʽ
//		  ����������ģʽ
u8 PS2_RedLight(void)
{
	CS_L;
	PS2_Cmd(Comd[0]);       //��ʼ����
	PS2_Cmd(Comd[1]);       //��������
	CS_H;
	if (Data[1] == 0X73) {
		return (0);
	}else{
		return (1);
	}
}


//��ȡ�ֱ�����
void PS2_ReadData(void)
{
	volatile u8 byte;
	volatile u16 ref;

	CS_L;
	Delay(10);
	PS2_Cmd(Comd[0]);                       //��ʼ����
	PS2_Cmd(Comd[1]);                       //��������
	for (byte = 2; byte < 9; byte++)        //��ʼ��������
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


//�Զ�������PS2�����ݽ��д���      ֻ�����˰�������         Ĭ�������Ǻ��ģʽ  ֻ��һ����������ʱ
//����Ϊ0�� δ����Ϊ1

u16 LastHandkey = 0xFFFF;
u8 PS2_DataKey()
{
	u8 index;

	PS2_ClearData();
	PS2_ReadData();
	LastHandkey = Handkey;
	Handkey = (Data[4]<<8)|Data[3];     //����16������  ����Ϊ0�� δ����Ϊ1
	if (Handkey == 0xff) {
		return (0);
	}
	for (index = 0; index < 16; index++)
	{
		if ((Handkey&(1<<(MASK[index]-1))) == 0) {
			return (index+1);
		}
	}
	return (0);         //û���κΰ�������
}

bool PS2_NewButtonState( u16 button )
{
	button = 0x0001u << ( button - 1 ); //�����button��ֵ�� �ð���������������bit��ֵ+1�� ���� PSB_SELECT ���ֵ �� 1�� �������е�λ��0λ�� ������ƣ�
	return ( ( ( LastHandkey ^ Handkey ) & button ) > 0 ); //���ϴεİ������ݺ���εİ������ݽ���������㣬����������β�ͬ�Ĳ��ֻ���1���͵õ���״̬�����˱仯�İ���
	                                                       //Ȼ������������Ҫ���İ������������㣬���������������˱仯����ô�������1�� ����0�����Է��ؾ���true
}

bool PS2_Button( u16 button )
{
	button = 0x0001u << ( button - 1 ); //�����button��ֵ�� �ð���������������bit��ֵ+1�� ���� PSB_SELECT ���ֵ �� 1�� �������е�λ��0λ�� ������ƣ�
	return ( ( (~Handkey) & button ) > 0 ); //�����������ӦλΪ0��û����Ϊ1�� ����������ȡ��֮�󣬾ͱ���˰���Ϊ1��û����Ϊ0
	//����������Ҫ���İ����������㣬��������������£���Ӧλ����1��û���¾���0��������0�ȽϵĽ����
}

bool PS2_ButtonPressed( u16 button )
{
	return (PS2_NewButtonState( button ) && PS2_Button( button )); //��������ס����������ǰ�����һ���µ�״̬����ô���ǰ����ձ�����
}

bool PS2_ButtonReleased( u16 button )
{
	return ( PS2_NewButtonState( button ) && !PS2_Button( button )); //����û����ס����������ǰ�����һ���µ�״̬����ô���ǰ����ձ��ɿ�
}

//�õ�һ��ҡ�˵�ģ����	 ��Χ0~255
u8 PS2_AnologData(u8 button)
{
	return Data[button];
}

//������ݻ�����
void PS2_ClearData()
{
	u8 a;
	for(a=0; a<9; a++)
		Data[a]=0x00;
}
/******************************************************
   Function:    void PS2_Vibration(u8 motor1, u8 motor2)
   Description: �ֱ��𶯺�����
   Calls:		 void PS2_Cmd(u8 CMD);
   Input: motor1:�Ҳ�С�𶯵�� 0x00�أ�������
           motor2:�����𶯵�� 0x40~0xFF �������ֵԽ�� ��Խ��
******************************************************/
void PS2_Vibration(u8 motor1, u8 motor2)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01); //��ʼ����
	PS2_Cmd(0x42);  //��������
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
//��������
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
//����ģʽ����
void PS2_TurnOnAnalogMode(void)
{
	CS_L;
	Delay(50);
	PS2_Cmd(0x01);
	PS2_Cmd(0x44);
	PS2_Cmd(0X00);
	PS2_Cmd(0x01); //analog=0x01;digital=0x00  ������÷���ģʽ
	PS2_Cmd(0x03); //Ox03�������ã�������ͨ��������MODE������ģʽ��
	//0xEE������������ã���ͨ��������MODE������ģʽ��
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	PS2_Cmd(0X00);
	CS_H;
	Delay(50);
}
//������
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
//��ɲ���������
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
//�ֱ����ó�ʼ��
void PS2_SetInit(void)
{
	DelayMs(10);
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_ShortPoll();
	PS2_EnterConfing();             //��������ģʽ
	PS2_TurnOnAnalogMode(); //�����̵ơ�����ģʽ����ѡ���Ƿ񱣴�
	PS2_VibrationMode();    //������ģʽ
	PS2_ExitConfing();              //��ɲ���������
}
/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

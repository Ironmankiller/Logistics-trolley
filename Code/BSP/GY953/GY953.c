/*
 * @Author: spy 
 * @Date: 2019-07-28 15:13:29 
 * @Last Modified by: spy
 * @Last Modified time: 2019-07-28 20:06:27
 */
#include "GY953.h"
#include "BSP\MPU6050\MPU6050.h"
#include "APP\IMU\ahrs.h"
#include "delay.h"
#include "string.h"

uint8_t stata_reg=0;//接收状态寄存器
//5个数据缓存，对应Acc-Q，缓存长度为7，包含6个数据+1个校验和数据
//缓存长度为9，包含8个四元数数据+1个校验和数据
uint8_t Acc_buf[7]={0},Gyro_buf[7]={0},Mag_buf[7]={0},
Eur_buf[7]={0},Quad_buf[9]={0};

void GY953_Init()
{
    uint8_t send_data[3]={0};
//	send_data[0]=0xa5;
//	send_data[1]=0x15;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);//指令校验和
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送加速度输出指令
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x25;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送陀螺仪数据输出指令
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x35;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送磁场数据输出指令
//	delay_ms(1);
	send_data[0]=0xa5;
	send_data[1]=0x45;
	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送欧拉角数据输出指令
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x65;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送四元数数据输出指令
    delay_ms(1);
    GY953_Output_Speed(200);
}

void GY953_Data_Prepare()
{
	uint8_t data_buf[9]={0};
	switch(GY953_Data_Check(data_buf))//检查有无数据接收完毕，并返回输出指令
	{
		case 0x15:{//加数度数据输出
			mpu6050.Acc_I16.x=(data_buf[0]<<8)|data_buf[1];
			mpu6050.Acc_I16.y=(data_buf[2]<<8)|data_buf[3];
			mpu6050.Acc_I16.z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x25:{//陀螺仪数据输出
			mpu6050.Gyro_I16.x=(data_buf[0]<<8)|data_buf[1];
			mpu6050.Gyro_I16.y=(data_buf[2]<<8)|data_buf[3];
			mpu6050.Gyro_I16.z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x35:{//磁场数据输出
			Mag.X=(data_buf[0]<<8)|data_buf[1];
			Mag.Y=(data_buf[2]<<8)|data_buf[3];
			Mag.Z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x45:{//欧拉角数据输出
			EulerAngle.Roll=(data_buf[0]<<8)|data_buf[1];
			EulerAngle.Pitch=(data_buf[2]<<8)|data_buf[3];
			EulerAngle.Yaw=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x65:{//四元数数据输出
        	QuadNumber.q0=(data_buf[0]<<8)|data_buf[1];
			QuadNumber.q1=(data_buf[2]<<8)|data_buf[3];
			QuadNumber.q2=(data_buf[4]<<8)|data_buf[5];
			QuadNumber.q3=(data_buf[6]<<8)|data_buf[7];
		}
		break;
		default:break;
	}
}

uint8_t GY953_Data_Check(uint8_t *data)
{
	uint8_t i=0,flag=0,length=0,sum=0x5a+0x5a;
	if(stata_reg)
	{
		if(stata_reg&ACC)//判断加数度数据接收完毕
		{
			stata_reg^=ACC;//清标志位
			flag=0x15;//记录功能字节
			length=6;//记录数据长度
			/*转移数据到data，避免串口中断对data1_buf的影响*/
		  memcpy(data,Acc_buf,sizeof(data1_buf));
		  sum=sum+flag+length;//累加帧头、功能字节、数据长度
		}
		else if(stata_reg&GYR)
		{
			stata_reg^=GYR;
			flag=0x25;
			length=6;
		  memcpy(data,Gyro_buf,sizeof(data2_buf));
		  sum=sum+flag+length;
		}
		else if(stata_reg&MAG)
		{
			stata_reg^=MAG;
			flag=0x35;
			length=6;
		  memcpy(data,Mag_buf,sizeof(data3_buf));
		  sum=sum+flag+length;
		}
		else if(stata_reg&RPY)
		{
			stata_reg^=RPY;
			flag=0x45;
			length=6;
		  memcpy(data,Eur_buf,sizeof(data4_buf));
		  sum=sum+flag+length;
		}
		else if(stata_reg&Q4)
		{
			stata_reg^=Q4;
			flag=0x65;
			length=8;
		  memcpy(data,Quad_buf,sizeof(data5_buf));
		  sum=sum+flag+length;
		}
		for(i=0;i<length;i++)//累加数据
		{
		 sum+=data[i];
		}
		if(sum!=data[i])//判断校验和是否正确
		return 0;
		else
			return flag;//返回功能字节	
	}
	else
		return 0;
}

void GY953_Calibrate()
{
	uint8_t send_data[3]={0};
	send_data[0]=0xa5;
	send_data[1]=0x57;
	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送加速度计校准指令
	delay_ms(1);
	send_data[1]=0x58;
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//发送磁场校准指令
}

void GY953_Output_Speed(uint8_t f)
{
	uint8_t send_data[3]={0};
	send_data[0]=0xa5;
	switch (f)
	{
	case 50:
		send_data[1] = 0xa4;
		break;
	case 100:
		send_data[1] = 0xa5;
		break;
	case 200:
		send_data[1] = 0xa6;
		break;
	default:
		break;
	}
	send_data[2] = (uint8_t)(send_data[0] + send_data[1]);
}


//////////////
/*更据ACC、GYR等不同种数据对应接收，便于后期加工使用*/
///////////
void GY953_UART_IRQHandler(void)
{
	static uint8_t rebuf[13]={0},i=0;
	if(USART_GetITStatus(GY953_UART_PORT, USART_IT_RXNE) != RESET)
	{
		rebuf[i++]=USART_ReceiveData(GY953_UART_PORT);
		if(rebuf[0]!=0x5a)//判断帧头
			i=0;
	  if((i==2)&&(rebuf[1]!=0x5a))//判断帧头
			i=0;
		if(i>4)//当i计数值=5时，功能字节接受完毕，数据长度字节接收完毕，开始接受数据
		{
			switch(rebuf[2])//判断功能字节
			{
				case 0x15:Acc_buf[i-5]=rebuf[i-1];break;//保存加速度数据
				case 0x25:Gyro_buf[i-5]=rebuf[i-1];break;//保存陀螺仪数据
				case 0x35:Mag_buf[i-5]=rebuf[i-1];break;//保存磁场数据
				case 0x45:Eur_buf[i-5]=rebuf[i-1];break;//保存欧拉角数据
				case 0x65:Quad_buf[i-5]=rebuf[i-1];break;//保存四元数数据
			}
			//判断数据接收完毕
			//rebuf[3]为数据个数，5=两帧头+1个功能字节+1个数据长度字节+1个数据校验和字节
			if(i==rebuf[3]+5)
			{
				switch(rebuf[2])
				{
					case 0x15:stata_reg|=ACC;break;//加速度数据接收完毕
					case 0x25:stata_reg|=GYR;break;//陀螺仪数据接收完毕
					case 0x35:stata_reg|=MAG;break;//磁场数据接收完毕
					case 0x45:stata_reg|=RPY;break;//欧拉角数据接收完毕
					case 0x65:stata_reg|=Q4;break;//四元数数据接收完毕
				}
				i=0;
			}
		}
		USART_ClearFlag(GY953_UART_PORT,USART_FLAG_RXNE);//清中断标志
	}	
}

/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

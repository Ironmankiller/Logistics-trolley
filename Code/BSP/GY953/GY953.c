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

uint8_t stata_reg=0;//����״̬�Ĵ���
//5�����ݻ��棬��ӦAcc-Q�����泤��Ϊ7������6������+1��У�������
//���泤��Ϊ9������8����Ԫ������+1��У�������
uint8_t Acc_buf[7]={0},Gyro_buf[7]={0},Mag_buf[7]={0},
Eur_buf[7]={0},Quad_buf[9]={0};

void GY953_Init()
{
    uint8_t send_data[3]={0};
//	send_data[0]=0xa5;
//	send_data[1]=0x15;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);//ָ��У���
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//���ͼ��ٶ����ָ��
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x25;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//�����������������ָ��
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x35;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//���ʹų��������ָ��
//	delay_ms(1);
	send_data[0]=0xa5;
	send_data[1]=0x45;
	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//����ŷ�����������ָ��
//	delay_ms(1);
//	send_data[0]=0xa5;
//	send_data[1]=0x65;
//	send_data[2]=(uint8_t)(send_data[0]+send_data[1]);
//	Usart_SendBytes(GY953_UART_PORT,send_data,3);//������Ԫ���������ָ��
    delay_ms(1);
    GY953_Output_Speed(200);
}

void GY953_Data_Prepare()
{
	uint8_t data_buf[9]={0};
	switch(GY953_Data_Check(data_buf))//����������ݽ�����ϣ����������ָ��
	{
		case 0x15:{//�������������
			mpu6050.Acc_I16.x=(data_buf[0]<<8)|data_buf[1];
			mpu6050.Acc_I16.y=(data_buf[2]<<8)|data_buf[3];
			mpu6050.Acc_I16.z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x25:{//�������������
			mpu6050.Gyro_I16.x=(data_buf[0]<<8)|data_buf[1];
			mpu6050.Gyro_I16.y=(data_buf[2]<<8)|data_buf[3];
			mpu6050.Gyro_I16.z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x35:{//�ų��������
			Mag.X=(data_buf[0]<<8)|data_buf[1];
			Mag.Y=(data_buf[2]<<8)|data_buf[3];
			Mag.Z=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x45:{//ŷ�����������
			EulerAngle.Roll=(data_buf[0]<<8)|data_buf[1];
			EulerAngle.Pitch=(data_buf[2]<<8)|data_buf[3];
			EulerAngle.Yaw=(data_buf[4]<<8)|data_buf[5];
		}
		break;
		case 0x65:{//��Ԫ���������
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
		if(stata_reg&ACC)//�жϼ��������ݽ������
		{
			stata_reg^=ACC;//���־λ
			flag=0x15;//��¼�����ֽ�
			length=6;//��¼���ݳ���
			/*ת�����ݵ�data�����⴮���ж϶�data1_buf��Ӱ��*/
		  memcpy(data,Acc_buf,sizeof(data1_buf));
		  sum=sum+flag+length;//�ۼ�֡ͷ�������ֽڡ����ݳ���
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
		for(i=0;i<length;i++)//�ۼ�����
		{
		 sum+=data[i];
		}
		if(sum!=data[i])//�ж�У����Ƿ���ȷ
		return 0;
		else
			return flag;//���ع����ֽ�	
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
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//���ͼ��ٶȼ�У׼ָ��
	delay_ms(1);
	send_data[1]=0x58;
	Usart_SendBytes(GY953_UART_PORT,send_data,3);//���ʹų�У׼ָ��
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
/*����ACC��GYR�Ȳ�ͬ�����ݶ�Ӧ���գ����ں��ڼӹ�ʹ��*/
///////////
void GY953_UART_IRQHandler(void)
{
	static uint8_t rebuf[13]={0},i=0;
	if(USART_GetITStatus(GY953_UART_PORT, USART_IT_RXNE) != RESET)
	{
		rebuf[i++]=USART_ReceiveData(GY953_UART_PORT);
		if(rebuf[0]!=0x5a)//�ж�֡ͷ
			i=0;
	  if((i==2)&&(rebuf[1]!=0x5a))//�ж�֡ͷ
			i=0;
		if(i>4)//��i����ֵ=5ʱ�������ֽڽ�����ϣ����ݳ����ֽڽ�����ϣ���ʼ��������
		{
			switch(rebuf[2])//�жϹ����ֽ�
			{
				case 0x15:Acc_buf[i-5]=rebuf[i-1];break;//������ٶ�����
				case 0x25:Gyro_buf[i-5]=rebuf[i-1];break;//��������������
				case 0x35:Mag_buf[i-5]=rebuf[i-1];break;//����ų�����
				case 0x45:Eur_buf[i-5]=rebuf[i-1];break;//����ŷ��������
				case 0x65:Quad_buf[i-5]=rebuf[i-1];break;//������Ԫ������
			}
			//�ж����ݽ������
			//rebuf[3]Ϊ���ݸ�����5=��֡ͷ+1�������ֽ�+1�����ݳ����ֽ�+1������У����ֽ�
			if(i==rebuf[3]+5)
			{
				switch(rebuf[2])
				{
					case 0x15:stata_reg|=ACC;break;//���ٶ����ݽ������
					case 0x25:stata_reg|=GYR;break;//���������ݽ������
					case 0x35:stata_reg|=MAG;break;//�ų����ݽ������
					case 0x45:stata_reg|=RPY;break;//ŷ�������ݽ������
					case 0x65:stata_reg|=Q4;break;//��Ԫ�����ݽ������
				}
				i=0;
			}
		}
		USART_ClearFlag(GY953_UART_PORT,USART_FLAG_RXNE);//���жϱ�־
	}	
}

/******************* (C) COPYRIGHT 2019 SPY *****END OF FILE************/

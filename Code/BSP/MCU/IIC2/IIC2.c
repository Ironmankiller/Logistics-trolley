/******************** (C) COPYRIGHT 2014 ANO Tech ********************************
* ����   �������ƴ�
* �ļ���  ��i2c_soft.c
* ����    �����ģ��i2cͨ��
* ����    ��www.anotc.com
* �Ա�    ��anotc.taobao.com
* ����QȺ ��190169595
**********************************************************************************/
#include "IIC2.h"

volatile u8 I2C2_FastMode;

void I2c2_Soft_delay()
{
    __nop(); __nop(); __nop();
    __nop(); __nop(); __nop();
    __nop(); __nop(); __nop();

    if (!I2C2_FastMode)
    {
        u8 i = 15;
        while (i--);
    }
}

void I2c2_Soft_Init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(ANO_RCC_I2C2, ENABLE);
    GPIO_InitStructure.GPIO_Pin = I2C2_Pin_SCL | I2C2_Pin_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(ANO_GPIO_I2C2, &GPIO_InitStructure);
}

int I2c2_Soft_Start()
{
    SDA2_H;
    SCL2_H;
    I2c2_Soft_delay();
    if (!SDA2_read)return 0;	//SDA��Ϊ�͵�ƽ������æ,�˳�
    SDA2_L;
    I2c2_Soft_delay();
    if (SDA2_read) return 0;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
    SDA2_L;
    I2c2_Soft_delay();
    return 1;

}

void I2c2_Soft_Stop()
{
    SCL2_L;
    I2c2_Soft_delay();
    SDA2_L;
    I2c2_Soft_delay();
    SCL2_H;
    I2c2_Soft_delay();
    SDA2_H;
    I2c2_Soft_delay();
}

void I2c2_Soft_Ask()
{
    SCL2_L;
    I2c2_Soft_delay();
    SDA2_L;
    I2c2_Soft_delay();
    SCL2_H;
    I2c2_Soft_delay();
    SCL2_L;
    I2c2_Soft_delay();
}

void I2c2_Soft_NoAsk()
{
    SCL2_L;
    I2c2_Soft_delay();
    SDA2_H;
    I2c2_Soft_delay();
    SCL2_H;
    I2c2_Soft_delay();
    SCL2_L;
    I2c2_Soft_delay();
}

int I2c2_Soft_WaitAsk(void) 	 //����Ϊ:=1��ASK,=0��ASK
{
    u8 ErrTime = 0;
    SCL2_L;
    I2c2_Soft_delay();
    SDA2_H;
    I2c2_Soft_delay();
    SCL2_H;
    I2c2_Soft_delay();
    while (SDA2_read)
    {
        ErrTime++;
        if (ErrTime>50)
        {
            I2c2_Soft_Stop();
            return 1;
        }
    }
    SCL2_L;
    I2c2_Soft_delay();
    return 0;
}

void I2c2_Soft_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i = 8;
    while (i--)
    {
        SCL2_L;
        I2c2_Soft_delay();
        if (SendByte & 0x80)
            SDA2_H;
        else
            SDA2_L;
        SendByte <<= 1;
        I2c2_Soft_delay();
        SCL2_H;
        I2c2_Soft_delay();
    }
    SCL2_L;
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������NACK
u8 I2c2_Soft_ReadByte(u8 ask)  //���ݴӸ�λ����λ//
{
    u8 i = 8;
    u8 ReceiveByte = 0;

    SDA2_H;
    while (i--)
    {
        ReceiveByte <<= 1;
        SCL2_L;
        I2c2_Soft_delay();
        SCL2_H;
        I2c2_Soft_delay();
        if (SDA2_read)
        {
            ReceiveByte |= 0x01;
        }
    }
    SCL2_L;

    if (ask)
        I2c2_Soft_Ask();
    else
        I2c2_Soft_NoAsk();
    return ReceiveByte;
}


// IICдһ���ֽ�����
u8 IIC2_Write_1Byte(u8 SlaveAddress, u8 REG_Address, u8 REG_data)
{
    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1);
    if (I2c2_Soft_WaitAsk())
    {
        I2c2_Soft_Stop();
        return 1;
    }
    I2c2_Soft_SendByte(REG_Address);
    I2c2_Soft_WaitAsk();
    I2c2_Soft_SendByte(REG_data);
    I2c2_Soft_WaitAsk();
    I2c2_Soft_Stop();
    return 0;
}

// IIC��1�ֽ�����
u8 IIC2_Read_1Byte(u8 SlaveAddress, u8 REG_Address, u8 *REG_data)
{
    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1);
    if (I2c2_Soft_WaitAsk())
    {
        I2c2_Soft_Stop();
        return 1;
    }
    I2c2_Soft_SendByte(REG_Address);
    I2c2_Soft_WaitAsk();
    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2c2_Soft_WaitAsk();
    *REG_data = I2c2_Soft_ReadByte(0);
    I2c2_Soft_Stop();
    return 0;
}

// IICдn�ֽ�����
u8 IIC2_Write_nByte(u8 SlaveAddress, u8 REG_Address, u8 len, u8 *buf)
{
    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1);
    if (I2c2_Soft_WaitAsk())
    {
        I2c2_Soft_Stop();
        return 1;
    }
    I2c2_Soft_SendByte(REG_Address);
    I2c2_Soft_WaitAsk();
    while (len--)
    {
        I2c2_Soft_SendByte(*buf++);
        I2c2_Soft_WaitAsk();
    }
    I2c2_Soft_Stop();
    return 0;
}

// IIC��n�ֽ�����
u8 IIC2_Read_nByte(u8 SlaveAddress, u8 REG_Address, u8 len, u8 *buf)
{
    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1);
    if (I2c2_Soft_WaitAsk())
    {
        I2c2_Soft_Stop();
        return 1;
    }
    I2c2_Soft_SendByte(REG_Address);
    I2c2_Soft_WaitAsk();

    I2c2_Soft_Start();
    I2c2_Soft_SendByte(SlaveAddress << 1 | 0x01);
    I2c2_Soft_WaitAsk();
    while (len)
    {
        if (len == 1)
        {
            *buf = I2c2_Soft_ReadByte(0);
        }
        else
        {
            *buf = I2c2_Soft_ReadByte(1);
        }
        buf++;
        len--;
    }
    I2c2_Soft_Stop();
    return 0;
}


/******************* (C) COPYRIGHT 2014 ANO TECH *****END OF FILE************/


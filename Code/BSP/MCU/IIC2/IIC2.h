#ifndef _I2C2_SOFT_H
#define	_I2C2_SOFT_H

#include "stm32f4xx.h"
#include "time.h"

/***************I2C GPIO¶¨Òå******************/
#define ANO_GPIO_I2C2	GPIOA
#define I2C2_Pin_SCL		GPIO_Pin_9
#define I2C2_Pin_SDA		GPIO_Pin_10
#define ANO_RCC_I2C2		RCC_AHB1Periph_GPIOA
/*********************************************/

#define SCL2_H         ANO_GPIO_I2C2->BSRRL = I2C2_Pin_SCL
#define SCL2_L         ANO_GPIO_I2C2->BSRRH = I2C2_Pin_SCL
#define SDA2_H         ANO_GPIO_I2C2->BSRRL = I2C2_Pin_SDA
#define SDA2_L         ANO_GPIO_I2C2->BSRRH = I2C2_Pin_SDA
#define SCL2_read      ANO_GPIO_I2C2->IDR  & I2C2_Pin_SCL
#define SDA2_read      ANO_GPIO_I2C2->IDR  & I2C2_Pin_SDA


extern volatile u8 I2C2_FastMode;
void I2c2_Soft_Init(void);
void I2c2_Soft_SendByte(u8 SendByte);
u8 I2c2_Soft_ReadByte(u8);

//int I2c_Soft_Single_Write(u8 SlaveAddress,u8 REG_Address,u8 REG_data);
//int I2c_Soft_Single_Read(u8 SlaveAddress,u8 REG_Address);
//int I2c_Soft_Mult_Read(u8 SlaveAddress,u8 REG_Address,u8 * ptChar,u8 size);

u8 IIC2_Write_1Byte(u8 SlaveAddress,u8 REG_Address,u8 REG_data);
u8 IIC2_Read_1Byte(u8 SlaveAddress,u8 REG_Address,u8 *REG_data);
u8 IIC2_Write_nByte(u8 SlaveAddress, u8 REG_Address, u8 len, u8 *buf);
u8 IIC2_Read_nByte(u8 SlaveAddress, u8 REG_Address, u8 len, u8 *buf);

#endif

#include "SPI1.h"
SPI_InitTypeDef  SPI1_InitStructure;
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* 使能 SPI1 及GPIO 时钟 */
    RCC_AHB1PeriphClockCmd (SPI1_SCK_GPIO_CLK | SPI1_MISO_GPIO_CLK|SPI1_MOSI_GPIO_CLK|SPI1_CS_GPIO_CLK, ENABLE);

    /*!< SPI 时钟使能 */
    SPI1_CLK_INIT(SPI1_CLK, ENABLE);
 
  //设置引脚复用
    GPIO_PinAFConfig(SPI1_SCK_GPIO_PORT,SPI1_SCK_PINSOURCE,SPI1_SCK_AF); 
	GPIO_PinAFConfig(SPI1_MISO_GPIO_PORT,SPI1_MISO_PINSOURCE,SPI1_MISO_AF); 
	GPIO_PinAFConfig(SPI1_MOSI_GPIO_PORT,SPI1_MOSI_PINSOURCE,SPI1_MOSI_AF); 
  
    /*!< 配置 SPI1 引脚: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    
    GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI1 引脚: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI1_MISO_PIN;
    GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< 配置 SPI1 引脚: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI1_MOSI_PIN;
    GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< 配置 SPI1 引脚: CS */
    GPIO_InitStructure.GPIO_Pin = SPI1_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(SPI1_CS_GPIO_PORT, &GPIO_InitStructure);

    /* 停止信号 FLASH: CS引脚高电平*/
    SPI1_CS_HIGH();

    /* SPI1 模式配置 */
    // FLASH芯片 支持SPI模式0及模式3，据此设置CPOL CPHA
    SPI1_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI1_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI1_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI1_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI1_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI1_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI1_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI1_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI1_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI1_InitStructure);
    
    /* 使能 SPI1  */
    SPI_Cmd(SPI1, ENABLE);

}


//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   
//SPI_BaudRatePrescaler_8   8分频   
//SPI_BaudRatePrescaler_16  16分频  
//SPI_BaudRatePrescaler_256 256分频 
  
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); 

} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据					    
}



#include "SPI1.h"
SPI_InitTypeDef  SPI1_InitStructure;
void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* ʹ�� SPI1 ��GPIO ʱ�� */
    RCC_AHB1PeriphClockCmd (SPI1_SCK_GPIO_CLK | SPI1_MISO_GPIO_CLK|SPI1_MOSI_GPIO_CLK|SPI1_CS_GPIO_CLK, ENABLE);

    /*!< SPI ʱ��ʹ�� */
    SPI1_CLK_INIT(SPI1_CLK, ENABLE);
 
  //�������Ÿ���
    GPIO_PinAFConfig(SPI1_SCK_GPIO_PORT,SPI1_SCK_PINSOURCE,SPI1_SCK_AF); 
	GPIO_PinAFConfig(SPI1_MISO_GPIO_PORT,SPI1_MISO_PINSOURCE,SPI1_MISO_AF); 
	GPIO_PinAFConfig(SPI1_MOSI_GPIO_PORT,SPI1_MOSI_PINSOURCE,SPI1_MOSI_AF); 
  
    /*!< ���� SPI1 ����: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    
    GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI1 ����: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI1_MISO_PIN;
    GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI1 ����: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI1_MOSI_PIN;
    GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< ���� SPI1 ����: CS */
    GPIO_InitStructure.GPIO_Pin = SPI1_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(SPI1_CS_GPIO_PORT, &GPIO_InitStructure);

    /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
    SPI1_CS_HIGH();

    /* SPI1 ģʽ���� */
    // FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
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
    
    /* ʹ�� SPI1  */
    SPI_Cmd(SPI1, ENABLE);

}


//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI1->CR1&=0XFFC7;
	SPI1->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ� 
	SPI_Cmd(SPI1,ENABLE); 

} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����					    
}



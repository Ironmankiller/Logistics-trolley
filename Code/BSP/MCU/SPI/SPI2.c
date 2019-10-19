#include "SPI2.h"
SPI_InitTypeDef  SPI2_InitStructure;
void SPI2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    /* ʹ�� SPI2 ��GPIO ʱ�� */
    RCC_AHB1PeriphClockCmd (SPI2_SCK_GPIO_CLK | SPI2_MISO_GPIO_CLK|SPI2_MOSI_GPIO_CLK|SPI2_CS_GPIO_CLK, ENABLE);

    /*!< SPI ʱ��ʹ�� */
    SPI2_CLK_INIT(SPI2_CLK, ENABLE);
 
  //�������Ÿ���
    GPIO_PinAFConfig(SPI2_SCK_GPIO_PORT,SPI2_SCK_PINSOURCE,SPI2_SCK_AF); 
	GPIO_PinAFConfig(SPI2_MISO_GPIO_PORT,SPI2_MISO_PINSOURCE,SPI2_MISO_AF); 
	GPIO_PinAFConfig(SPI2_MOSI_GPIO_PORT,SPI2_MOSI_PINSOURCE,SPI2_MOSI_AF); 
  
    /*!< ���� SPI2 ����: SCK */
    GPIO_InitStructure.GPIO_Pin = SPI2_SCK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;  
    
    GPIO_Init(SPI2_SCK_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI2 ����: MISO */
    GPIO_InitStructure.GPIO_Pin = SPI2_MISO_PIN;
    GPIO_Init(SPI2_MISO_GPIO_PORT, &GPIO_InitStructure);
  
	/*!< ���� SPI2 ����: MOSI */
    GPIO_InitStructure.GPIO_Pin = SPI2_MOSI_PIN;
    GPIO_Init(SPI2_MOSI_GPIO_PORT, &GPIO_InitStructure);  

	/*!< ���� SPI2 ����: CS */
    GPIO_InitStructure.GPIO_Pin = SPI2_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(SPI2_CS_GPIO_PORT, &GPIO_InitStructure);

    /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
    SPI2_CS_HIGH();

    /* SPI2 ģʽ���� */
    // FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
    SPI2_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI2_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI2_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI2_InitStructure.SPI_CPOL = SPI_CPOL_High;
    SPI2_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
    SPI2_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI2_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI2_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI2_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI2, &SPI2_InitStructure);
    
    /* ʹ�� SPI2  */
    SPI_Cmd(SPI2, ENABLE);

}


//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   
//SPI_BaudRatePrescaler_8   8��Ƶ   
//SPI_BaudRatePrescaler_16  16��Ƶ  
//SPI_BaudRatePrescaler_256 256��Ƶ 
  
void SPI2_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
	SPI2->CR1&=0XFFC7;
	SPI2->CR1|=SPI_BaudRatePrescaler;	//����SPI2�ٶ� 
	SPI_Cmd(SPI2,ENABLE); 

} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI2_ReadWriteByte(u8 TxData)
{		
	u8 retry=0;				 	
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}			  
	SPI_I2S_SendData(SPI2, TxData); //ͨ������SPIx����һ������
	retry=0;

	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
		{
		retry++;
		if(retry>200)return 0;
		}	  						    
	return SPI_I2S_ReceiveData(SPI2); //����ͨ��SPIx������յ�����					    
}



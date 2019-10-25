#include "74HC165.h"
#include "delay.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"

void HC165_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(HC165_SHLD_GPIO_CLK | HC165_CLK_GPIO_CLK | HC165_DATA_GPIO_CLK, ENABLE);//使能时钟

	GPIO_InitStructure.GPIO_Pin = HC165_SHLD_GPIO_PIN | HC165_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(HC165_SHLD_GPIO_PORT, &GPIO_InitStructure);  //初始化GPIO
    
    GPIO_InitStructure.GPIO_Pin = HC165_CLK_GPIO_PIN;
    GPIO_Init(HC165_CLK_GPIO_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = HC165_DATA_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	    		
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(HC165_DATA_GPIO_PORT, &GPIO_InitStructure);  
    
    SHLD_H;
    HC165_CLK_H;
}



void HC165_delay(void)
{
    delay_us(1);

//    if (!HC165_FastMode)
//    {
//        u8 i = 15;
//        while (i--);
//    }
}

u8 HC165_Read_Byte(void)
{
    u8 i = 8;
    u8 ReceiveByte = 0;
    
    //多芯片级联时须注掉SHLD_H和SHLD_L
    SHLD_L;
    HC165_delay();
    SHLD_H;
    HC165_delay();
    while (i--)
    {
        ReceiveByte <<= 1;
        HC165_CLK_L;
        HC165_delay();
        HC165_CLK_H;
        HC165_delay();
        if (DATA_read)
        {
            ReceiveByte |= 0x01;
        }
    }
    return ReceiveByte;
}

u16 HC165_Read_2Byte(void)
{
    u16 ReceiveByte = 0;
    
//    SHLD_L;
//    HC165_delay();
//    SHLD_H;
//    delay_us(5);
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0400;
    }
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0800;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0100;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0200;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x2000;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x1000;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x4000;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x8000;
    }
    
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0001;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0002;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0008;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0004;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0080;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0040;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0010;
    }
    HC165_delay();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    if (DATA_read)
    {
        ReceiveByte |= 0x0020;
    }
    HC165_delay();
    
    return ReceiveByte;
}

void HC165_Show(void){
    
    u16 temp = 0;
    u16 temp1 = 0;
    u16 temp2 = 0;
    u16 temp3 = 0;
    u16 temp4 = 0;
    u16 tempp = 0;
    SHLD_L;
    HC165_delay();
    SHLD_H;
    HC165_delay();


    temp = HC165_Read_2Byte();
    for(int i=15;i>=0;i--){
        tempp = temp;
        tempp >>= i;
        tempp &= 1;
        if(tempp == 1){
            u1_printf("1");
        }else{
            u1_printf("0");
        }
    }
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    temp1 = HC165_Read_2Byte();
    for(int i=15;i>=0;i--){
        tempp = temp1;
        tempp >>= i;
        tempp &= 1;
        if(tempp == 1){
            u1_printf("1");
        }else{
            u1_printf("0");
        }
    }
    
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    temp2 = HC165_Read_2Byte();
    for(int i=15;i>=0;i--){
        tempp = temp2;
        tempp >>= i;
        tempp &= 1;
        if(tempp == 1){
            u1_printf("1");
        }else{
            u1_printf("0");
        }
    }
    
    
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    temp3 = HC165_Read_2Byte();
    for(int i=15;i>=0;i--){
        tempp = temp3;
        tempp >>= i;
        tempp &= 1;
        if(tempp == 1){
            u1_printf("1");
        }else{
            u1_printf("0");
        }
    }
    
//    HC165_CLK_L;
//    HC165_delay();
//    HC165_CLK_H;
//    HC165_delay();
//    temp4 = HC165_Read_2Byte();
//    for(int i=15;i>=0;i--){
//        tempp = temp4;
//        tempp >>= i;
//        tempp &= 1;
//        if(tempp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    HC165_CLK_L;
//    HC165_delay();
//    HC165_CLK_H;
//    HC165_delay();
    
    u1_printf("\r\n");
}

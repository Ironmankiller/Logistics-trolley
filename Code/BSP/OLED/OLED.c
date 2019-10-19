#include "stm32f4xx.h"
#include "OLED.h"
#include <stdio.h>
#include "OLED_Fonts.h"  	 
#include "delay.h"
#include "sys.h"

//-----------------OLED端口定义---------------- 
#define OLED_RST_Clr()      PDout(5) = 0    //RST
#define OLED_RST_Set()      PDout(5) = 1    //RST

#define OLED_RS_Clr()       PDout(4) = 0    //DC
#define OLED_RS_Set()       PDout(4) = 1    //DC

#define OLED_SCLK_Clr()     PDout(7) = 0    //SCL D0
#define OLED_SCLK_Set()     PDout(7) = 1    //SCL

#define OLED_SDIN_Clr()     PDout(6) = 0    //SDA D1
#define OLED_SDIN_Set()     PDout(6) = 1    //SDA

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 	
u8 OLED_GRAM[128][8];

void OLED_Refresh_Gram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
		OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置―列低地址
		OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置―列高地址
		for(n=0;n<128;n++)OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA); 
	}   
}

//初始化OLED					    
void OLED_Init(void)
{
    /* IO初始化 */
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    //GPIO初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;       //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);              //初始化GPIO

    OLED_SCLK_Set();
    OLED_SDIN_Set();
    OLED_RS_Set();

    /* OLED寄存器初始化 */
    OLED_RST_Clr();
    delay_ms(100);
    OLED_RST_Set();

    OLED_WR_Byte(0xAE, OLED_CMD); //关闭显示
    OLED_WR_Byte(0xD5, OLED_CMD); //设置时钟分频因子,震荡频率
    OLED_WR_Byte(80, OLED_CMD);   //[3:0],分频因子;[7:4],震荡频率
    OLED_WR_Byte(0xA8, OLED_CMD); //设置驱动路数
    OLED_WR_Byte(0X3F, OLED_CMD); //默认0X3F(1/64) 
    OLED_WR_Byte(0xD3, OLED_CMD); //设置显示偏移
    OLED_WR_Byte(0X00, OLED_CMD); //默认为0

    OLED_WR_Byte(0x40, OLED_CMD); //设置显示开始行 [5:0],行数.

    OLED_WR_Byte(0x8D, OLED_CMD); //电荷泵设置
    OLED_WR_Byte(0x14, OLED_CMD); //bit2，开启/关闭
    OLED_WR_Byte(0x20, OLED_CMD); //设置内存地址模式
    OLED_WR_Byte(0x02, OLED_CMD); //[1:0],00，列地址模式;01，行地址模式;10,页地址模式;默认10;
    OLED_WR_Byte(0xA1, OLED_CMD); //段重定义设置,bit0:0,0->0;1,0->127;
    OLED_WR_Byte(0xC0, OLED_CMD); //设置COM扫描方向;bit3:0,普通模式;1,重定义模式 COM[N-1]->COM0;N:驱动路数
    OLED_WR_Byte(0xDA, OLED_CMD); //设置COM硬件引脚配置
    OLED_WR_Byte(0x12, OLED_CMD); //[5:4]配置

    OLED_WR_Byte(0x81, OLED_CMD); //对比度设置
    OLED_WR_Byte(0xEF, OLED_CMD); //1~255;默认0X7F (亮度设置,越大越亮)
    OLED_WR_Byte(0xD9, OLED_CMD); //设置预充电周期
    OLED_WR_Byte(0xf1, OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
    OLED_WR_Byte(0xDB, OLED_CMD); //设置VCOMH 电压倍率
    OLED_WR_Byte(0x30, OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

    OLED_WR_Byte(0xA4, OLED_CMD); //全局显示开启;bit0:1,开启;0,关闭;(白屏/黑屏)
    OLED_WR_Byte(0xA6, OLED_CMD); //设置显示方式;bit0:1,反相显示;0,正常显示	    						   
    OLED_WR_Byte(0xAF, OLED_CMD); //开启显示	 
    OLED_Clear();
}


/**********************************************
* 向OLED写入一个字节。
* dat:要写入的数据/命令
* cmd:数据/命令标志 0,表示命令;1,表示数据;
**********************************************/
void OLED_WR_Byte(u8 dat,u8 cmd)
{	
	u8 i;			  
	if(cmd)
	  OLED_RS_Set();
	else 
	  OLED_RS_Clr();		  
	for(i=0;i<8;i++)
	{			  
		OLED_SCLK_Clr();
		if(dat&0x80)
		   OLED_SDIN_Set();
		else 
		   OLED_SDIN_Clr();
		OLED_SCLK_Set();
		dat<<=1;   
	}				 		  
	OLED_RS_Set();   	  
} 

	  	  
//开启OLED显示    
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}

//关闭OLED显示     
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		  

//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)for(n=0;n<128;n++)OLED_GRAM[n][i]=0X00;  
	OLED_Refresh_Gram();//更新显示
}

//画点 
//x:0~127
//y:0~63
//t:1 填充 0,清空				   
void OLED_DrawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//超出范围了.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)OLED_GRAM[x][pos]|=temp;
	else OLED_GRAM[x][pos]&=~temp;	    
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示				 
//size:选择字体 16/12 
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;
	chr=chr-' ';//得到偏移后的值				   
    for(t=0;t<size;t++)
    {   
		if(size==12)temp=oled_asc2_1206[chr][t];  //调用1206字体
		else temp=oled_asc2_1608[chr][t];		 //调用1608字体 	                          
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)OLED_DrawPoint(x,y,mode);
			else OLED_DrawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
    }          
}

//m^n函数
u32 oled_pow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}		

//显示2个数字
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//mode:模式	0,填充模式;1,叠加模式
//num:数值(0~4294967295);	 		  
void OLED_ShowNumber(u8 x,u8 y,u32 num,u8 len,u8 size)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size/2)*t,y,' ',size,1);
				continue;
			}else enshow=1; 
		 	 
		}
	 	OLED_ShowChar(x+(size/2)*t,y,temp+'0',size,1); 
	}
} 

//显示字符串
//x,y:起点坐标  
//*p:字符串起始地址
//用16字体
void OLED_ShowString(u8 x,u8 y,const u8 *p)
{
#define MAX_CHAR_POSX 122
#define MAX_CHAR_POSY 58          
    while(*p!='\0')
    {       
        if(x>MAX_CHAR_POSX){x=0;y+=12;}
        if(y>MAX_CHAR_POSY){y=x=0;OLED_Clear();}
        OLED_ShowChar(x,y,*p,12,1);	 
        x+=6;
        p++;
    }  
}	   

//显示浮点数
//x,y:起点坐标  
//p:浮点数
void OLED_ShowFloat(u8 x, u8 y, float p)
{
	char s[10];
	if (p > 0)
	{
		OLED_ShowString(x - 5, y, " ");
		sprintf(s, "%3d", (int32_t)p);
		OLED_ShowString(x, y, (const u8 *)s);
	}
	else
	{
		OLED_ShowString(x -  5, y, "-");
		sprintf(s, "%3d", -(int32_t)p);
		OLED_ShowString(x, y, (const u8 *)s);
	}
	OLED_ShowString(x + 17, y, ".");
	OLED_ShowNumber(x + 21, y, (int)(p * 10) % 10, 1, 12);
}

void OLED_ShowInt(uint8_t x, uint8_t y, int32_t a)
{
    char s[10];
    sprintf(s, "%d", a);
    OLED_ShowString(x, y, (const u8 *)s);
}




/**
******************************************************************************
* @file    bsp_can.c
* @author  YX_L 李彪
* @date    2017-02-10
* @brief   这个文件提供CAN通信的初始化、数据收发送
******************************************************************************
* @attention
*          本文件适用于F103 库函数V3.5.0 和F407 库函数V1.4.0
*          使用时只需直接拷贝进相关文件夹即可，以下是详细使用说明
*          如果用F4:
*          1.首先调用 void CAN_Config(void)进行初始化
*          2.CAN1发送数据调用 u8 CAN1_SendMsg(u8* msg,u32 TX_STD_ID)
*          3.CAN1接收数据调用 u8 CAN1_ReceiveMsg(u8* buf)
*          4.使用CAN2时首先在 bsp_can.h 中将 CAN2_USE_ENABLE 设置为1
*          5.CAN2发送数据调用 u8 CAN2_SendMsg(u8* msg,u32 TX_STD_ID)
*          6.CAN2接收数据调用 u8 CAN2_ReceiveMsg(u8* buf)
*          7.使用接收中断时，请先在 bsp_can.h 中使能
*
*          如果用F1:
*          1.首先调用 u8 CAN_Config()进行初始化
*          2.CAN1发送数据调用 u8 CAN1_SendMsg(u8* msg, u32 TX_STD_ID)
*          3.CAN1接收数据调用 u8 CAN1_ReceiveMsg(u8 *buf)
*          7.使用接收中断时，请先在 bsp_can.h 中使能 CAN_RX0_INT_ENABLE
*
******************************************************************************
*/
#include "can.h"
#include "includes.h"

CanTxMsg TxMessage;

/**********************************使用F4 CAN通信*******************************/
#if defined STM32F40_41xxx 

static void CAN_GPIO_Config(void);
static void CAN_NVIC_Config(void);
static void CAN_Mode_Config(void);
static void CAN_Filter_Config(void);

/*
* 函数名：CAN_Config
* 描述  ：完整配置CAN的功能
* 输入  ：无
* 输出  ：无
* 调用  ：外部调用
*/
void CAN_Config(void)
{
    CAN_GPIO_Config();
    CAN_NVIC_Config();
    CAN_Mode_Config();
    CAN_Filter_Config();
}

/*
* 函数名：CAN1_SendMsg
* 描述  ：使用CAN1发送数据
* 输入  ：msg ：数据指针,最大为8个字节
*         TX_STD_ID ：发送ID
* 输出  ：1 发送成功
*         0 发送失败
* 调用  ：外部调用
*/
u8 CAN1_SendMsg(u8* msg, u32 TX_STD_ID)
{
    u8 mbox;
    u16 i = 0;
    TxMessage.StdId = TX_STD_ID;				 //使用的标准ID		 
    TxMessage.IDE = CAN_ID_STD;					 //标准模式
    TxMessage.RTR = CAN_RTR_DATA;				 //发送的是数据
    TxMessage.DLC = 8;							     //数据长度为2字节
    for (i = 0; i<8; i++)
    {
        TxMessage.Data[i] = msg[i];
    }
    mbox = CAN_Transmit(CAN1, &TxMessage);
    i = 0;
    while ((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i<0XFFF))i++;	//等待发送结束
    if (i >= 0XFFF)return 1;//发送失败
    return 0;		         //发送成功
}

/*
* 函数名：CAN1_ReceiveMsg
* 描述  ：使用CAN1接收数据
* 输入  ：buf:数据指针,最大为8个字节
* 输出  ：接收数据长度
* 调用  ：外部调用
*/
u8 CAN1_ReceiveMsg(u8* buf)
{
    CanRxMsg RxMessage;
    u16 i;
    if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)return 0;		//查询FIFO0是否有信息 没有就直接退出 
    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
    for (i = 0; i<RxMessage.DLC; i++)
    {
        buf[i] = RxMessage.Data[i];
    }
    return RxMessage.DLC;
}

/*
* 函数名：CAN1_RX0_IRQHandler
* 描述  ：使用CAN1中断服务函数，测试使用
* 输入  ：无
* 输出  ：无
* 调用  ：外部调用
*/
#if CAN1_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
//void CAN1_RX0_IRQHandler(void)
//{
//    CanRxMsg RxMessage;
//    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);
//    CAN_Command_Receive(RxMessage.Data);
//}
#endif

/*
* 函数名：CAN2_SendMsg
* 描述  ：使用CAN2发送数据
* 输入  ：msg ：数据指针,最大为8个字节
*         TX_STD_ID ：发送ID
* 输出  ：1 发送成功
*         0 发送失败
* 调用  ：外部调用
*/
u8 CAN2_SendMsg(u8* msg, u32 TX_STD_ID)
{
    u8 mbox;
    u16 i = 0;
    TxMessage.StdId = TX_STD_ID;				 //使用的标准ID		 
    TxMessage.IDE = CAN_ID_STD;					 //标准模式
    TxMessage.RTR = CAN_RTR_DATA;				 //发送的是数据
    TxMessage.DLC = 8;							     //数据长度为2字节
    for (i = 0; i<8; i++)
    {
        TxMessage.Data[i] = msg[i];
    }
    mbox = CAN_Transmit(CAN2, &TxMessage);
    i = 0;
    while ((CAN_TransmitStatus(CAN2, mbox) == CAN_TxStatus_Failed) && (i<0XFFF))i++;	//等待发送结束
    if (i >= 0XFFF)return 0;//发送失败
    return 1;		         //发送成功
}

/*
* 函数名：CAN2_ReceiveMsg
* 描述  ：使用CAN2接收数据
* 输入  ：buf:数据指针,最大为8个字节
* 输出  ：接收数据长度
* 调用  ：外部调用
*/
u8 CAN2_ReceiveMsg(u8* buf)
{
    u16 i;
    CanRxMsg RxMessage;
    if (CAN_MessagePending(CAN2, CAN_FIFO0) == 0)return 0;		//查询FIFO0是否有信息 没有就直接退出 
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);//读取数据	
    for (i = 0; i<RxMessage.DLC; i++)
    {
        buf[i] = RxMessage.Data[i];
    }
    return RxMessage.DLC;
}

/*
* 函数名：CAN1_RX0_IRQHandler
* 描述  ：使用CAN1中断服务函数，测试使用
* 输入  ：无
* 输出  ：无
* 调用  ：外部调用
*/
#if CAN2_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void CAN2_RX0_IRQHandler(void)
{
    OSIntEnter();
    CanRxMsg RxMessage;
    CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
    //CAN_Command_Receive(RxMessage.Data);
    OSIntExit();
}
#endif

/*
* 函数名：CAN_GPIO_Config
* 描述  ：CAN的GPIO 配置
* 输入  ：无
* 输出  ：无
* 调用  ：内部调用
*/
static void CAN_GPIO_Config(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE); //使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);               //初始化PA11,PA12  
                                                         //引脚复用映射配置
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_CAN1); //GPIOA11复用为CAN1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_CAN1); //GPIOA12复用为CAN1

#if CAN2_USE_ENABLE

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟	 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE); //使能CAN2时钟	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;         //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);               //初始化PA11,PA12 

    //引脚复用映射配置
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_CAN2); //GPIOB12复用为CAN2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_CAN2); //GPIOB13复用为CAN3
#endif	
}

/*
* 函数名：CAN_NVIC_Config
* 描述  ：CAN1的NVIC 配置,CAN1第1优先级组，0优先级 CAN2第2优先级组，1优先级
* 输入  ：无
* 输出  ：无
* 调用  ：内部调用
*/
static void CAN_NVIC_Config(void)
{

#if CAN1_RX0_INT_ENABLE

    NVIC_InitTypeDef  NVIC_InitStructure;

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);     //FIFO0消息挂号中断允许.
    /*
    *CAN_IT_FMP0   FIFO0消息挂号中断允许
    *CAN_IT_FF0    FIFO0消息满中断允许
    *CAN_IT_FOV0   FIFO0消息上溢中断允许
    */
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;           //CAN1 FIFO0 接收中断
    /*
    *CAN1_TX_IRQn  CAN1 发送中断
    *CAN1_RX0_IRQn CAN1 FIFO0中断
    *CAN1_RX1_IRQn CAN1 FIFO1中断
    *CAN1_SCE_IRQn CAN1 状态改变中断
    *其他中断详见stm32f4xx_can.h 537行
    */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

#if CAN2_RX0_INT_ENABLE

    //NVIC_InitTypeDef  NVIC_InitStructure;      //如果CAN1_RX0_INT_ENABLE为1时，这句话就要删除

    CAN_ITConfig(CAN2, CAN_IT_FMP0, ENABLE);     //FIFO0消息挂号中断允许.
    /*
    *CAN_IT_FMP0   FIFO0消息挂号中断允许
    *CAN_IT_FF0    FIFO0消息满中断允许
    *CAN_IT_FOV0   FIFO0消息上溢中断允许
    */
    NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX0_IRQn;           //CAN1 FIFO0 接收中断
    /*
    *CAN2_TX_IRQn  发送中断
    *CAN2_RX0_IRQn FIFO0中断
    *CAN2_RX1_IRQn FIFO1中断
    *CAN2_SCE_IRQn 状态改变中断
    *其他中断详见stm32f4xx_can.h 537行
    */
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

/*
* 函数名：CAN_Mode_Config
* 描述  ：CAN的模式 配置
* 输入  ：无
* 输出  ：无
* 调用  ：内部调用
*/
static void CAN_Mode_Config(void)
{
    CAN_InitTypeDef    CAN_InitStructure;

    CAN_InitStructure.CAN_TTCM = DISABLE;	             //非时间触发通信模式   
    CAN_InitStructure.CAN_ABOM = DISABLE;	             //软件自动离线管理	  
    CAN_InitStructure.CAN_AWUM = DISABLE;              //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;	             //禁止报文自动传送 
    CAN_InitStructure.CAN_RFLM = DISABLE;	             //报文不锁定,新的覆盖旧的  
    CAN_InitStructure.CAN_TXFP = DISABLE;	             //优先级由报文标识符决定 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	   //模式设置 
	//设置波特率
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	         //BTR-SJW 重新同步跳跃宽度 2个时间单元
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;           //BTR-TS1 时间段1 占用了6个时间单元
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;           //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler = 3;               //BTR-BRP 波特率分频器  定义了时间单元的时间 36/(1+6+3)/4=0.9Mbps
    CAN_Init(CAN1, &CAN_InitStructure);              // 初始化CAN1 

#if CAN2_USE_ENABLE

    CAN_InitStructure.CAN_TTCM = DISABLE;	             //非时间触发通信模式   
    CAN_InitStructure.CAN_ABOM = DISABLE;	             //软件自动离线管理	  
    CAN_InitStructure.CAN_AWUM = DISABLE;              //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART = ENABLE;	             //禁止报文自动传送 
    CAN_InitStructure.CAN_RFLM = DISABLE;	             //报文不锁定,新的覆盖旧的  
    CAN_InitStructure.CAN_TXFP = DISABLE;	             //优先级由报文标识符决定 
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	   //模式设置 
	//设置波特率
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	         //BTR-SJW 重新同步跳跃宽度 1个时间单元
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;           //BTR-TS1 时间段1 占用了6个时间单元
    CAN_InitStructure.CAN_BS2 = CAN_BS2_4tq;           //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler = 3;               //BTR-BRP 波特率分频器  定义了时间单元的时间 36/(1+7+6)/6=0.9Mbps
    CAN_Init(CAN2, &CAN_InitStructure);              // 初始化CAN2
#endif
}

/*
* 函数名：CAN_Filter_Config
* 描述  ：CAN的过滤器 配置
* 输入  ：无
* 输出  ：无
* 调用  ：内部调用
*/
static void CAN_Filter_Config(void)
{
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    /***********************1个32位的标识符屏蔽位模式的过滤器********************************/
    CAN_FilterInitStructure.CAN_FilterNumber = 0;	                      //过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;    //32位

    CAN_FilterInitStructure.CAN_FilterIdHigh = ((u16)CAN1_RX_STD_ID << 5) & 0xFFFF;
    //CAN_FilterIdHigh包含的是STD[10:0]和EXID[17:13],标准CAN ID本身是不包含扩展ID数据，
    //因此为了要将标准CAN ID放入此寄存器，标准CAN ID首先应左移5位后才能对齐.  
    CAN_FilterInitStructure.CAN_FilterIdLow = CAN_ID_STD;

    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;              //过滤器高16位每位必须匹配
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;               //过滤器低16位每位必须匹配
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;              //激活过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);                         //滤波器初始化

#if CAN2_USE_ENABLE 

    CAN_FilterInitStructure.CAN_FilterNumber = 14;	                      //过滤器14
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;    //32位

    CAN_FilterInitStructure.CAN_FilterIdHigh = ((u16)CAN2_RX_STD_ID << 5) & 0xFFFF;
    //CAN_FilterIdHigh包含的是STD[10:0]和EXID[17:13],标准CAN ID本身是不包含扩展ID数据，
    //因此为了要将标准CAN ID放入此寄存器，标准CAN ID首先应左移5位后才能对齐.  
    CAN_FilterInitStructure.CAN_FilterIdLow = CAN_ID_STD;

    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;              //过滤器高16位每位必须匹配
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;               //过滤器低16位每位必须匹配
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器14关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;              //激活过滤器14
    CAN_FilterInit(&CAN_FilterInitStructure);                         //滤波器初始化

#endif
}

#endif

/**********************************使用F1 CAN通信*******************************/
#if defined (STM32F10X_HD) || defined (STM32F10X_MD) 

void CAN_Config(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;

#if CAN1_RX0_INT_ENABLE 
    NVIC_InitTypeDef  NVIC_InitStructure;
#endif

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能PORTA时钟	                   											 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
    GPIO_Init(GPIOA, &GPIO_InitStructure);		//初始化IO

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化IO

                                          //CAN单元设置
    CAN_InitStructure.CAN_TTCM = DISABLE;				 //非时间触发通信模式  //
    CAN_InitStructure.CAN_ABOM = DISABLE;				 //软件自动离线管理	 //
    CAN_InitStructure.CAN_AWUM = DISABLE;				 //睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)//
    CAN_InitStructure.CAN_NART = ENABLE;			     //禁止报文自动传送 //
    CAN_InitStructure.CAN_RFLM = DISABLE;				 //报文不锁定,新的覆盖旧的 // 
    CAN_InitStructure.CAN_TXFP = DISABLE;				 //优先级由报文标识符决定 //
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;	 //模式设置 
	//设置波特率
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	         //BTR-SJW 重新同步跳跃宽度 2个时间单元
    CAN_InitStructure.CAN_BS1 = CAN_BS1_9tq;           //BTR-TS1 时间段1 占用了6个时间单元
    CAN_InitStructure.CAN_BS2 = CAN_BS2_8tq;           //BTR-TS1 时间段2 占用了3个时间单元
    CAN_InitStructure.CAN_Prescaler = 4;              //BTR-BRP 波特率分频器  定义了时间单元的时间 36/(1+6+3)/4=0.9Mbps
    CAN_Init(CAN1, &CAN_InitStructure);              // 初始化CAN1 

    CAN_FilterInitStructure.CAN_FilterNumber = 0;	  //过滤器0
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit; //32位 
    CAN_FilterInitStructure.CAN_FilterIdHigh = ((u16)CAN1_RX_STD_ID << 5) & 0xFFFF;///32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0xFFFF;//32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0xFFFF;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE; //激活过滤器0

    CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化
#if CAN1_RX0_INT_ENABLE

    CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);//FIFO0消息挂号中断允许.		    

    NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif
}

#if CAN1_RX0_INT_ENABLE	//使能RX0中断
//中断服务函数			    
void USB_LP_CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_Receive(CAN1, 0, &RxMessage);
}
#endif

/*
* 函数名：CAN1_SendMsg
* 描述  ：使用CAN1发送数据
* 输入  ：msg ：数据指针,最大为8个字节
*         TX_STD_ID ：发送ID
* 输出  ：1 发送成功
*         0 发送失败
* 调用  ：外部调用
*/
u8 CAN1_SendMsg(u8* msg, u32 TX_STD_ID)
{
    u8 mbox;
    u16 i = 0;
    CanTxMsg TxMessage;
    TxMessage.StdId = TX_STD_ID;			// 标准标识符 
    TxMessage.IDE = CAN_ID_STD;           // 标准帧
    TxMessage.RTR = CAN_RTR_Data;		 // 数据帧
    TxMessage.DLC = 8;						// 要发送的数据长度

    for (i = 0; i<8; i++)
    {
        TxMessage.Data[i] = msg[i];
    }

    mbox = CAN_Transmit(CAN1, &TxMessage);
    i = 0;
    while ((CAN_TransmitStatus(CAN1, mbox) == CAN_TxStatus_Failed) && (i<0XFFF))i++;	//等待发送结束
    if (i >= 0XFFF)return 0; //返回值:0,失败;
    return 1;		      //返回值:1,成功;
}

/*
* 函数名：CAN1_ReceiveMsg
* 描述  ：使用CAN1接收数据
* 输入  ：buf:数据指针,最大为8个字节
* 输出  ：接收数据长度
* 调用  ：外部调用
*/
u8 CAN1_ReceiveMsg(u8 *buf)
{
    u32 i;
    CanRxMsg RxMessage;

    if (CAN_MessagePending(CAN1, CAN_FIFO0) == 0)
    {
        return 0;		//没有接收到数据,直接退出 
    }

    CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	

    for (i = 0; i<8; i++)
    {
        buf[i] = RxMessage.Data[i];
    }
    return RxMessage.DLC;
}

#endif

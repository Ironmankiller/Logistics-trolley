//-----------------------------------------------------------------------------
// Uart_Command.c
//
//  Created on	: 2016-10-25
//      Author	: DGVY
//		version	: V1.0
//		brief	: Uart指令系统
//-----------------------------------------------------------------------------
// Attention:
//      指令格式：
//          Command_Head+Command_Body=([xxxx[,xxxx[,...]]])

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Uart_Command.h"
#include "Printf_Uart.h"
#include <string.h>
#include "My_Flag.h"
#include "stm32f4xx.h"
#include "Uart_Command_Dispose.h"


//-----------------------------------------------------------------------------
// Private Typedef
//-----------------------------------------------------------------------------

#define CONNOND_PARAMETER_LEN 20

typedef struct
{
    u8 Command_Head[COMMOND_HEAD_LEN];          //指令头
    u8 Command_Body[COMMOND_BODY_LEN];          //指令体
    u8 Parameter[CONNOND_PARAMETER_LEN];        //指令参数
}Command_struct;

//Command_struct command;

//-----------------------------------------------------------------------------
// Private Define
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Macro
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Private Variables
//-----------------------------------------------------------------------------
u8 Command_Buffer[USART_REC_LEN];


//-----------------------------------------------------------------------------
// Private Function Prototypes
//-----------------------------------------------------------------------------

static void Command_Transform(Command_struct *Command);
static ErrorStatus Command_Format_Verify(void);

static void Command_Analysis(Command_struct *Command);

static u8 Command_Head_Analysis(u8 *command_head);
static u8 Command_Body_Analysis(u8 *command_body, u8 head_num);

//===============================================================================


//-----------------------------------------------------------------------------
// Extern Function
// 外部函数
//-----------------------------------------------------------------------------


/*****************************************************************************\
* Function Name   : Uart_Command_Do
* Input           : None
* Output          : None
* Return          : None
* Description     : 指令的解析与执行
\*****************************************************************************/
void Uart_Command_Do(void)
{
    if (Flag_UART_RX == Ready)   //uart接收完一次指令
    {

        Command_struct command;
        memset(&command,'\0',sizeof(Command_struct));

        strcpy((char *)Command_Buffer, (char *)USART_RX_BUF);   //copy str

        // 检验指令格式是否正确
        if (Command_Format_Verify() == ERROR)
        {
            printf("\r\nERROR: Command's format is undefined!\r\n\r\n");
            Flag_UART_RX = Not_Ready;
            return;
        }
         
        // 分解指令
        Command_Transform(&command);

        // 解析指令并执行
        Command_Analysis(&command);

        Flag_UART_RX = Not_Ready;
    }

}

//===============================================================================


//-----------------------------------------------------------------------------
// Static Function
// 内部函数
//-----------------------------------------------------------------------------

/*****************************************************************************\
* Function Name   : Command_Transform
* Input           : None
* Output          : None
* Return          : Command_struct  [指令结构体]
* Description     : 分解指令，将收到的字符串分段
\*****************************************************************************/
static void Command_Transform(Command_struct *Command)
{
    u8 i = 0;
    u8 j = 0;

    // 提取指令头
    while (Command_Buffer[i] != '+')
    {
        Command->Command_Head[j] = Command_Buffer[i];
        j++;
        i++;
    }
    i++;
    j = 0;

    // 提取指令体
    while (Command_Buffer[i] != '=')
    {
        Command->Command_Body[j] = Command_Buffer[i];
        j++;
        i++;
    }
    i++;
    i++;    //跳过左括号
    j = 0;

    // 提取指令参数
    while (Command_Buffer[i] != ')')
    {
        Command->Parameter[j] = Command_Buffer[i];
        j++;
        i++;
    }

}

/*****************************************************************************\
* Function Name   : Command_Format_Verify
* Input           : None
* Output          : None
* Return          : ErrorStatus   [ERROR,指令格式有错；SUCCESS,指令格式正确]
* Description     : 判断指令格式是否有问题
\*****************************************************************************/
static ErrorStatus Command_Format_Verify(void)
{
    u8 i = 0;
    u8 verify = 0;

    while (1)
    {
        if ((Command_Buffer[i] == '\0') || (i == USART_REC_LEN))
        {
            return ERROR;
        }

        if ((Command_Buffer[i] == '+')&&verify == 0)
        {
            verify = 1;
        }

        if ((Command_Buffer[i] == '=') && verify == 1)
        {
            verify = 2;
        }

        if ((Command_Buffer[i] == '(') && verify == 2)
        {
            verify = 3;
        }

        if ((Command_Buffer[i] == ')') && verify == 3)
        {
            return SUCCESS;
        }
        
        i++;
    }
}

/*****************************************************************************\
* Function Name   : Command_Analysis
* Input           : None
* Output          : None
* Return          : None
* Description     : 解析指令头，将命令对应到不同的执行函数
\*****************************************************************************/
static void Command_Analysis(Command_struct *command)
{
    u8 head_num = 0;
    head_num = Command_Head_Analysis(command->Command_Head); //解析命令头
    if (head_num == 0)
    {
        printf("\r\nERROR: Command-head is not found!\r\n\r\n");
        return;
    }

    u8 body_num = 0;
    body_num = Command_Body_Analysis(command->Command_Body,head_num); //解析命令体
    if (body_num == 0)
    {
        printf("\r\nERROR: Command-body is not found!\r\n\r\n");
        return;
    }

    // 通过函数指针调用相关命令函数
    Command_Function[head_num - 1][body_num - 1](command->Parameter);

}

/*****************************************************************************\
* Function Name   : Command_Head_Analysis
* Input           : None
* Output          : None
* Return          : None
* Description     : 解析指令头，将命令对应到不同的执行函数
\*****************************************************************************/
static u8 Command_Head_Analysis(u8 *command_head)
{
    u8 i = 0;
    while (strcmp((const char*)Command_Head[i], (const char*)command_head) != 0)
    {
        i++;

        if (i > COMMOND_HEAD_LEN)
        {
            return 0;
        }
    }
    i++;
    return i;
}

/*****************************************************************************\
* Function Name   : Command_Body_Analysis
* Input           : None
* Output          : None
* Return          : None
* Description     : 解析指令体
\*****************************************************************************/
static u8 Command_Body_Analysis(u8 *command_body, u8 head_num)
{
    u8 i = 0;
    while (strcmp((const char*)Command_Body[head_num-1][i], (const char*)command_body) != 0)
    {
        i++;

        if (i > COMMOND_BODY_LEN)
        {
            return 0;
        }
    }
    i++;
    return i;
}

//===============================================================================




/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/

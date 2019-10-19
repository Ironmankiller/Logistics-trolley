//-----------------------------------------------------------------------------
// Queue.h
//
//  Created on	: 2016-7-14
//      Author	: DGVY
//		version	: V1.0
//		brief	: 队列操作的接口
//-----------------------------------------------------------------------------
// Attention:
//        链式队列
//        通过单链表的方式实现

//-----------------------------------------------------------------------------
// Define to prevent recursive inclusion
//-----------------------------------------------------------------------------
//#pragma once
#ifndef __QUEUE_H
#define __QUEUE_H

#ifdef __cplusplus  
extern "C" {
#endif  

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "APP\Math\Vect\Vect.h"
#include "My_Flag.h"

//-----------------------------------------------------------------------------
// Private Define
//-----------------------------------------------------------------------------
#define QUEUE_TYPE PointTypeDef     //队列元素类型

#define QueueState Flag
#define Empty MY_TRUE         //空队列
#define Not_Empty MY_FALSE    //非空

//-----------------------------------------------------------------------------
// Private Typedef
//-----------------------------------------------------------------------------

// 队列节点结构体
typedef struct QUEUE_NODE
{
    QUEUE_TYPE value;
    struct QUEUE_NODE *next;
}QueueNodeTypeDef;

// 队列对象结构体
typedef struct QUEUE
{
    QueueNodeTypeDef *front;
    QueueNodeTypeDef *rear;
}QueueTypeDef;

//-----------------------------------------------------------------------------
// Exported constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Extern Variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Exported functions
//-----------------------------------------------------------------------------

/* 
** @function: Queue_Init
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 初始化一个队列
*/
void Queue_Init(register QueueTypeDef *queue);

/*
** @function: Queue_Destory
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 销毁一个队列
*/
eBoolTypedef Queue_Destory(register QueueTypeDef *queue);

/*
** @function: Queue_Insert
** @Input   : QueueTypeDef* [队列名指针]
**            QUEUE_TYPE    [要添加的元素]
** @brief   : 将一个元素添加到指定队列中
*/
eBoolTypedef Queue_Insert(register QueueTypeDef *queue, QUEUE_TYPE value);

/*
** @function: Queue_Get_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : QUEUE_TYPE    [队列的第一个元素]
** @brief   : 获取队列第一个元素的值，但不改变队列结构
*/
eBoolTypedef Queue_Get_First(register QUEUE_TYPE *output_value,
							 register QueueTypeDef *queue);

/*
** @function: Queue_GetDel_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : QUEUE_TYPE    [队列的第一个元素]
** @brief   : 获取队列第一个元素的值，并将其删除
*/
eBoolTypedef Queue_GetDel_First(register QUEUE_TYPE *output_value,
								register QueueTypeDef *queue);

/*
** @function: Queue_Delete_First
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 删除队列第一个元素的值
*/
eBoolTypedef Queue_Delete_First(register QueueTypeDef *queue);

/*
** @function: Queue_Delete_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : 空返回 Empty ,非空返回 Not_Empty
** @brief   : 判断队列是否为空
*/
QueueState Queue_Is_Empty(register QueueTypeDef *queue);


#ifdef __cplusplus  
}
#endif 

#endif /* __QUEUE_H */
/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/

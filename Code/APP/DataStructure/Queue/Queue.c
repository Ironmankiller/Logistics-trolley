/**
  ******************************************************************************
  * @file    Queue.c
  * @author  DGVY
  * @version v1.1
  * @date    2016-11-25
  * @brief   数据结构 -- 队列
  ******************************************************************************
  * @attention
	*	相关联的文件：
	*		1.Queue.h -- Code/APP/DataStructure/Queue.h
  ******************************************************************************
  * @history
  	* v1.1
  	*	加入错误判断
  ******************************************************************************
**/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "Queue.h"
#include "My_Flag.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

//===============================================================================

/*
** @function: Queue_Init
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 初始化一个队列
*/
void Queue_Init(register QueueTypeDef *queue)
{
    queue->front = NULL;
    queue->rear  = NULL;
}

/*
** @function: Queue_Destory
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 销毁一个队列
*/
eBoolTypedef Queue_Destory(register QueueTypeDef *queue)
{
	eBoolTypedef judje = MY_FALSE;
    while(Queue_Is_Empty(queue) != Empty)
    {
    	judje = Queue_Delete_First(queue);
    	if(!judje)
    	{
    		return MY_FALSE;
    	}
    }
    return MY_TRUE;
}

/*
** @function: Queue_Insert
** @Input   : QueueTypeDef* [队列名指针]
**            QUEUE_TYPE    [要添加的元素]
** @brief   : 将一个元素添加到指定队列中
*/
eBoolTypedef Queue_Insert(register QueueTypeDef *queue, QUEUE_TYPE value)
{
    register QueueNodeTypeDef *p_new = NULL;

    // 为新节点分配内存
    p_new = (QueueNodeTypeDef *)malloc(sizeof(QueueNodeTypeDef));
    if (p_new == NULL)
    {
        return MY_FALSE;     //内存分配失败
    }

    //为新节点赋值
    p_new->value = value;
    p_new->next = NULL;

    // 在队列中添加新节点
    if (queue->front == NULL)    //空队列
    {
        queue->front = p_new;
        queue->rear = p_new;
    }
    else
    {
        queue->rear->next = p_new;
        queue->rear = p_new;
    }

    return MY_TRUE;
}

/*
** @function: Queue_Get_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : QUEUE_TYPE    [队列的第一个元素]
** @brief   : 获取队列第一个元素的值，但不改变队列结构
*/
eBoolTypedef Queue_Get_First(register QUEUE_TYPE *output_value,
							 register QueueTypeDef *queue)
{
    if (Queue_Is_Empty(queue) == Empty)
    {
        return MY_FALSE;
    }

    memcpy(output_value,queue,sizeof(QUEUE_TYPE));

    return MY_TRUE;
}

/*
** @function: Queue_GetDel_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : QUEUE_TYPE    [队列的第一个元素]
** @brief   : 获取队列第一个元素的值，并将其删除
*/
eBoolTypedef Queue_GetDel_First(register QUEUE_TYPE *output_value,
								register QueueTypeDef *queue)
{
    if (Queue_Is_Empty(queue) == Empty)
    {
        return MY_FALSE;
    }

    Queue_Get_First(output_value,queue);          //获取第一个元素
    Queue_Delete_First(queue);		 				  //删除第一个元素

    return MY_TRUE;
}

/*
** @function: Queue_Delete_First
** @Input   : QueueTypeDef* [队列名指针]
** @brief   : 删除队列第一个元素的值
*/
eBoolTypedef Queue_Delete_First(register QueueTypeDef *queue)
{
    if (Queue_Is_Empty(queue) == Empty)
    {
        return MY_FALSE;
    }

    register QueueNodeTypeDef *p_current = queue->front;

    queue->front = p_current->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(p_current);

    return MY_TRUE;
}

/*
** @function: Queue_Delete_First
** @Input   : QueueTypeDef* [队列名指针]
** @return  : 空返回 Empty ,非空返回 Not_Empty
** @brief   : 判断队列是否为空
*/
QueueState Queue_Is_Empty(register QueueTypeDef *queue)
{
    if (queue->front == NULL)
    {
        return Empty;
    }
    else
    {
        return Not_Empty;
    }
}


/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/

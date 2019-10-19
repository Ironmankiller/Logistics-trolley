/*
 * ANO_DT.h
 *
 *  Created on: 2016年11月26日
 *      Author: DGVY
 */

#ifndef __ANO_DT_H
#define __ANO_DT_H

#include "stm32f4xx.h"
#include <stdint.h>

/**
  * @name   Argument_Load
  * @param  None
  * @retval None
  * @brief  从FLASH里读取PID数据
  */
void Argument_Load(void);

void Argument_Init(void);

// Uart 命令行保存PID参数
void Commond_Save(void);

/**
  * @name   ANO_DT_Send_Status
  * @param  尚未看懂
  * @retval None
  * @brief  发送 姿态
  */
void ANO_DT_Send_Status(float angle_rol, float angle_pit, float angle_yaw,
						s32 alt, uint8_t fly_model, uint8_t armed);

/**
  * @name   ANO_DT_Send_Senser
  * @param  各传感器的数据
  * @retval None
  * @brief  传感器数据---此处用于给示波器发送数据
  */
void ANO_DT_Send_Senser(s16 a_x,s16 a_y,s16 a_z,
						s16 g_x,s16 g_y,s16 g_z,
						s16 m_x,s16 m_y,s16 m_z
						,s32 bar);

void ANO_DT_Do(void);

#endif /* __ANO_DT_H */

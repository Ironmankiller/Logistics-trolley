//-----------------------------------------------------------------------------
// My_Math.h
//
//  Created on	: 2016-10-7
//      Author	: DGVY
//		version	: V1.0
//		brief	: 常用的数学计算函数
//-----------------------------------------------------------------------------
// Attention:
//

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#ifndef __MY_MATH_H
#define __MY_MATH_H

//-----------------------------------------------------------------------------
// Define to prevent recursive inclusion
//-----------------------------------------------------------------------------
//计算用宏
#include <math.h>
#include <sys.h>
#include "string.h"
#include "stdlib.h"
#include "arm_math.h"

// 圆周率
#define pi 										3.141592653589793f

// 弧度变角度
#define rad2deg(rad) 							( (double)(rad)/pi*180 )

// 角度变弧度
#define deg2rad(deg) 							( (double)(deg)*pi/180 )

// 平方
#define square(x)    							( (x)*(x) )

// 开方
#define my_sqrt(x)  							__sqrtf(x)

// 三角函数
#define my_sin(x)  								sinf(deg2rad(x))
#define my_cos(x)  								cosf(deg2rad(x))

// 反三角函数
#define my_acos(x) 								rad2deg(acosf(x))
#define my_asin(x) 								rad2deg(asinf(x))

// 最大值
#define my_max(a,b) 							( (a)>(b) ? (a) : (b) )

// 最小值
#define my_min(a,b)								( (a)<(b) ? (a) : (b) )

// 求绝对值
#define my_abs(num)  							(((num)>0)?(num):-(num)) 
// 求两个数差值的绝对值
#define my_minus_abs(num1,num2)                 ((num1>num2)?(num1-num2):(num2-num1))

#define my_limit( x,min,max ) ( (x) < (min)  ? (min) : ( (x) > (max) ? (max) : (x) ) )

//浮点数相关
#define float_equal(a,b) 					    ( fabsf((a)-(b))<0.000001f )
#define float_not_equal(a,b) 			        ( fabsf((a)-(b))>0.000001f )
#define float_greater(a,b) 				        ( (a)-(b)>0.000001f )
#define float_smaller(a,b) 				        ( (a)-(b)<-0.000001f )

float string_to_float(u8 *str);

char *myitoa(int value, char *string, int radix);

int myatoi(char *str);

void swap(int *a, int *b);

void quicksort(int array[], int begin, int end);

void Bubble_Sort(uint32_t _data[], uint32_t size);

float fast_atan2(float y, float x);

float To_180_degrees(float x);

float invSqrt(float x);

#endif /* __MY_MATH_H */
/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/

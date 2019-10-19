//-----------------------------------------------------------------------------
// DataConverTypeDef.h
//
//  Created on	: 2016-7-14
//      Author	: DGVY
//		version	: V1.0
//		brief	: 数据转换联合体
//                  4个元素的数组、uint32_t、int32_t、float，之间相互转化
//                  8个元素的数组、uint64_t、int64_t、double，之间相互转化
//-----------------------------------------------------------------------------
// Attention:
//

//-----------------------------------------------------------------------------
// Define to prevent recursive inclusion
//-----------------------------------------------------------------------------
//#pragma once
#ifndef __DATACONVERTYPEDEF_H
#define __DATACONVERTYPEDEF_H

#ifdef __cplusplus  
extern "C" {
#endif  

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <stdint.h>

//数据转换联合体 -- 32位：
typedef union
{
    uint8_t  u8_form[4];
    uint32_t u32_form;
    int32_t  s32_form;
    float    float_form;
}uDataConvert32TypeDef;

//数据转换联合体 -- 64位：
typedef union
{
    uint8_t  u8_form[8];
    uint64_t u64_form;
    int64_t  s64_form;
    double   float_form;
}uDataConvert64TypeDef;

#ifdef __cplusplus  
}
#endif 

#endif /* __DATACONVERTYPEDEF_H */
/******************* (C) COPYRIGHT 2016 DGVY **********END OF FILE***********/


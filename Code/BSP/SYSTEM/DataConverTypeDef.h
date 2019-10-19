//-----------------------------------------------------------------------------
// DataConverTypeDef.h
//
//  Created on	: 2016-7-14
//      Author	: DGVY
//		version	: V1.0
//		brief	: ����ת��������
//                  4��Ԫ�ص����顢uint32_t��int32_t��float��֮���໥ת��
//                  8��Ԫ�ص����顢uint64_t��int64_t��double��֮���໥ת��
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

//����ת�������� -- 32λ��
typedef union
{
    uint8_t  u8_form[4];
    uint32_t u32_form;
    int32_t  s32_form;
    float    float_form;
}uDataConvert32TypeDef;

//����ת�������� -- 64λ��
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


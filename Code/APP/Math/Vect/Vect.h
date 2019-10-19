#ifndef _VECT_H_
#define _VECT_H_
#include "stm32f4xx.h"
#include <stdint.h>

/*编码器点坐标
*提高精度使用double型
*/
typedef struct 
{
    float x;
    float y;
	float  Angle;
}EncodePointTypeDef;

// 表示坐标
typedef struct
{
    float x;
    float y;
}CoordsTypeDef;

// 某一点的信息，包括坐标和角度
typedef struct
{
    CoordsTypeDef Coords;   //坐标
    float   Angle;  //角度
}PointTypeDef;

/*向量*/
typedef struct
{
    CoordsTypeDef Start; //起始点
    CoordsTypeDef End;   //终止点
}
VectorTypeDef;

/**
 * @Description : 向量相关的函数声明
 */
void SetPoint(PointTypeDef * p_point, int32_t x, int32_t y, float ang);
void set_vect(VectorTypeDef *p_vect, CoordsTypeDef *p_start, CoordsTypeDef *p_end);
void get_foot_point(VectorTypeDef *p_vect, CoordsTypeDef *p0, CoordsTypeDef * foot_point);
VectorTypeDef p2v_vertical_vect(VectorTypeDef *p_vect, CoordsTypeDef *p0);
VectorTypeDef p2v_parallel_vect(VectorTypeDef *p_vect, CoordsTypeDef *p0);
float get_vect_len(VectorTypeDef *p_vect);
float get_point_dis(CoordsTypeDef * p1, CoordsTypeDef * p2);
float is_vect_same_direction(VectorTypeDef * v1, VectorTypeDef * v2);
float get_v2x_ang(VectorTypeDef *p_vect);


#endif

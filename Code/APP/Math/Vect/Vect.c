#include "vect.h"
#include <string.h>
#include "APP\Math\My_Math.h"

/**
 * @function    : 
 * @Description : 根据两点设置一个向量
**/
void set_vect(VectorTypeDef *p_vect, CoordsTypeDef *p_start, CoordsTypeDef *p_end)
{
	memcpy(&p_vect->Start, p_start, sizeof(CoordsTypeDef));
	memcpy(&p_vect->End, p_end,   sizeof(CoordsTypeDef));
}

void SetPoint(PointTypeDef * p_point, int32_t x, int32_t y, float ang)
{
    p_point->Coords.x = x;
    p_point->Coords.y = y;
    p_point->Angle = ang;
}

/**
 * @function    : 
 * @Description : 求得向量外一点到向量的垂线的垂足
 * @param       : 向量的指针，点坐标的指针，垂足的指针
**/
void get_foot_point(VectorTypeDef *p_vect, CoordsTypeDef *p0, CoordsTypeDef * foot_point)
{
	float x0 = p0->x;
	float y0 = p0->y;					/*点坐标*/
	float x1 = p_vect->Start.x;
	float y1 = p_vect->Start.y;		  	/*向量起点坐标*/
	float x2 = p_vect->End.x;
	float y2 = p_vect->End.y;			/*向量终点坐标*/
	if (x1 == x2)						/*若目标直线垂直于X轴*/
	{
		foot_point->x = x1;
		foot_point->y = y0;
	} 
	else if (y1 == y2)					/*若目标直线垂直于Y轴*/
	{
		foot_point->x = x0;
		foot_point->y = y1;  				
	} 
	else								/*一般情况*/
	{
		foot_point->x = p0->x - (float)((y1 - y2)*(x0*y1 - x1*y0 - x0*y2 + x2*y0 + x1*y2 - x2*y1))
		                        /(square(x1) - 2*x1*x2 + square(x2) + square(y1) - 2*y1*y2 + square(y2));
		foot_point->y = (float)(x1-x2) * (foot_point->x-x0) / (y2-y1) + y0; 
	}
}

/*
 *	p2v_vertical_vect:	求点到向量的垂线向量
 *	p_vect:				向量
 *	p0:					点坐标
 *	返回值：			垂足到点的向量
 */
VectorTypeDef p2v_vertical_vect(VectorTypeDef *p_vect, CoordsTypeDef *p0)
{
    CoordsTypeDef f_point;    /*footpoint*/
	VectorTypeDef  ver_vect;	/*垂线向量*/

	get_foot_point(p_vect, p0, &f_point);	/*垂线交点及相对位置*/
	set_vect(&ver_vect, &f_point, p0);	    /*点以及垂线交点构成的向量*/
							
	return  ver_vect;
}

/*
 *	p2v_parallel_vect:	获取点到向量起点在平行于向量方向的向量
 *	p_vect:				向量指针
 *	p0：				点坐标
 *	返回值：			向量起点到点到向量垂足的向量
 */
VectorTypeDef p2v_parallel_dist(VectorTypeDef *p_vect, CoordsTypeDef *p0)
{
    CoordsTypeDef f_point;			/*垂线交点*/
	VectorTypeDef  par_vect;	   	/*经过点的平行于输入向量的向量*/
				
	get_foot_point(p_vect, p0, &f_point);		/*求垂线交点*/
	set_vect(&par_vect, &p_vect->Start, &f_point);	/*平行向量*/
	//判断两个向量是不是同向，如果是反向那么返回的距离值是负的
	
	return par_vect;
    // if ((p_vect->End.x - p_vect->Start.x) * (par_vect.End.x - par_vect.Start.x) + 
        // (p_vect->End.y - p_vect->Start.y) * (par_vect.End.x - par_vect.Start.y) < 0)
        // return -get_vect_len(&par_vect);				/*长度*/	
    // else 
        // return get_vect_len(&par_vect);
}

//获取向量长度
float get_vect_len(VectorTypeDef *p_vect)
{
	float x, y, l;
	x = p_vect->End.x - p_vect->Start.x;
	y = p_vect->End.y - p_vect->Start.y;
	l = my_sqrt(square(x)+square(y));
	return l;
}

/*
 *	get_point_dis:	获取两点的距离
 *	p1:				点坐标
 *	p2：			点坐标
 *	返回值：	两点的距离
 */
float get_point_dis(CoordsTypeDef * p1, CoordsTypeDef * p2)
{
    VectorTypeDef vect;
    memcpy(&vect.Start,p1,sizeof(CoordsTypeDef));
    memcpy(&vect.End,p2,sizeof(CoordsTypeDef));
    return get_vect_len(&vect);
}

/*
 *	is_vect_same_direction:	判断两个向量是否同向
 *	v1:				向量指针
 *	v2：			向量指针
 *	返回值：	大于0 同向 等于0 垂直 小于0 反向
 */
float is_vect_same_direction(VectorTypeDef * v1, VectorTypeDef * v2)
{
    float X1 = v1->End.x - v1->Start.x;
    float Y1 = v1->End.y - v1->Start.y;
    float X2 = v2->End.x - v2->Start.x;
    float Y2 = v2->End.y - v2->Start.y;
    if (X1 * X2 + Y1 * Y2 >= 0)
    {
        return 1;    //垂直或同向
    }
    else
    {
        return 0;    //反向
    }
}


/*
 *	get_v2x_ang:	获取向量与X轴的夹角
 *	p_vect:			向量指针
 *	返回值：		向量与X轴的夹角【0-360°）(前开后闭)
 */
float get_v2x_ang(VectorTypeDef *p_vect)
{
	float x = p_vect->End.x - p_vect->Start.x;		/*输入向量的X分量*/
	float y = p_vect->End.y - p_vect->Start.y;		/*输入向量的Y分量*/
	
	/*判断角度区间，并用向量乘法计算与X轴的角度*/
	if (x == 0 && y == 0)
	{
		return 0;
	}								
	else if (y>=0)
	{
		return  my_acos((float)x / my_sqrt(square(x)+square(y)));
	}
	else
	{
		return 360 - my_acos((float)x / my_sqrt(square(x)+square(y)));
	}
}





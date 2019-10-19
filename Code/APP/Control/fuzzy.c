#include "fuzzy.h"
#include <stdint.h>

int32_t NB = 0, NM = 1, NS = 2, ZO = 3, PS = 4, PM = 5, PB = 6;
float EFF[7] = { -400,-200,-60,0,60,200,400 };  /*E语言变量特征点*/
float ECFF[5] = { -8,-3,0,3,8 };          /*EC语言变量特征点*/

/**************************************************************************
* 函数功能：模糊控制器
* 函数输入：误差E、误差微分EC（E_LAST - E_NOW）
* 函数输出：控制量U
*************************************************************************/
float Fuzzy_P(float E,float EC)
{
    float UFF[7] = { -0.015f,-0.007f,-0.005,0.0f,0.001f,0.005f,0.01f};  /*输出量U语言值特征点*/
    //float UFF[7] = { -0.004f,-0.003f,-0.002f,0.0f,0.001f,0.002f,0.005f };  /*输出量U语言值特征点*/

   /*模糊控制规则表*/
    int rule[7][5] =
    {
        {PM, PS, NS, NM, NB},
        {PS, ZO, NS, NM, NB},  
        {PB, PS, ZO, NS, NB},  
        {PB, ZO, ZO, ZO, NB},  
        {PB, PS, ZO, NS, NB},  
        {PS, ZO, NS, NM, NB},  
        {PM, PS, NS, NM, NB},
    };  //经验P参数

    EC = -EC; //(spy:这里是测量部件装反了吗？)
    float U=0;  /*偏差,偏差微分以及输出值的精确量*/ 
    float PF[2]={0},DF[2]={0},UF[4]={0};
     /*偏差,偏差微分以及输出值的隶属度*/ 
    int Pn=0,Dn=0,Un[4]={0};
    float t1=0,t2=0,t3=0,t4=0,temp1=0,temp2=0;
    /*隶属度的确定*/ 
    /*根据PD的指定语言值获得有效隶属度*/ 
    if(E>EFF[0] && E<EFF[6])
    {
        if(E<=EFF[1])
        {
            Pn=-2;
            PF[0]=(EFF[1]-E)/(EFF[1]-EFF[0]);
        }
        else if(E<=EFF[2])
        {
            Pn=-1;
            PF[0]=(EFF[2]-E)/(EFF[2]-EFF[1]);
        }
        else if(E<=EFF[3])
        {
            Pn=0;
            PF[0]=(EFF[3]-E)/(EFF[3]-EFF[2]);
        }
        else if(E<=EFF[4])
        {
            Pn=1;
            PF[0]=(EFF[4]-E)/(EFF[4]-EFF[3]);
        }
        else if(E<=EFF[5])
        {
            Pn=2;
            PF[0]=(EFF[5]-E)/(EFF[5]-EFF[4]);
        }
        else if(E<=EFF[6])
        {
            Pn=3;
            PF[0]=(EFF[6]-E)/(EFF[6]-EFF[5]);
        }
    }
    //不在给定的区间内
    else if(E<=EFF[0])
    {
        Pn=-2;
        PF[0]=1;
    }
    else if(E>=EFF[6])
    {
        Pn=3;
        PF[0]=0;
    }

    PF[1]=1-PF[0];


    //判断D的隶属度
    if(EC>ECFF[0]&&EC<ECFF[4])
    {
        if(EC<=ECFF[1])
        {
            Dn=-2;
            DF[0]=(ECFF[1]-EC)/(ECFF[1]-ECFF[0]);
        }
        else if(EC<=ECFF[2])
        {
            Dn=-1;
            DF[0]=(ECFF[2]-EC)/(ECFF[2]-ECFF[1]);
        }
        else if(EC<=ECFF[3])
        {
            Dn=0;
            DF[0]=(ECFF[3]-EC)/(ECFF[3]-ECFF[2]);
        }
        else if(EC<=ECFF[4])
        {
            Dn=1;
            DF[0]=(ECFF[4]-EC)/(ECFF[4]-ECFF[3]);
        }
    }
    //不在给定的区间内
    else if (EC<=ECFF[0])
    {
        Dn=-2;
        DF[0]=1;
    }
    else if(EC>=ECFF[4])
    {
        Dn=1;
        DF[0]=0;
    }

    DF[1]=1-DF[0];

    /*使用误差范围优化后的规则表rule[7][7]*/ 
    /*输出值使用13个隶属函数,中心值由UFF[7]指定*/ 
    /*一般都是四个规则有效*/ 
    Un[0]=rule[Pn-1+3][Dn-1+3]; 
    Un[1]=rule[Pn+3][Dn-1+3]; 
    Un[2]=rule[Pn-1+3][Dn+3]; 
    Un[3]=rule[Pn+3][Dn+3]; 
 
    if(PF[0]<=DF[0])    //求小   
        UF[0]=PF[0];
    else
        UF[0]=DF[0];
    if(PF[1]<=DF[0])
        UF[1]=PF[1];
    else
        UF[1]=DF[0];
    if(PF[0]<=DF[1])
        UF[2]=PF[0];
    else
        UF[2]=DF[1];
    if(PF[1]<=DF[1])
        UF[3]=PF[1];
    else
        UF[3]=DF[1];
    /*同隶属函数输出语言值求大*/ 
    if(Un[0]==Un[1])
    {
        if(UF[0]>UF[1])
            UF[1]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[2])
    {
        if(UF[0]>UF[2])
            UF[2]=0;
        else
            UF[0]=0;
    }
    if(Un[0]==Un[3])
    {
        if(UF[0]>UF[3])
            UF[3]=0;
        else
            UF[0]=0;
    }
    if(Un[1]==Un[2])
    {
        if(UF[1]>UF[2])
            UF[2]=0;
        else
            UF[1]=0;
    }
    if(Un[1]==Un[3])
    {
        if(UF[1]>UF[3])
            UF[3]=0;
        else
            UF[1]=0;
    } 
    if(Un[2]==Un[3])
    {
        if(UF[2]>UF[3])
            UF[3]=0;
        else
            UF[2]=0;
    }
    t1=UF[0]*UFF[Un[0]];
    t2=UF[1]*UFF[Un[1]];
    t3=UF[2]*UFF[Un[2]];
    t4=UF[3]*UFF[Un[3]];
    temp1=t1+t2+t3+t4;
    temp2=UF[0]+UF[1]+UF[2]+UF[3];//模糊量输出
    U=temp1/temp2;
    return U;
}

float Fuzzy_D(float E, float EC)
{
    //只用到了后5个值
    float UFF[7] = { -8,-8,-9,0,1,7,20 };  /*输出量U语言值特征点(根据赛道类型选择不同的输出值)*/

    /*模糊控制规则表*/
    //  int rule[7][5]=
    //  {
    ////ec -2  -1   0   1   2   //   e
    //    { 1,  1,  1,  0,  0}, //  -3
    //    { 4,  2,  1,  0,  0}, //  -2
    //    { 6,  4,  1,  1,  0}, //  -1
    //    { 3,  3,  0,  3,  3}, //   0
    //    { 0,  1,  1,  4,  6}, //   1
    //    { 0,  0,  1,  2,  4}, //   2
    //    { 0,  0,  1,  1,  1}  //   3
    //  };

    //int rule[7][5]=
    //{
    ////ec -2  -1   0   1   2   //   e
    //    { 0,  0,  1,  1,  1}, //  -3
    //    { 0,  0,  1,  2,  4}, //  -2
    //    { 0,  1,  2,  4,  6}, //  -1
    //    { 3,  3,  0,  3,  3}, //   0
    //    { 6,  4,  2,  1,  0}, //   1
    //    { 4,  2,  1,  0,  0}, //   2
    //    { 1,  1,  1,  0,  0}  //   3
    //};  //智能车上用到的规则表

    //int rule[7][5] =
    //{
    //    {PB,  PB,  NB,  NB,  NM},
    //    {PB,  PM,  NM,  ZO,  PS},
    //    {PM,  PM,  NS,  PM,  PB},
    //    {ZO,  ZO,  ZO,  ZO,  ZO},
    //    {PB,  PM,  NS,  PM,  PM},
    //    {PS,  ZO,  NM,  PM,  PB},
    //    {NM,  NB,  NB,  PB,  PB},
    //};

    int rule[7][5] =
    {
        { PM, PB, ZO, ZO, NS },
        { PS, PM, ZO, PB, PB },
        { ZO, PS, ZO, ZO, NS },
        { NS, ZO, ZO, ZO, NS },
        { NS, ZO, ZO, PS, ZO },
        { PB, PB, ZO, PM, PS },
        { NS, ZO, ZO, PB, PM }
    };  //经验D参数

    //EC = -EC;
    float U = 0;  /*偏差,偏差微分以及输出值的精确量*/
    float PF[2] = { 0 }, DF[2] = { 0 }, UF[4] = { 0 };
    /*偏差,偏差微分以及输出值的隶属度*/
    int Pn = 0, Dn = 0, Un[4] = { 0 };
    float t1 = 0, t2 = 0, t3 = 0, t4 = 0, temp1 = 0, temp2 = 0;
    /*隶属度的确定*/
    /*根据PD的指定语言值获得有效隶属度*/
    if (E>EFF[0] && E<EFF[6])
    {
        if (E <= EFF[1])
        {
            Pn = -2;
            PF[0] = (EFF[1] - E) / (EFF[1] - EFF[0]);
        }
        else if (E <= EFF[2])
        {
            Pn = -1;
            PF[0] = (EFF[2] - E) / (EFF[2] - EFF[1]);
        }
        else if (E <= EFF[3])
        {
            Pn = 0;
            PF[0] = (EFF[3] - E) / (EFF[3] - EFF[2]);
        }
        else if (E <= EFF[4])
        {
            Pn = 1;
            PF[0] = (EFF[4] - E) / (EFF[4] - EFF[3]);
        }
        else if (E <= EFF[5])
        {
            Pn = 2;
            PF[0] = (EFF[5] - E) / (EFF[5] - EFF[4]);
        }
        else if (E <= EFF[6])
        {
            Pn = 3;
            PF[0] = (EFF[6] - E) / (EFF[6] - EFF[5]);
        }
    }
    //不在给定的区间内
    else if (E <= EFF[0])
    {
        Pn = -2;
        PF[0] = 1;
    }
    else if (E >= EFF[6])
    {
        Pn = 3;
        PF[0] = 0;
    }

    PF[1] = 1 - PF[0];


    //判断D的隶属度
    if (EC>ECFF[0] && EC<ECFF[4])
    {
        if (EC <= ECFF[1])
        {
            Dn = -2;
            DF[0] = (ECFF[1] - EC) / (ECFF[1] - ECFF[0]);
        }
        else if (EC <= ECFF[2])
        {
            Dn = -1;
            DF[0] = (ECFF[2] - EC) / (ECFF[2] - ECFF[1]);
        }
        else if (EC <= ECFF[3])
        {
            Dn = 0;
            DF[0] = (ECFF[3] - EC) / (ECFF[3] - ECFF[2]);
        }
        else if (EC <= ECFF[4])
        {
            Dn = 1;
            DF[0] = (ECFF[4] - EC) / (ECFF[4] - ECFF[3]);
        }
    }
    //不在给定的区间内
    else if (EC <= ECFF[0])
    {
        Dn = -2;
        DF[0] = 1;
    }
    else if (EC >= ECFF[4])
    {
        Dn = 1;
        DF[0] = 0;
    }

    DF[1] = 1 - DF[0];

    /*使用误差范围优化后的规则表rule[7][7]*/
    /*输出值使用13个隶属函数,中心值由UFF[7]指定*/
    /*一般都是四个规则有效*/
    Un[0] = rule[Pn - 1 + 3][Dn - 1 + 3];
    Un[1] = rule[Pn + 3][Dn - 1 + 3];
    Un[2] = rule[Pn - 1 + 3][Dn + 3];
    Un[3] = rule[Pn + 3][Dn + 3];

    if (PF[0] <= DF[0])    //求小   
        UF[0] = PF[0];
    else
        UF[0] = DF[0];
    if (PF[1] <= DF[0])
        UF[1] = PF[1];
    else
        UF[1] = DF[0];
    if (PF[0] <= DF[1])
        UF[2] = PF[0];
    else
        UF[2] = DF[1];
    if (PF[1] <= DF[1])
        UF[3] = PF[1];
    else
        UF[3] = DF[1];
    /*同隶属函数输出语言值求大*/
    if (Un[0] == Un[1])
    {
        if (UF[0]>UF[1])
            UF[1] = 0;
        else
            UF[0] = 0;
    }
    if (Un[0] == Un[2])
    {
        if (UF[0]>UF[2])
            UF[2] = 0;
        else
            UF[0] = 0;
    }
    if (Un[0] == Un[3])
    {
        if (UF[0]>UF[3])
            UF[3] = 0;
        else
            UF[0] = 0;
    }
    if (Un[1] == Un[2])
    {
        if (UF[1]>UF[2])
            UF[2] = 0;
        else
            UF[1] = 0;
    }
    if (Un[1] == Un[3])
    {
        if (UF[1]>UF[3])
            UF[3] = 0;
        else
            UF[1] = 0;
    }
    if (Un[2] == Un[3])
    {
        if (UF[2]>UF[3])
            UF[3] = 0;
        else
            UF[2] = 0;
    }
    t1 = UF[0] * UFF[Un[0]];
    t2 = UF[1] * UFF[Un[1]];
    t3 = UF[2] * UFF[Un[2]];
    t4 = UF[3] * UFF[Un[3]];
    temp1 = t1 + t2 + t3 + t4;
    temp2 = UF[0] + UF[1] + UF[2] + UF[3];//模糊量输出
    U = temp1 / temp2;
    return U;
}


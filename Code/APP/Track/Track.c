#include "Track.h"
#include "BSP\74HC165\74HC165.h"
#include "BSP\Mecanum\Mecanum.h"
#include "BSP\SYSTEM\usart\Printf_Uart.h"
static float findCenter(u16 data);
static u16 findNum(u16 data);

void Track_Init(void){
    HC165_Init();
}

//     ͷ
//   B-^-C
//   |   | 
//   A-^-D
//    β
void Track_Read(void){

    u16 side_AB;
    u16 side_BC;
    u16 side_CD;
    u16 side_DA;

    
    SHLD_L;
    HC165_delay();
    SHLD_H;
    HC165_delay();
    
    side_CD = HC165_Read_2Byte();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    side_BC = HC165_Read_2Byte();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    side_AB = HC165_Read_2Byte();
    HC165_CLK_L;
    HC165_delay();
    HC165_CLK_H;
    HC165_delay();
    side_DA = HC165_Read_2Byte();
    
    Mecanum.side_AB = findNum(side_AB);
    Mecanum.side_BC = findNum(side_BC);
    Mecanum.side_CD = findNum(side_CD);
    Mecanum.side_DA = findNum(side_DA);
//    Mecanum.side_BC = findCenter(side_BC)*0.3f+Mecanum.side_BC*0.7f;
//    Mecanum.side_CD = findCenter(side_CD)*0.3f+Mecanum.side_CD*0.7f;
//    Mecanum.side_DA = findCenter(side_DA)*0.3f+Mecanum.side_DA*0.7f;
    

//    for(int i=15;i>=0;i--){
//        temp = side_AB;
//        temp >>= i;
//        temp &= 1;
//        if(temp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    u1_printf(" %.2f ",Mecanum.side_AB);
//    for(int i=15;i>=0;i--){
//        temp = side_BC;
//        temp >>= i;
//        temp &= 1;
//        if(temp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    u1_printf(" %.2f ",Mecanum.side_BC);
//    for(int i=15;i>=0;i--){
//        temp = side_CD;
//        temp >>= i;
//        temp &= 1;
//        if(temp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    u1_printf(" %.2f ",Mecanum.side_CD);
//    for(int i=15;i>=0;i--){
//        temp = side_DA;
//        temp >>= i;
//        temp &= 1;
//        if(temp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    u1_printf(" %.2f ",Mecanum.side_DA);

//    u1_printf("\r\n");
    
}

static u16 findNum(u16 data) {
    u16 temp;
    u16 num = 0;
    for(int i=0;i<16;i++){
        temp = data>>i;
        if((temp&0x0001) == 0) {
            num++;
        }
    }
    return num;
}

static float findCenter(u16 data){
    u16 temp;
    u8 begin=0;
    u8 end=0;
    for(int i=0;i<16;i++){
        temp = data>>i;
        if((temp&0x0001) == 0){         //只有当该位是0该位的下一位也是0才认为检测到线
            if(((data>>i+1)&1)==0 && i!=15){
                begin = i;
                break;
            }
        }
    }
    
    for(int i=15;i>=0;i--){
        temp = data>>i;
        if((temp&1) == 0){         //只有当该位是0该位的上一位也是0才认为检测到线
            if(((data>>i-1)&1)==0 && i!=0){
                end = i;
                break;
            }
        }
    }
    
//    for(int i=15;i>=0;i--){
//        temp = data;
//        temp >>= i;
//        temp &= 1;
//        if(temp == 1){
//            u1_printf("1");
//        }else{
//            u1_printf("0");
//        }
//    }
//    u1_printf("\r\n");
//    u1_printf("begin=%d\tend=%d\r\n",begin,end);
    return (begin+end)/2.0f;
}

#ifndef __Steer_MOTO_H
#define __Steer_MOTO_H

void setmotor_pwm(unsigned short num,unsigned short pwm,unsigned short time);

//四个关节舵机中值
#define arm1_offset     1885
#define arm2_offset     1600
#define arm3_offset     1180
#define arm4_offset     1400

//舵机角度控制
#define arm1(degree)    setmotor_pwm(1, (float)(degree)*2000.0f/270.0f + arm1_offset, 100)
#define arm2(degree)    setmotor_pwm(2, (float)(degree)*2000.0f/270.0f + arm2_offset,100)
#define arm3(degree)    setmotor_pwm(3, -(float)(degree)*2000.0f/270.0f + arm3_offset,100)
#define arm4(degree)    setmotor_pwm(4, -(float)(degree)*2000.0f/270.0f + arm4_offset,100)

#endif

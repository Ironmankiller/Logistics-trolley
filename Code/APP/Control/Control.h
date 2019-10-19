#ifndef __CONTROL_H
#define __CONTROL_H

#include "My_Flag.h"
#include <stdint.h>
#include "param.h"

void ready_control(void);
void goToDeparture_control(void);
void grab_control(void);
void goToDestination_control(void);
void place_control(void);
void backToDeparture_control(void);
void back_control(void);

extern int flag;
extern Flag Init_flag;
#endif // !__CONTROL_H

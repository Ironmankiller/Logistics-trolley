#ifndef __CONTROL_H
#define __CONTROL_H

#include "My_Flag.h"
#include <stdint.h>
#include "param.h"

void ready_control(void);
void scan_control(void);
void goToDeparture_control(void);
void grabFromDeparture_control(void);
void goToDestination_control(void);
void placeToProcessing_control(void);
void backToDeparture_control(void);
void grabFromProcessing_control(void);
void goToFinish_control(void);
void placeToFinish_control(void);
void goToProcessing_control(void);
void backToProcessing_control(void);
void back_control(void);

extern int flag;
extern Flag Init_flag;
#endif // !__CONTROL_H

#ifndef __KEY_BOARD_H
#define __KEY_BOARD_H

#include <stdint.h>

#define S1                   0x01
#define S2                   0x02
#define S3                   0x03
#define S4                   0x04
#define S5                   0x05
#define S6                   0x06
#define S7                   0x07
#define S8                   0x08
#define S9                   0x09
#define S10                  0x0a
#define S11                  0x0b
#define S12                  0x0c
#define S13                  0x0d
#define S14                  0x0e
#define S15                  0x0f
#define S16                  0x10


typedef struct
{
    uint8_t getKeyVal : 7;
    uint8_t fkeyLock : 1;
}BUTTON_TypeDef;

extern BUTTON_TypeDef Matrix_Keyboard;

void Key_Board_Iint(void);
uint8_t scan_keypad2(BUTTON_TypeDef *p);




#endif

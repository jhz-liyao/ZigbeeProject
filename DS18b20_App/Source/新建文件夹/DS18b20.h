#ifndef __DS18B20_H__
#define __DS18B20_H__


#include "hal_board.h"
#define DS18B20_PORT P0
#define DS18B20_BIT  7
#define DS18B20_PIN  P0_7
#define DS18B20_SEL  P0SEL
#define DS18B20_DIR  P0DIR

extern void hal_DS18B20_Init (void);
extern bool hal_DS18B20_Start(void);
extern uint8 hal_DS18B20_Read(void);
extern void hal_DS18B20_Write(uint8 data);
#endif
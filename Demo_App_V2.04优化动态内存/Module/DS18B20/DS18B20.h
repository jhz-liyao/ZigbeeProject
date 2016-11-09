#ifndef __DS18B20_H__
#define __DS18B20_H__ 
#include "hal_board.h"

//---------------CMD--------------------
//ROM操作命令
#define READ_ROM 0x33     
#define MATH_ROM 0x55    
#define SEARCH_ROM 0xF0
#define SKIP_ROM 0xCC   //跳过ROM寻址
#define WING_ENA 0xEC
//存储器操作指令
#define TEMP_SAMPLING 0x44      //启动一次温度转换
#define READ_RAM      0xBE      //读取内部RAM中9字节数据
#define WRITE_RAM     0x4E      //写入上限和下限温度数据
#define MOVE_E2PROM   0x48      //将上下限数据写入EEPROM
#define FETCH_E2PROM  0xB8      //从EEPROM恢复上下限数据
#define READ_POWER    0xB4      //读取供电模式 0寄生 1正常供电


#define DS18B20_PORT P0
#define DS18B20_BIT  7
#define DS18B20_PIN  P0_7
#define DS18B20_SEL  P0SEL
#define DS18B20_DIR  P0DIR

extern void hal_DS18B20_Init (void);
extern bool hal_DS18B20_Start(void);
extern uint8 hal_DS18B20_Read(void);
extern void hal_DS18B20_Write(uint8 data);
extern void hal_DS18B20_Sampling(void);
extern float hal_DS18B20_GetTemp(void);
#endif
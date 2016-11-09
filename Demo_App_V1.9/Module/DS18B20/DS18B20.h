#ifndef __DS18B20_H__
#define __DS18B20_H__ 
#include "hal_board.h"

//---------------CMD--------------------
//ROM��������
#define READ_ROM 0x33     
#define MATH_ROM 0x55    
#define SEARCH_ROM 0xF0
#define SKIP_ROM 0xCC   //����ROMѰַ
#define WING_ENA 0xEC
//�洢������ָ��
#define TEMP_SAMPLING 0x44      //����һ���¶�ת��
#define READ_RAM      0xBE      //��ȡ�ڲ�RAM��9�ֽ�����
#define WRITE_RAM     0x4E      //д�����޺������¶�����
#define MOVE_E2PROM   0x48      //������������д��EEPROM
#define FETCH_E2PROM  0xB8      //��EEPROM�ָ�����������
#define READ_POWER    0xB4      //��ȡ����ģʽ 0���� 1��������


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
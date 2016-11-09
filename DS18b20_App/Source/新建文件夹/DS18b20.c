#include "DS18b20.h"
#include "tool.h"

#define SET_IO_INPUT    RESET(DS18B20_DIR,DS18B20_BIT)
#define SET_IO_OUTPUT     SET(DS18B20_DIR,DS18B20_BIT)
/****************************************************
*      ������: hal_DS18B20_Init
*        ����: ��ʼ��18b20����
*        ����: ��
*      ����ֵ: ��
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
void hal_DS18B20_Init(void){ 
    RESET(DS18B20_SEL,DS18B20_BIT);
    SET_IO_OUTPUT;
    DS18B20_PIN = 1;
}

/****************************************************
*      ������: hal_DS18B20_Start
*        ����: ģ������
*        ����: ��
*      ����ֵ: ��
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
bool hal_DS18B20_Start(void){
    uint16 cnt = 30;
    SET_IO_OUTPUT;
    DS18B20_PIN = 0;
    MicroWait(750);
    DS18B20_PIN = 1;
    SET_IO_INPUT;
    while(DS18B20_PIN && cnt--)
        MicroWait(10);  
    if(cnt){
        SET_IO_OUTPUT;
        DS18B20_PIN = 1;
        return true;
    }
    return false;
}

/****************************************************
*      ������: hal_DS18B20_Read
*        ����: ��IC����
*        ����: ��
*      ����ֵ: ����
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
uint8 hal_DS18B20_Read(void){
    uint8 i, data = 0;
    SET_IO_OUTPUT; 
    for(i = 0; i < 8; i++){ 
        data >>= 1;
        DS18B20_PIN = 0;
        MicroWait(15);
        SET_IO_INPUT;
        MicroWait(20);
        if(DS18B20_PIN) data |= 0x80;
        MicroWait(20);
        SET_IO_OUTPUT;
        DS18B20_PIN = 1;
    }
    return data;
}

/****************************************************
*      ������: hal_DS18B20_Write
*        ����: дIC����
*        ����: ��
*      ����ֵ: ��
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
void hal_DS18B20_Write(uint8 data){
    uint8 i;
    SET_IO_OUTPUT; 
    for(i = 0; i < 8; i++){ 
      DS18B20_PIN = 0;
      MicroWait(15);
      DS18B20_PIN = data & 0x01?1:0;
      MicroWait(45);
      DS18B20_PIN = 1;
      MicroWait(10);
      data >>= 1;
    } 
}

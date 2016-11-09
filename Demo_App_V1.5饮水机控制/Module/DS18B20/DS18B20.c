#include "DS18B20.h"
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
*      ����ֵ: ��ʼ�źųɹ�״̬
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
bool hal_DS18B20_Start(void){
    uint16 cnt = 30;
    SET_IO_OUTPUT;
    DS18B20_PIN = 0;
    MicroWait(600);
    DS18B20_PIN = 1;
    SET_IO_INPUT;
    while(DS18B20_PIN && cnt--)
        MicroWait(10);  
    if(cnt){
        SET_IO_OUTPUT;
        DS18B20_PIN = 1;
        MicroWait(100);
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
    
    HAL_DISABLE_INTERRUPTS();
    for(i = 0; i < 8; i++){ 
        data >>= 1; 
        SET_IO_OUTPUT; 
        DS18B20_PIN = 0;
        MicroWait(1);
        SET_IO_INPUT;
        MicroWait(4);
        if(DS18B20_PIN) data |= 0x80; 
        MicroWait(55);
        
    }
    HAL_ENABLE_INTERRUPTS();
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
    HAL_DISABLE_INTERRUPTS();
    for(i = 0; i < 8; i++){ 
      SET_IO_OUTPUT;
      DS18B20_PIN = 0;
      MicroWait(1);
      DS18B20_PIN = data & 0x01;
      MicroWait(59);
      DS18B20_PIN = 1;
      MicroWait(10);
      data >>= 1;
    }
    HAL_ENABLE_INTERRUPTS();
}

/****************************************************
*      ������: hal_DS18B20_Sampling
*        ����: ����IC�¶Ȳ���
*        ����: ��
*      ����ֵ: ��
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
void hal_DS18B20_Sampling(void){ 
    hal_DS18B20_Start();
    hal_DS18B20_Write(SKIP_ROM);
    hal_DS18B20_Write(TEMP_SAMPLING);  
}

/****************************************************
*      ������: hal_DS18B20_GetTemp
*        ����: ��ȡ18B20���¶���Ϣ
*        ����: ��
*      ����ֵ: ��
*    ������Ϣ: author liyao��2016/9/28 
****************************************************/
float hal_DS18B20_GetTemp(void){
    uint16 temp;   //����ߵ�8λ ���� 
    hal_DS18B20_Start();
    hal_DS18B20_Write(SKIP_ROM);
    hal_DS18B20_Write(READ_RAM);
//    V1 = hal_DS18B20_Read();    //��λ
//    V2 = hal_DS18B20_Read();    //��λ
    
    temp  = hal_DS18B20_Read();    //��λ 
    temp |= hal_DS18B20_Read() <<8; //��λ 
    return temp * 0.0625;
    //return ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 
}



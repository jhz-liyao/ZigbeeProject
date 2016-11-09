#include "DS18b20.h"
#include "tool.h"

#define SET_IO_INPUT    RESET(DS18B20_DIR,DS18B20_BIT)
#define SET_IO_OUTPUT     SET(DS18B20_DIR,DS18B20_BIT)
/****************************************************
*      函数名: hal_DS18B20_Init
*        功能: 初始化18b20引脚
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
****************************************************/
void hal_DS18B20_Init(void){ 
    RESET(DS18B20_SEL,DS18B20_BIT);
    SET_IO_OUTPUT;
    DS18B20_PIN = 1;
}

/****************************************************
*      函数名: hal_DS18B20_Start
*        功能: 模块启动
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
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
*      函数名: hal_DS18B20_Read
*        功能: 读IC数据
*        参数: 无
*      返回值: 数据
*    创建信息: author liyao，2016/9/28 
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
*      函数名: hal_DS18B20_Write
*        功能: 写IC数据
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
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

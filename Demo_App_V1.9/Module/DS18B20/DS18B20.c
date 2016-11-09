#include "DS18B20.h"
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
*      返回值: 起始信号成功状态
*    创建信息: author liyao，2016/9/28 
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
*      函数名: hal_DS18B20_Read
*        功能: 读IC数据
*        参数: 无
*      返回值: 数据
*    创建信息: author liyao，2016/9/28 
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
*      函数名: hal_DS18B20_Write
*        功能: 写IC数据
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
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
*      函数名: hal_DS18B20_Sampling
*        功能: 启动IC温度采样
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
****************************************************/
void hal_DS18B20_Sampling(void){ 
    hal_DS18B20_Start();
    hal_DS18B20_Write(SKIP_ROM);
    hal_DS18B20_Write(TEMP_SAMPLING);  
}

/****************************************************
*      函数名: hal_DS18B20_GetTemp
*        功能: 获取18B20的温度信息
*        参数: 无
*      返回值: 无
*    创建信息: author liyao，2016/9/28 
****************************************************/
float hal_DS18B20_GetTemp(void){
    uint16 temp;   //定义高低8位 缓冲 
    hal_DS18B20_Start();
    hal_DS18B20_Write(SKIP_ROM);
    hal_DS18B20_Write(READ_RAM);
//    V1 = hal_DS18B20_Read();    //低位
//    V2 = hal_DS18B20_Read();    //高位
    
    temp  = hal_DS18B20_Read();    //低位 
    temp |= hal_DS18B20_Read() <<8; //高位 
    return temp * 0.0625;
    //return ((V1 >> 4)+((V2 & 0x07)*16)); //转换数据 
}



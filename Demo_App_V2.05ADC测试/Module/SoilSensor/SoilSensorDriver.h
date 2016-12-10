#ifndef __WATERMACHINEDRIVER_H__
#define __WATERMACHINEDRIVER_H__ 
#include "ioCC2530.h"
#include "hal_defs.h"
#include "Protocol.h"
#include "ProtocolFrame.h" 
#define MIN_VALUE 3300
#define MAX_VALUE 7900
#define MAX_SCOPE (MAX_VALUE - MIN_VALUE)

//计算百分比
#define PERCENT_VALUE(ReturnValue,Adc_Value)\
do{uint32_t tmp_value= 0;\
    if(Adc_Value > MAX_VALUE)\
      tmp_value = MAX_VALUE;  \
    else if(Adc_Value < MIN_VALUE)\
      tmp_value = MIN_VALUE; \
    else \
      tmp_value = Adc_Value; \
    ReturnValue = (uint8_t)((MAX_SCOPE - (tmp_value - MIN_VALUE)) * 100 / MAX_SCOPE);}while(0)
      
      
      
typedef struct {
    uint16_t Short_Addr; 
    uint16_t Sensor1; 
    uint16_t Sensor2; 
    uint16_t Sensor3; 
    uint16_t Sensor4; 
    uint16_t Sensor5; 
    uint16_t Sensor6; 
    uint16_t Sensor7;
}SoilSensor_Info_T;

extern SoilSensor_Info_T SoilSensorM ;
extern void SoilSensor_Init(void);
extern void SoilSensor_Read(void);
//extern void Cmd_P_Handle(Protocol_Info_T* pi);
#endif

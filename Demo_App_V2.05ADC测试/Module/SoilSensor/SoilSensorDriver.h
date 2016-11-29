#ifndef __WATERMACHINEDRIVER_H__
#define __WATERMACHINEDRIVER_H__ 
#include "ioCC2530.h"
#include "hal_defs.h"
#include "Protocol.h"
#include "ProtocolFrame.h" 
 
 
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
